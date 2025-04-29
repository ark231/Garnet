#include "concrete_statements.hpp"

#include <fmt/core.h>

namespace Garnet::ast {
std::vector<std::shared_ptr<Base>> VariableDeclStatement::children() const { return {decl_}; }
std::vector<std::shared_ptr<Base>> ReturnStatement::children() const { return {retval_}; }
}  // namespace Garnet::ast
