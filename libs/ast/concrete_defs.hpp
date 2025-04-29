#ifndef GARNET_COMPILER_LIBS_AST_CONCRETE_DEFS
#define GARNET_COMPILER_LIBS_AST_CONCRETE_DEFS
#include <memory>
#include <vector>

#include "concrete_infos.hpp"
#include "def_base.hpp"
#include "sentence.hpp"
namespace Garnet::ast {
class FunctionDef : public DefBase {
   public:
    FunctionDef(FunctionInfo info) : info_(info) {}
    FunctionDef(FunctionInfo info, std::vector<std::shared_ptr<Sentence>>&& sentences)
        : info_(info), sentences_(sentences) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    const FunctionInfo& info() const { return info_; };
    void add_sentence(std::shared_ptr<Sentence> sentence) { sentences_.push_back(sentence); };
    void add_sentences(std::vector<std::shared_ptr<Sentence>>&& sentences);
    const std::vector<std::shared_ptr<Sentence>>& sentences() const { return sentences_; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    FunctionInfo info_;
    std::vector<std::shared_ptr<Sentence>> sentences_;
};
}  // namespace Garnet::ast
#endif
