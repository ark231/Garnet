#include "concrete_defs.hpp"

#include <bits/ranges_algobase.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <algorithm>
#include <iterator>
#include <memory>

namespace Garnet::ast {
std::vector<std::shared_ptr<Base>> FunctionDef::children() const {
    std::vector<std::shared_ptr<Base>> result;
    std::ranges::transform(sentences_, std::back_inserter(result),
                           [](auto a) { return std::dynamic_pointer_cast<Base>(a); });
    return result;
}
void FunctionDef::add_sentences(std::vector<std::shared_ptr<Sentence>>&& sentences) {
    std::ranges::move(sentences, std::back_inserter(sentences_));
}
}  // namespace Garnet::ast
