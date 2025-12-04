#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include "argparse/argparse.hpp"

#include "front/parser.hpp"
#include "front/scanner.hpp"
#include "il/cfg_printer.hpp"
#include "il/generator.hpp"
#include "il/il_printer.hpp"
#include "opt/constant_folding.hpp"
#include "opt/constant_propagation.hpp"
#include "opt/dead_code_elimination.hpp"
#include "opt/pass.hpp"
#include "opt/simplify_cfg.hpp"
#include "sem/name_resolver.hpp"
#include "sem/type_resolver.hpp"
#include "x86_64/generator.hpp"
#include "il/analyses.hpp"

using namespace arkoi;

std::string get_base_path(const std::string &path);

std::string read_file(const std::string &path);

int32_t run_binary(const std::string &path);

int32_t compile(
    const std::string &source,
    std::ofstream *il_ostream,
    std::ofstream *cfg_ostream,
    std::ofstream *asm_ostream
);

int main(const int argc, const char *argv[]) {
    argparse::ArgumentParser argument_parser(PROJECT_NAME, PROJECT_VERSION, argparse::default_arguments::none);

    argument_parser.add_argument("-h", "--help")
            .action([&](const auto &) {
                std::cout << argument_parser.help().str();
                std::exit(0);
            })
            .help("Shows the help message and exits")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    argument_parser.add_argument("--version")
            .action([&](const auto &) {
                std::cout << PROJECT_VERSION << std::endl;
                std::exit(0);
            })
            .help("Prints version information and exits")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    argument_parser.add_argument("-v")
            .help("Print (on the standard error output) the commands executed to run the stages of compilation")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);

    argument_parser.add_argument("inputs")
            .help("All input files that should be compiled\n\b")
            .nargs(argparse::nargs_pattern::at_least_one);
    argument_parser.add_argument("-o", "--output")
            .help("The output file name of the compiled files\n\b")
            .nargs(argparse::nargs_pattern::optional)
            .default_value("./a.out");

    argument_parser.add_argument("-S")
            .help("Only compile but do not assemble.\nFor each source an assembly file \".s\" is generated")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    argument_parser.add_argument("-c")
            .help("Only compile and assemble, but do not link.\nFor each source an object file \".o\" is generated")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    argument_parser.add_argument("-r")
            .help("Compile, assemble, link and run the program afterwards")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);

    argument_parser.add_argument("-print-asm")
            .help("Print the assembly code of each source to a file ending in \".s\"")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    argument_parser.add_argument("-print-cfg")
            .help("Print the Control-Flow-Graph of each source to a file ending in \".dot\"")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    argument_parser.add_argument("-print-il")
            .help("Print the Intermediate Language of each source to a file ending in \".il\"")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);

    try {
        argument_parser.parse_args(argc, argv);
    } catch (const std::exception &error) {
        std::cerr << error.what() << std::endl;
        std::cerr << argument_parser;
        return 1;
    }

    const auto inputs = argument_parser.get<std::vector<std::string>>("inputs");
    const auto output = argument_parser.get<std::string>("output");
    const auto verbose = argument_parser.get<bool>("-v");

    const auto mode_S = argument_parser.get<bool>("-S");
    const auto mode_c = argument_parser.get<bool>("-c");
    const auto mode_r = argument_parser.get<bool>("-r");
    const bool mode_full = !mode_S && !mode_c && !mode_r;

    const auto print_cfg = argument_parser.get<bool>("-print-cfg");
    const auto print_asm = argument_parser.get<bool>("-print-asm");
    const auto print_il = argument_parser.get<bool>("-print-il");

    const bool should_assemble = !mode_S;
    const bool should_link = mode_full || mode_r;
    const bool should_run = mode_r;

    std::vector<std::string> object_files;
    for (const auto &input: inputs) {
        const auto base_path = get_base_path(input);
        const auto source = read_file(input);

        const auto il_path  = base_path + ".il";
        const auto cfg_path = base_path + ".dot";
        const auto asm_path = base_path + ".s";

        { // This block has to exist, as the files get closed automatically because of RAII
            auto il_ostream  = std::ofstream(il_path);
            auto cfg_ostream = std::ofstream(cfg_path);
            auto asm_ostream = std::ofstream(asm_path);

            const auto compile_exit = compile(
                source,
                print_il  ? &il_ostream : nullptr,
                print_cfg ? &cfg_ostream : nullptr,
                print_asm ? &asm_ostream : nullptr
            );
            if (compile_exit != 0) return compile_exit;
        }

        if (!should_assemble) continue;

        const auto obj_path = base_path + ".o";
        const auto assemble_command = "as " + asm_path + " -o " + obj_path;
        if (verbose) std::cerr << "Assemble Command: " << assemble_command << std::endl;

        const auto assemble_result = std::system(assemble_command.c_str());
        const auto assemble_exit = WEXITSTATUS(assemble_result);
        if (assemble_exit != 0) return assemble_exit;

        object_files.push_back(obj_path);
    }

    if (should_link && !object_files.empty()) {
        const auto input_objects = std::accumulate(object_files.begin(), object_files.end(), std::string{});
        const auto link_command = "ld -o " + output + " " + input_objects;
        if (verbose) std::cerr << "Linking Command: " << link_command << std::endl;

        const auto link_result = std::system(link_command.c_str());
        const auto link_exit = WEXITSTATUS(link_result);
        if (link_exit != 0) return link_exit;
    }

    if (should_run && !object_files.empty()) {
        const int32_t run_exit = run_binary(output);
        std::remove(output.c_str());
        return run_exit;
    }

    return 0;
}

int32_t compile(
    const std::string &source,
    std::ofstream *il_ostream,
    std::ofstream *cfg_ostream,
    std::ofstream *asm_ostream
) {
    front::Scanner scanner(source);
    front::Parser parser(scanner.tokenize());

    auto program = parser.parse_program();
    if (scanner.has_failed() || parser.has_failed()) return 1;

    auto name_resolver = sem::NameResolver::resolve(program);
    if (name_resolver.has_failed()) return 1;

    auto type_resolver = sem::TypeResolver::resolve(program);
    if (type_resolver.has_failed()) return 1;

    auto module = il::Generator::generate(program);

    opt::PassManager manager;
    manager.add<opt::ConstantFolding>();
    manager.add<opt::ConstantPropagation>();
    manager.add<opt::DeadCodeElimination>();
    manager.add<opt::SimplifyCFG>();
    manager.run(module);

    if (il_ostream) {
        auto il_output = il::ILPrinter::print(module);
        *il_ostream << il_output.str();
    }

    if (cfg_ostream) {
        auto cfg_output = il::CFGPrinter::print(module);
        *cfg_ostream << cfg_output.str();
    }

    auto assembly_generator = x86_64::Generator(module);
    if (asm_ostream) {
        *asm_ostream << assembly_generator.output().str();
    }

    return 0;
}

std::string get_base_path(const std::string &path) {
    const auto last_dot = path.find_last_of('.');
    if (last_dot == std::string::npos || path.substr(last_dot) != ".ark") {
        throw std::invalid_argument("This is not a valid file path with '.ark' extension.");
    }

    return path.substr(0, last_dot);
}

std::string read_file(const std::string &path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


int32_t run_binary(const std::string &path) {
    const pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork process." << std::endl;
        return 1;
    }

    if (pid == 0) {
        execl(path.c_str(), path.c_str(), nullptr);
        std::cerr << "Failed to execute binary." << std::endl;
        return 1;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        std::cerr << "Failed to wait for child process." << std::endl;
        return 1;
    }

    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    std::cerr << "Child process terminated abnormally." << std::endl;
    return 1;
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================