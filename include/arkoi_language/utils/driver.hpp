#pragma once

#include "pretty_diagnostics/source.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace arkoi::utils {
/**
 * @brief Generate a unique temporary filesystem path.
 *
 * This function creates a path in the system's temporary directory, suitable
 * for storing intermediate compilation artifacts like object files or assembly.
 *
 * @return A `std::filesystem::path` to a unique temporary location.
 */
std::filesystem::path generate_temp_path();

/**
 * @brief Compile a source unit through the entire compilation pipeline.
 *
 * This function coordinates the front-end (parsing, semantic analysis),
 * intermediate representation generation, optimizations, and back-end (code generation).
 *
 * @param source The source input with diagnostic support from `pretty_diagnostics`.
 * @param il_ostream Optional output stream for the intermediate language (IL).
 *                   If provided, the IL will be printed to this stream after generation.
 * @param cfg_ostream Optional output stream for the control-flow graph (CFG).
 *                    If provided, the CFG will be printed in DOT format.
 * @param asm_ostream Optional output stream for the generated x86-64 assembly.
 *
 * @return The compilation status code (0 on success, non-zero on failure).
 * @see assemble, link
 */
int32_t compile(
    const std::shared_ptr<pretty_diagnostics::Source>& source,
    std::ofstream* il_ostream,
    std::ofstream* cfg_ostream,
    std::ofstream* asm_ostream
);

/**
 * @brief Execute a compiled binary and capture its result.
 *
 * @param path The filesystem path to the executable binary.
 *
 * @return The exit code returned by the executed program.
 */
int32_t run_binary(const std::string& path);

/**
 * @brief Link object files into a final executable output.
 *
 * This typically invokes the system linker (e.g., `ld` or `gcc`).
 *
 * @param object_files A list of paths to object files to be linked.
 * @param output Output stream for capturing linker messages (stdout/stderr).
 * @param verbose If true, enables verbose output from the linker.
 *
 * @return The linker exit code (0 on success, non-zero on failure).
 * @see assemble
 */
int32_t link(const std::vector<std::string>& object_files, std::ofstream& output, bool verbose = false);

/**
 * @brief Assemble an assembly file into a relocatable object file.
 *
 * This typically invokes a system assembler (e.g., `nasm` or `as`).
 *
 * @param input_file Path to the assembly source file (.s or .asm).
 * @param output Output stream for capturing assembler messages (stdout/stderr).
 * @param verbose If true, enables verbose output from the assembler.
 *
 * @return The assembler exit code (0 on success, non-zero on failure).
 * @see compile, link
 */
int32_t assemble(const std::string& input_file, std::ofstream& output, bool verbose = false);
} // namespace arkoi::utils

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
