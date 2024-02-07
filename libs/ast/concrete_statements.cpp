#include "concrete_statements.hpp"

#include <fmt/core.h>

#include "../utils/format.hpp"
namespace WomuYuro::ast {
std::string VariableDeclStatement::to_string() const { return fmt::format("VariableDeclStatement<{}>", *decl_); };
std::vector<std::shared_ptr<Base>> VariableDeclStatement::children() const { return {decl_}; }
}  // namespace WomuYuro::ast
