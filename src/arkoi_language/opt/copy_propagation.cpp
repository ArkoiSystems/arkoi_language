#include "arkoi_language/opt/copy_propagation.hpp"

#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool CopyPropagation::on_block(il::BasicBlock& block) {
    bool changed = false;

    _copy_map.clear();

    for (auto& instruction : block.instructions()) {
        if (auto* assign = std::get_if<il::Assign>(&instruction)) {
            const auto* source_variable = std::get_if<il::Variable>(&assign->value());
            if (!source_variable) continue;

            il::Variable resolved_variable = *source_variable;
            if (_copy_map.contains(*source_variable)) {
                resolved_variable = _copy_map.at(*source_variable);
                assign->set_value(resolved_variable);
                changed = true;
            }

            _copy_map.insert_or_assign(assign->result(), resolved_variable);
        } else {
            changed |= _propagate(instruction);
        }
    }

    return changed;
}

bool CopyPropagation::_propagate(il::Instruction& target) {
    auto propagated = false;

    std::visit(
        match{
            [&](il::Binary& instruction) {
                propagated |= _propagate(instruction.left());
                propagated |= _propagate(instruction.right());
            },
            [&](il::Return& instruction) {
                propagated |= _propagate(instruction.value());
            },
            [&](il::Cast& instruction) {
                propagated |= _propagate(instruction.source());
            },
            [&](il::If& instruction) {
                propagated |= _propagate(instruction.condition());
            },
            [&](il::Store& instruction) {
                propagated |= _propagate(instruction.source());
            },
            [&](il::Argument& instruction) {
                propagated |= _propagate(instruction.source());
            },
            [&](il::Assign& instruction) {
                propagated |= _propagate(instruction.value());
            },
            [&](il::Call& instruction) {
                for (auto& argument : instruction.arguments()) {
                    propagated |= _propagate(argument);
                }
            },
            [&](il::Phi& instruction) {
                for (auto& variable : instruction.incoming() | std::views::values) {
                    propagated |= _propagate(variable);
                }
            },
            [&](il::Alloca&) { },
            [&](il::Load&) { },
            [&](il::Goto&) { },
        },
        target
    );

    return propagated;
}

bool CopyPropagation::_propagate(il::Operand& operand) {
    auto* variable = std::get_if<il::Variable>(&operand);
    if (variable == nullptr) return false;
    return _propagate(*variable);
}

bool CopyPropagation::_propagate(il::Variable& variable) {
    const auto result = _copy_map.find(variable);
    if (result == _copy_map.end()) return false;

    variable = result->second;
    return true;
}
