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

void dump_cfg(const std::string &base_path, il::Module &module);

std::string get_base_path(const std::string &path);

std::string read_file(const std::string &path);

int32_t run_binary(const std::string &path);

int main(int argc, char *argv[]) {
    argparse::ArgumentParser argument_parser(PROJECT_NAME, PROJECT_VERSION);

    argument_parser.add_argument("input")
            .help("the input path to the arkoi source file to compile.");
    argument_parser.add_argument("-o", "--output")
            .help("the output path of the compiled input path.");
    argument_parser.add_argument("-il", "--output-il").flag()
            .help("print the intermediate language to a file ending with \".il\".");
    argument_parser.add_argument("-asm", "--output-asm").flag()
            .help("print the assembly code to a file ending with \".asm\".");
    argument_parser.add_argument("-cfg", "--output-cfg").flag()
            .help("print the control flow graph to a file ending with \".dot\".");
    argument_parser.add_argument("-link", "--do-link").flag()
            .help("assemble and link the output assembly to create an executable.");
    argument_parser.add_argument("-run", "--do-run").flag()
            .help("assemble, link and run the resulting executable.");

    try {
        argument_parser.parse_args(argc, argv);
    } catch (const std::exception &error) {
        std::cerr << error.what() << std::endl;
        std::cerr << argument_parser;
        return 1;
    }

    const auto output_path = argument_parser.present<std::string>("output");
    const auto input_path = argument_parser.get<std::string>("input");

    const auto base_path = output_path ? get_base_path(output_path.value()) : get_base_path(input_path);

    const auto output_il = argument_parser.get<bool>("--output-il");
    const auto output_asm = argument_parser.get<bool>("--output-asm");
    const auto output_cfg = argument_parser.get<bool>("--output-cfg");

    const auto do_link = argument_parser.get<bool>("--do-link");
    const auto do_run  = argument_parser.get<bool>("--do-run");

    auto source = read_file(input_path);
    front::Scanner scanner(source);

    front::Parser parser(scanner.tokenize());
    auto program = parser.parse_program();

    if (scanner.has_failed() || parser.has_failed()) exit(1);

    auto name_resolver = sem::NameResolver::resolve(program);
    if (name_resolver.has_failed()) exit(1);

    auto type_resolver = sem::TypeResolver::resolve(program);
    if (type_resolver.has_failed()) exit(1);

    auto module = il::Generator::generate(program);
    if (output_il) {
        auto output = il::ILPrinter::print(module);
        std::string il_path = base_path + "_org.il";
        std::ofstream il_output(il_path);
        il_output << output.str();
        il_output.close();
    }

    if (output_cfg) dump_cfg(base_path + "_org", module);

    opt::PassManager manager;
    manager.add<opt::ConstantFolding>();
    manager.add<opt::ConstantPropagation>();
    manager.add<opt::DeadCodeElimination>();
    manager.add<opt::SimplifyCFG>();
    manager.run(module);

    if (output_il) {
        auto output = il::ILPrinter::print(module);
        std::string il_path = base_path + "_opt.il";
        std::ofstream il_output(il_path);
        il_output << output.str();
    }

    if (output_cfg) dump_cfg(base_path + "_opt", module);

    auto assembly_generator = x86_64::Generator(module);
    std::string asm_path = base_path + ".asm";
    std::ofstream asm_output(asm_path);
    asm_output << assembly_generator.output().str();
    asm_output.close();

    std::string object_path = base_path + ".o";
    std::string nasm_command = "as " + asm_path + " -o " + object_path;
    int nasm_result = std::system(nasm_command.c_str());
    if (WEXITSTATUS(nasm_result) != 0) exit(1);

    if (!output_asm) std::remove(asm_path.c_str());

    if (do_link || do_run) {
        std::string ld_command = "ld " + object_path + " -o " + base_path;
        int ld_result = std::system(ld_command.c_str());
        if (WEXITSTATUS(ld_result) != 0) exit(1);
    }

    if (do_run) return run_binary(base_path);

    return 0;
}

void dump_cfg(const std::string &base_path, il::Module &module) {
    auto cfg_output = il::CFGPrinter::print(module);

    auto dot_path = base_path + ".dot";
    auto png_path = base_path + ".png";

    std::ofstream file(dot_path);
    file << cfg_output.str();
    file.close();

    std::string assemble_command = "dot -Tpng " + dot_path + " -o " + png_path;
    int assemble_result = std::system(assemble_command.c_str());
    if (WEXITSTATUS(assemble_result) != 0) exit(1);
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