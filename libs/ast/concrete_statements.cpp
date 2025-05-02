#include "concrete_statements.hpp"

#include <fmt/core.h>

namespace Garnet::ast {
std::vector<std::shared_ptr<Base>> VariableDeclStatement::children() const { return {decl_}; }
std::vector<std::shared_ptr<Base>> ReturnStatement::children() const { return {retval_}; }
std::vector<std::shared_ptr<Base>> LoopStatement::children() const { return {block_}; }
std::vector<std::shared_ptr<Base>> BreakStatement::children() const { return {}; }
std::vector<std::shared_ptr<Base>> IfStatement::children() const { return {}; }
}  // namespace Garnet::ast
