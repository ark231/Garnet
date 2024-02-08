#include "compilation_unit.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>

#include "../utils/format_utils.hpp"

namespace WomuYuro::ast {

std::string CompilationUnit::to_string(IndentLevel level) const {
    std::stringstream result;
    for (const auto decl : decls_) {
        result << fmt::format("{}\n", decl->to_string(level));  // 二重にindentがかかるから、with_indentは使わない
    }
    return result.str();
}
std::vector<std::shared_ptr<Base>> CompilationUnit::children() const {
    std::vector<std::shared_ptr<Base>> result;
    std::ranges::transform(decls_, std::back_inserter(result),
                           [](auto& child) { return std::dynamic_pointer_cast<Base>(child); });
    return result;
}
void CompilationUnit::add_decl(std::shared_ptr<DeclBase> decl) { decls_.push_back(decl); }
const std::vector<std::shared_ptr<DeclBase>>& CompilationUnit::decls() const { return decls_; }
}  // namespace WomuYuro::ast
