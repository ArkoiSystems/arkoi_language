#include "arkoi_language/utils/driver.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

#include "arkoi_language/front/parser.hpp"
#include "arkoi_language/front/scanner.hpp"
#include "arkoi_language/il/cfg_printer.hpp"
#include "arkoi_language/il/generator.hpp"
#include "arkoi_language/il/il_printer.hpp"
#include "arkoi_language/il/ssa.hpp"
#include "arkoi_language/opt/constant_folding.hpp"
#include "arkoi_language/opt/constant_propagation.hpp"
#include "arkoi_language/opt/dead_code_elimination.hpp"
#include "arkoi_language/opt/pass.hpp"
#include "arkoi_language/opt/simplify_cfg.hpp"
#include "arkoi_language/sem/name_resolver.hpp"
#include "arkoi_language/sem/type_resolver.hpp"
#include "arkoi_language/x86_64/generator.hpp"

using namespace arkoi::driver;
using namespace arkoi;

std::string random_hex(size_t length) {
    static std::mt19937_64 rng{ std::random_device{ }() };
    static std::uniform_int_distribution dist(0, 15);
    const static auto HEX_CHARACTERS = "0123456789abcdef";

    std::string result;
    result.reserve(length);

    while (length--) result += HEX_CHARACTERS[dist(rng)];
    return result;
}

std::filesystem::path driver::generate_temp_path() {
    const auto temp_dir = std::filesystem::temp_directory_path();
    const auto unique_name = random_hex(12);
    return temp_dir / ("arkoi_" + unique_name);
}

int32_t driver::compile(
    const std::shared_ptr<pretty_diagnostics::Source>& source,
    std::ofstream* il_ostream,
    std::ofstream* cfg_ostream,
    std::ofstream* asm_ostream
) {
    utils::Diagnostics diagnostics;

    front::Scanner scanner(source, diagnostics);
    auto tokens = scanner.tokenize();

    front::Parser parser(source, std::move(tokens), diagnostics);
    auto program = parser.parse_program();

    if (diagnostics.has_errors()) {
        diagnostics.render(std::cerr);
        return 1;
    }

    auto name_resolver = sem::NameResolver(diagnostics);
    name_resolver.visit(program);
    if (diagnostics.has_errors()) {
        diagnostics.render(std::cerr);
        return 1;
    }

    auto type_resolver = sem::TypeResolver(diagnostics);
    type_resolver.visit(program);
    if (diagnostics.has_errors()) {
        diagnostics.render(std::cerr);
        return 1;
    }

    auto il_generator = il::Generator();
    il_generator.visit(program);

    auto module = il_generator.module();
    for (auto& function : module) {
        auto ssa_promoter = il::SSAPromoter(function);
        ssa_promoter.promote();
    }

    opt::PassManager manager;
    manager.add<opt::ConstantFolding>();
    manager.add<opt::ConstantPropagation>();
    manager.add<opt::DeadCodeElimination>();
    manager.add<opt::SimplifyCFG>();
    manager.run(module);

    if (il_ostream) {
        auto il_printer = il::ILPrinter(*il_ostream);
        il_printer.visit(module);
        il_ostream->flush();
    }

    if (cfg_ostream) {
        auto cfg_printer = il::CFGPrinter(*cfg_ostream);
        cfg_printer.visit(module);
        cfg_ostream->flush();
    }

    for (auto& function : module) {
        auto phi_lowerer = il::PhiLowerer(function);
        phi_lowerer.lower();
    }

    if (asm_ostream) {
        auto asm_generator = x86_64::Generator(source, module);
        *asm_ostream << asm_generator.output().str();
        asm_ostream->flush();
    }

    return 0;
}

int32_t driver::run_binary(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Binary does not exist: " << path << std::endl;
        return 1;
    }

    std::error_code error;
    std::filesystem::permissions(
        path,
        std::filesystem::perms::owner_exec,
        std::filesystem::perm_options::add,
        error
    );
    if (error) {
        std::cerr << "Failed to set permissions for binary: " << error.message() << std::endl;
        return 1;
    }

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
        const int32_t signal = 128 + WTERMSIG(status);
        std::cerr << "Child process terminated by signal: " << signal << std::endl;
        return signal;
    }

    if (WIFEXITED(status)) {
        const int32_t exit_code = WEXITSTATUS(status);
        std::cout << "Executed with exit code: " << exit_code << std::endl;
        return exit_code;
    }

    std::cerr << "Child process terminated abnormally." << std::endl;
    return 1;
}

int32_t driver::link(const std::vector<std::string>& object_files, std::ofstream& output, const bool verbose) {
    const auto temp_path = generate_temp_path().string() + ".o";

    std::ostringstream command;
    command << "ld -o " << std::quoted(temp_path);
    for (const auto& object_file : object_files) command << " " << std::quoted(object_file);

    const auto link_command = command.str();
    if (verbose) std::cerr << "STAGE=LINKING: " << link_command << std::endl;

    const auto link_result = std::system(link_command.c_str());
    if (link_result == -1) return -1;

    const auto link_exit = WEXITSTATUS(link_result);

    std::ifstream input(temp_path);
    output << input.rdbuf();
    input.close();

    std::remove(temp_path.c_str());

    return link_exit;
}

int32_t driver::assemble(const std::string& input_file, std::ofstream& output, bool verbose) {
    const auto temp_path = generate_temp_path().string() + ".o";

    std::ostringstream command;
    command << "as -o " << std::quoted(temp_path) << " " << std::quoted(input_file);

    const auto assemble_command = command.str();
    if (verbose) std::cerr << "STAGE=ASSEMBLING: " << assemble_command << std::endl;

    const auto assemble_result = std::system(assemble_command.c_str());
    if (assemble_result == -1) return -1;

    const auto assemble_exit = WEXITSTATUS(assemble_result);

    std::ifstream input(temp_path);
    output << input.rdbuf();
    input.close();

    std::remove(temp_path.c_str());

    return assemble_exit;
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
