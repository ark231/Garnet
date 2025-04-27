#include "concrete_expressions.hpp"

#include <fmt/core.h>

namespace WomuYuro::ast {
std::vector<std::shared_ptr<Base>> BinaryOperator::children() const { return {left_, right_}; }
std::vector<std::shared_ptr<Base>> VariableReference::children() const { return {}; }
std::vector<std::shared_ptr<Base>> SignedIntegerLiteral::children() const { return {}; }
std::vector<std::shared_ptr<Base>> UnsignedIntegerLiteral::children() const { return {}; }
std::vector<std::shared_ptr<Base>> FloatingPointLiteral::children() const { return {}; }
std::vector<std::shared_ptr<Base>> FunctionCall::children() const { return {}; }
}  // namespace WomuYuro::ast
