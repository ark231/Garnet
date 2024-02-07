#include "concrete_expressions.hpp"

#include <fmt/core.h>

#include <magic_enum.hpp>
#include <magic_enum_format.hpp>

#include "../utils/format.hpp"
namespace WomuYuro::ast {
std::string BinaryOperator::to_string() const { return fmt::format("BinaryOperator<{} {} {}>", *left_, op_, *right_); }
std::vector<std::shared_ptr<Base>> BinaryOperator::children() const {
    // std::vector<std::shared_ptr<Base>> result;
    // result.push_back(left_);
    // result.push_back(right_);
    // return result;
    return {left_, right_};
}
std::string VariableReference::to_string() const { return fmt::format("VariableReference<{} {}>", name_, valref_); }
std::vector<std::shared_ptr<Base>> VariableReference::children() const { return {}; }
std::string SignedIntegerLiteral::to_string() const { return fmt::format("SignedIntegerLiteral<{}>", value_); }
std::vector<std::shared_ptr<Base>> SignedIntegerLiteral::children() const { return {}; }
std::string UnsignedIntegerLiteral::to_string() const { return fmt::format("UnsignedIntegerLiteral<{}>", value_); }
std::vector<std::shared_ptr<Base>> UnsignedIntegerLiteral::children() const { return {}; }
std::string FloatingPointLiteral::to_string() const { return fmt::format("FloatingPointLiteral<{}>", value_); }
std::vector<std::shared_ptr<Base>> FloatingPointLiteral::children() const { return {}; }
}  // namespace WomuYuro::ast
