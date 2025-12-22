#pragma once

#include "pretty_diagnostics/source.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace arkoi::driver {
/**
 * @brief Generate a temporary filesystem path
 *
 * @return A filesystem path to a temporary location
 */
std::filesystem::path generate_temp_path();

/**
 * @brief Compile a source unit through the compilation pipeline
 *
 * @param source The source input with diagnostic support
 * @param il_ostream Optional output stream for the intermediate language
 * @param cfg_ostream Optional output stream for the control-flow graph
 * @param asm_ostream Optional output stream for the generated assembly
 *
 * @return The compilation exit code (0 on success, non-zero on failure)
 */
int32_t compile(
    const std::shared_ptr<pretty_diagnostics::Source>& source,
    std::ofstream* il_ostream,
    std::ofstream* cfg_ostream,
    std::ofstream* asm_ostream
);

/**
 * @brief Execute a compiled binary
 *
 * @param path The filesystem path to the binary
 *
 * @return The exit code returned by the executed program
 */
int32_t run_binary(const std::string& path);

/**
 * @brief Link object files into a final output
 *
 * @param object_files A list of paths to object files
 * @param output Output stream for linker messages
 * @param verbose Enable verbose linker output
 *
 * @return The linker exit code (0 on success, non-zero on failure)
 */
int32_t link(const std::vector<std::string>& object_files, std::ofstream& output, bool verbose = false);

/**
 * @brief Assemble an assembly file into an object file
 *
 * @param input_file Path to the assembly source file
 * @param output Output stream for assembler messages
 * @param verbose Enable verbose assembler output
 *
 * @return The assembler exit code (0 on success, non-zero on failure)
 */
int32_t assemble(const std::string& input_file, std::ofstream& output, bool verbose = false);
} // namespace arkoi::driver

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
