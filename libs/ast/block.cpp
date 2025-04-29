#include "block.hpp"

#include <algorithm>
namespace Garnet::ast {
std::vector<std::shared_ptr<Base>> Block::children() const {
    std::vector<std::shared_ptr<Base>> result;
    std::ranges::transform(sentences_, std::back_inserter(result),
                           [](auto a) { return std::dynamic_pointer_cast<Base>(a); });
    return result;
}
void Block::add_sentences(std::vector<std::shared_ptr<Sentence>>&& sentences) {
    std::ranges::move(sentences, std::back_inserter(sentences_));
}

}  // namespace Garnet::ast
