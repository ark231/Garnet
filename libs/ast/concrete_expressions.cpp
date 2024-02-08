#include "concrete_expressions.hpp"

#include <fmt/core.h>

#include <magic_enum.hpp>
#include <magic_enum_format.hpp>

#include "../utils/format.hpp"
#include "../utils/format_utils.hpp"
namespace WomuYuro::ast {
std::string BinaryOperator::to_string(IndentLevel level) const {
    return format_with_indent(level, "BinaryOperator {}\n{}\n{}", op_, left_->to_string(level + 1),
                              right_->to_string(level + 1));
}
std::vector<std::shared_ptr<Base>> BinaryOperator::children() const { return {left_, right_}; }
std::string VariableReference::to_string(IndentLevel level) const {
    return format_with_indent(level, "VariableReference<{} {}>", name_, valref_);
}
std::vector<std::shared_ptr<Base>> VariableReference::children() const { return {}; }
std::string SignedIntegerLiteral::to_string(IndentLevel level) const {
    return format_with_indent(level, "SignedIntegerLiteral<{}>", value_);
}
std::vector<std::shared_ptr<Base>> SignedIntegerLiteral::children() const { return {}; }
std::string UnsignedIntegerLiteral::to_string(IndentLevel level) const {
    return format_with_indent(level, "UnsignedIntegerLiteral<{}>", value_);
}
std::vector<std::shared_ptr<Base>> UnsignedIntegerLiteral::children() const { return {}; }
std::string FloatingPointLiteral::to_string(IndentLevel level) const {
    return format_with_indent(level, "FloatingPointLiteral<{}>", value_);
}
std::vector<std::shared_ptr<Base>> FloatingPointLiteral::children() const { return {}; }
}  // namespace WomuYuro::ast
