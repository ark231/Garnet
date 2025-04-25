#include "compilation_unit.hpp"

#include <fmt/format.h>

#include <memory>
#include <sstream>

namespace WomuYuro::ast {

std::string CompilationUnit::to_string(IndentLevel level) const {
    std::stringstream result;
    for (const auto &child : children_) {
        result << fmt::format("{}\n", child->to_string(level));  // 二重にindentがかかるから、with_indentは使わない
    }
    return result.str();
}
void CompilationUnit::add_child(std::shared_ptr<Base> child) { children_.push_back(child); }
std::vector<std::shared_ptr<Base>> CompilationUnit::children() const { return children_; }
}  // namespace WomuYuro::ast
