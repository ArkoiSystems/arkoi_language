#include <complex>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include "argparse/argparse.hpp"

#include "arkoi_language/utils/driver.hpp"
#include "arkoi_language/utils/utils.hpp"

using namespace arkoi;

int main(const int argc, const char* argv[]) {
    argparse::ArgumentParser argument_parser(PROJECT_NAME, PROJECT_VERSION, argparse::default_arguments::none);

    argument_parser.add_description(
        "The Arkoi Compiler is a lightweight experimental compiler for the Arkoi\n"
        "Programming Language, designed to explore a mix of Python and C programming\n"
        "principles. It is primarily a learning and experimentation tool for testing\n"
        "new language features, compiler techniques, and language design concepts."
    );

    argument_parser.add_argument("-h", "--help")
                   .action(
                        [&](const auto&) {
                            std::cout << argument_parser.help().str();
                            std::exit(0);
                        }
                    )
                   .help("Shows the help message and exits")
                   .flag();
    argument_parser.add_argument("--version")
                   .action(
                        [&](const auto&) {
                            std::cout << PROJECT_VERSION << std::endl;
                            std::exit(0);
                        }
                    )
                   .help("Prints version information and exits")
                   .flag();

    argument_parser.add_group("Input and output control");
    argument_parser.add_argument("inputs")
                   .help("All input files that should be compiled\n\b")
                   .nargs(argparse::nargs_pattern::at_least_one);
    argument_parser.add_argument("-o", "--output")
                   .help("The output file name of the compiled files\n\b")
                   .default_value("a.out");
    argument_parser.add_argument("-v")
                   .help("Print (on the standard error output) the commands executed to run the stages of compilation")
                   .flag();

    argument_parser.add_group("Compilation modes");
    argument_parser.add_argument("-S")
                   .help("Only compile but do not assemble.\nFor each source an assembly file \".s\" is generated")
                   .flag();
    argument_parser.add_argument("-c")
                   .help("Only compile and assemble, but do not link.\nFor each source an object file \".o\" is generated")
                   .flag();
    argument_parser.add_argument("-r")
                   .help("Compile, assemble, link and run the program afterwards")
                   .flag();

    argument_parser.add_group("Output control of compilation stages");
    argument_parser.add_argument("-print-asm")
                   .help("Print the assembly code of each source to a file ending in \".s\"")
                   .flag();
    argument_parser.add_argument("-print-cfg")
                   .help("Print the Control-Flow-Graph of each source to a file ending in \".dot\"")
                   .flag();
    argument_parser.add_argument("-print-il")
                   .help("Print the Intermediate Language of each source to a file ending in \".il\"")
                   .flag();

    try {
        argument_parser.parse_args(argc, argv);
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        std::cerr << argument_parser;
        return 1;
    }

    const auto input_paths = argument_parser.get<std::vector<std::string>>("inputs");
    const auto output_path = argument_parser.get<std::string>("output");
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
    for (const auto& input_path : input_paths) {
        const auto source = std::make_shared<pretty_diagnostics::FileSource>(input_path);
        const auto base_path = get_base_path(input_path);

        const auto il_path = base_path + ".il";
        const auto cfg_path = base_path + ".dot";
        const auto asm_path = base_path + ".s";
        const auto obj_path = base_path + ".o";

        { // This block has to exist, as the files get closed automatically because of RAII,
            // which is necessary so the files get written before commands are executed with it.
            auto il_ostream = std::ofstream(il_path);
            auto cfg_ostream = std::ofstream(cfg_path);
            auto asm_ostream = std::ofstream(asm_path);

            const auto compile_exit = driver::compile(
                source,
                print_il ? &il_ostream : nullptr,
                print_cfg ? &cfg_ostream : nullptr,
                print_asm ? &asm_ostream : nullptr
            );
            if (compile_exit != 0) return compile_exit;
        }

        if (!should_assemble) continue;

        auto obj_ostream = std::ofstream(obj_path);
        auto assemble_exit = driver::assemble(asm_path, obj_ostream, verbose);
        if (assemble_exit != 0) return assemble_exit;

        object_files.push_back(obj_path);
    }

    if (!should_link || object_files.empty()) return 0;

    { // The same RAII logic applies here.
        auto output_ostream = std::ofstream(output_path);
        auto link_exit = driver::link(object_files, output_ostream, verbose);
        if (link_exit != 0) return link_exit;
    }

    if (!should_run || object_files.empty()) return 0;

    const int32_t run_exit = driver::run_binary(output_path);
    std::remove(output_path.c_str());

    return run_exit;
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
