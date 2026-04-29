#include <fstream>
#include <iostream>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <vector>

#include "argparse/argparse.hpp"

#include "arkoi_language/utils/driver.hpp"

using namespace arkoi;

struct PipelineFileUnit {
    std::filesystem::path input_path;
    std::filesystem::path cfg_path;
    std::filesystem::path asm_path;
    std::filesystem::path obj_path;
    std::filesystem::path il_path;
};

struct PipelineContext {
    std::vector<PipelineFileUnit> file_units;
    std::filesystem::path output_path;

    bool emit_cfg = false;
    bool emit_asm = false;
    bool emit_il = false;
    bool verbose = false;
};

enum class PipelineMode {
    CompileOnly,
    AssembleOnly,
    LinkOnly,
    Run,
    Full
};

using Stage = std::function<int(PipelineContext&)>;

int compile_stage(PipelineContext& context) {
    for (const auto& file_unit : context.file_units) {
        const auto source = std::make_shared<pretty_diagnostics::FileSource>(file_unit.input_path);

        std::ofstream il_file, cfg_file, asm_file;

        std::ofstream* il_ostream = nullptr;
        if (context.emit_il) {
            il_file.open(file_unit.il_path);
            if (!il_file.is_open()) return 1;
            il_ostream = &il_file;
        }

        std::ofstream* cfg_ostream = nullptr;
        if (context.emit_cfg) {
            cfg_file.open(file_unit.cfg_path);
            if (!cfg_file.is_open()) return 1;
            cfg_ostream = &cfg_file;
        }

        std::ofstream* asm_ostream = nullptr;
        if (context.emit_asm) {
            asm_file.open(file_unit.asm_path);
            asm_ostream = &asm_file;
        }

        int compile_exit = utils::compile(source, il_ostream, cfg_ostream, asm_ostream, context.verbose);
        if (compile_exit != 0) {
             return compile_exit;
        }
    }

    return 0;
}

int assemble_stage(PipelineContext& context) {
    for (const auto& file_unit : context.file_units) {
        const auto obj_path = file_unit.obj_path;
        const auto asm_path = file_unit.asm_path;
        
        auto obj_ostream = std::ofstream(obj_path);

        int assemble_exit = utils::assemble(asm_path, obj_ostream, context.verbose);
        if (assemble_exit != 0) {
             return assemble_exit;
        }
    }

    return 0;
}

int link_stage(PipelineContext& context) {
    auto output_ostream = std::ofstream(context.output_path);

    std::vector<std::string> obj_paths;
    for (const auto& file_unit : context.file_units) {
        obj_paths.push_back(file_unit.obj_path);
    }

    return utils::link(obj_paths, output_ostream, context.verbose);
}

int run_stage(PipelineContext& context) {
    int result = utils::run_binary(context.output_path);
    std::filesystem::remove(context.output_path);
    return result; 
}

int main(const int argc, const char* argv[]) {
    argparse::ArgumentParser argument_parser(PROJECT_NAME, PROJECT_VERSION, argparse::default_arguments::none);

    argument_parser.add_description(
        "The Arkoi Compiler is a lightweight experimental compiler for the Arkoi\n"
        "Programming Language, designed to explore a mix of Python and C programming\n"
        "principles. It is primarily a learning and experimentation tool for testing\n"
        "new language features, compiler techniques, and language design concepts."
    );

    argument_parser.add_argument("-h", "--help")
                   .action([&](const auto&) {
                        std::cout << argument_parser.help().str();
                        std::exit(0);
                    })
                   .help("Shows the help message and exits")
                   .flag();
    argument_parser.add_argument("--version")
                   .action([&](const auto&) {
                        std::cout << PROJECT_VERSION << std::endl;
                        std::exit(0);
                    })
                   .help("Prints version information and exits")
                   .flag();

    argument_parser.add_group("Input and output control");
    argument_parser.add_argument("inputs")
                   .help("All input files that should be compiled\n\b")
                   .nargs(argparse::nargs_pattern::at_least_one);
    argument_parser.add_argument("--output", "-o")
                   .help("The output file name of the compiled files\n\b")
                   .default_value("a.out");
    argument_parser.add_argument("--verbose", "-v")
                   .help("Print (on the standard error output) the commands executed to run the stages of compilation")
                   .flag();

    argument_parser.add_group("Compilation modes");
    argument_parser.add_argument("--compile-only", "-c")
                   .help("Only compile but do not assemble.\nFor each source an assembly file \".s\" is generated")
                   .flag();
    argument_parser.add_argument("--assemble-only", "-a")
                   .help("Only compile and assemble, but do not link.\nFor each source an object file \".o\" is generated")
                   .flag();
    argument_parser.add_argument("--link-only", "-l")
                   .help("Only compile, assemble and link, but do not run the program afterwards")
                   .flag();
    argument_parser.add_argument("--run", "-r")
                   .help("Compile, assemble, link and run the program afterwards")
                   .flag();

    argument_parser.add_group("Output control of compilation stages");
    argument_parser.add_argument("--emit-asm")
                   .help("Emits the assembly code of each source to a file ending in \".s\"")
                   .flag();

    argument_parser.add_argument("--emit-cfg")
                   .help("Emits the Control-Flow-Graph of each source to a file ending in \".dot\"")
                   .flag();

    argument_parser.add_argument("--emit-il")
                   .help("Emits the Intermediate Language of each source to a file ending in \".il\"")
                   .flag();

    try {
        argument_parser.parse_args(argc, argv);
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        std::cerr << argument_parser;
        return 1;
    }

    std::vector<PipelineFileUnit> file_units;    
    for (const auto& input_path : argument_parser.get<std::vector<std::string>>("inputs")) {
        const auto base_path = std::filesystem::path(input_path).replace_extension();

        file_units.push_back({
            .input_path = input_path,
            .cfg_path = std::filesystem::path(base_path).replace_extension(".dot"),
            .asm_path = std::filesystem::path(base_path).replace_extension(".s"),
            .obj_path = std::filesystem::path(base_path).replace_extension(".o"),
            .il_path = std::filesystem::path(base_path).replace_extension(".il"),
        });
    }

    PipelineContext context {
        .file_units = file_units,
        .output_path = argument_parser.get<std::string>("output"),
        .emit_cfg = argument_parser.get<bool>("--emit-cfg"),
        .emit_asm = argument_parser.get<bool>("--emit-asm"),
        .emit_il = argument_parser.get<bool>("--emit-il"),
        .verbose = argument_parser.get<bool>("--verbose"),
    };

    PipelineMode mode;
    if(argument_parser.get<bool>("--compile-only")) {
        mode = PipelineMode::CompileOnly;
    } else if (argument_parser.get<bool>("--assemble-only")) {
        mode = PipelineMode::AssembleOnly;
    } else if (argument_parser.get<bool>("--link-only")) {
        mode = PipelineMode::LinkOnly;
    } else if (argument_parser.get<bool>("--run")) {
        mode = PipelineMode::Run;
    } else {
        mode = PipelineMode::Full;
    }

    std::vector<Stage> pipeline;
    switch (mode) {
        case PipelineMode::CompileOnly:
            pipeline = {compile_stage};
            break;
        case PipelineMode::AssembleOnly:
            pipeline = {compile_stage, assemble_stage};
            break;
        case PipelineMode::Full:
        case PipelineMode::LinkOnly:
            pipeline = {compile_stage, assemble_stage, link_stage};
            break;
        case PipelineMode::Run:
            pipeline = {compile_stage, assemble_stage, link_stage, run_stage};
            break;
    }

    for (const auto& stage : pipeline) {
        int stage_exit = stage(context);
        if (stage_exit != 0) {
            return stage_exit;
        }
    }

    return 0;
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
