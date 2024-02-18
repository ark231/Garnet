#include "concrete_expressions.hpp"

#include <fmt/core.h>

#include <iterator>
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
std::string FunctionCall::to_string(IndentLevel level) const {
    std::string result;
    format_to_with_indent(level, std::back_inserter(result), "FunctionCall\n");
    for (const auto arg : args_) {
        fmt::format_to(std::back_inserter(result), "{}", arg->to_string(level + 1));
    }
    return result;
}
std::vector<std::shared_ptr<Base>> FunctionCall::children() const { return {}; }
}  // namespace WomuYuro::ast
