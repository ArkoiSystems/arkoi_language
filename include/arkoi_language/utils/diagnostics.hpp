#pragma once

#include <iostream>
#include <vector>

#include "pretty_diagnostics/report.hpp"

namespace arkoi::utils {

/**
 * @brief Collects and manages diagnostic reports during the compilation process.
 *
 * The `Diagnostics` class provides a centralized way to gather errors, warnings,
 * and notes from different stages of the compiler (scanning, parsing, semantic analysis).
 * It can then render these diagnostics using the `pretty_diagnostics` library.
 */
class Diagnostics {
public:
    /**
     * @brief Adds a pre-built report to the diagnostics collection.
     *
     * @param report The `pretty_diagnostics::Report` to add.
     */
    void add(pretty_diagnostics::Report report);

    /**
     * @brief Checks if any errors have been reported.
     *
     * @return `true` if there's at least one report with `Severity::Error`, `false` otherwise.
     */
    [[nodiscard]] bool has_errors() const;

    /**
     * @brief Renders all collected reports to the given output stream.
     * @param os The output stream to render to (defaults to `std::cerr`).
     */
    void render(std::ostream& os = std::cerr) const;

    /**
     * @brief Clears all collected reports.
     */
    void clear();

    /**
     * @brief Returns the collection of reports.
     *
     * @return A constant reference to the vector of reports.
     */
    [[nodiscard]] const auto& reports() const { return _reports; }

private:
    std::vector<pretty_diagnostics::Report> _reports;
};

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
