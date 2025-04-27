#include "compilation_unit.hpp"

#include <fmt/format.h>

#include <memory>

namespace WomuYuro::ast {

void CompilationUnit::add_child(std::shared_ptr<Base> child) { children_.push_back(child); }
std::vector<std::shared_ptr<Base>> CompilationUnit::children() const { return children_; }
}  // namespace WomuYuro::ast
