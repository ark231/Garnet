#ifndef GARNET_LIBS_AST_BLOCK
#define GARNET_LIBS_AST_BLOCK
#include <memory>
#include <vector>

#include "base.hpp"
#include "format_support.hpp"
#include "sentence.hpp"
namespace Garnet::ast {
class Block : public Base {
   public:
    Block() = default;
    Block(std::vector<std::shared_ptr<Sentence>> sentences) : sentences_(sentences) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

    void add_sentence(std::shared_ptr<Sentence> sentence) { sentences_.push_back(sentence); };
    void add_sentences(std::vector<std::shared_ptr<Sentence>>&& sentences);
    const std::vector<std::shared_ptr<Sentence>>& sentences() const { return sentences_; };

   private:
    std::vector<std::shared_ptr<Sentence>> sentences_;
};
}  // namespace Garnet::ast
#endif
