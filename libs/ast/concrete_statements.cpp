#include "concrete_statements.hpp"

#include <fmt/core.h>

#include "../utils/format_utils.hpp"
namespace WomuYuro::ast {
std::string VariableDeclStatement::to_string(IndentLevel level) const {
    return format_with_indent(level, "VariableDeclStatement\n{}", decl_->to_string(level + 1));
};
std::vector<std::shared_ptr<Base>> VariableDeclStatement::children() const { return {decl_}; }
std::string ReturnStatement::to_string(IndentLevel level) const {
    return format_with_indent(level, "ReturnStatement\n{}", retval_->to_string(level + 1));
};
std::vector<std::shared_ptr<Base>> ReturnStatement::children() const { return {retval_}; }
}  // namespace WomuYuro::ast
