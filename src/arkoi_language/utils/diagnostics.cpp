#include "arkoi_language/utils/diagnostics.hpp"

#include <algorithm>

#include "pretty_diagnostics/renderer.hpp"

using namespace pretty_diagnostics;
using namespace arkoi;

void utils::Diagnostics::add(Report report) {
    _reports.push_back(std::move(report));
}

bool utils::Diagnostics::has_errors() const {
    return std::ranges::any_of(
        _reports,
        [](const auto& report) {
            return report.severity() == Severity::Error;
        }
    );
}

void utils::Diagnostics::render(std::ostream& os) const {
    for (const auto& report : _reports) {
        auto renderer = TextRenderer(report);
        report.render(renderer, os);
    }
}

void utils::Diagnostics::clear() {
    _reports.clear();
}
