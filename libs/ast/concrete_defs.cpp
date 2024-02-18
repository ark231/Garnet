#include "concrete_defs.hpp"

#include <bits/ranges_algobase.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <algorithm>
#include <iterator>
#include <memory>

#include "../utils/format.hpp"
#include "format_utils.hpp"
namespace WomuYuro::ast {
std::string FunctionDef::to_string(IndentLevel level) const {
    std::string result;
    format_to_with_indent(level, std::back_inserter(result), "FunctionDef '{}' {} -> {}\n", info_.name(), info_.args(),
                          info_.result());
    for (const auto sentence : sentences_) {
        fmt::format_to(std::back_inserter(result), "{}\n", sentence->to_string(level + 1));
    }
    return result;
}
std::vector<std::shared_ptr<Base>> FunctionDef::children() const {
    std::vector<std::shared_ptr<Base>> result;
    std::ranges::transform(sentences_, std::back_inserter(result),
                           [](auto a) { return std::dynamic_pointer_cast<Base>(a); });
    return result;
}
void FunctionDef::add_sentences(std::vector<std::shared_ptr<Sentence>>&& sentences) {
    std::ranges::move(sentences, std::back_inserter(sentences_));
}
}  // namespace WomuYuro::ast
