#ifndef GARNET_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#define GARNET_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#include <iterator>
#include <memory>
#include <vector>

#include "block.hpp"
#include "concrete_decls.hpp"
#include "expression.hpp"
#include "statement.hpp"

namespace Garnet::ast {
class VariableDeclStatement : public Statement {
   public:
    VariableDeclStatement(std::shared_ptr<VariableDecl> decl) : decl_(decl) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   protected:
    std::shared_ptr<VariableDecl> decl_;
};
class ReturnStatement : public Statement {
   public:
    ReturnStatement(std::shared_ptr<Expression> retval) : retval_(retval) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   protected:
    std::shared_ptr<Expression> retval_;
};
class LoopStatement : public Statement {
   public:
    LoopStatement(std::shared_ptr<Block> block) : block_(block) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
    std::shared_ptr<Block> block() const { return block_; }

   protected:
    std::shared_ptr<Block> block_;
};
class IfStatement : public Statement {
   public:
    struct CondBlock {
        std::shared_ptr<Expression> cond;
        std::shared_ptr<Block> block;
    };
    IfStatement(std::vector<CondBlock> cond_blocks) : cond_blocks_{cond_blocks} {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
    std::vector<CondBlock> cond_blocks() const { return cond_blocks_; };
    void add_cond_block(CondBlock cond_block) { cond_blocks_.push_back(cond_block); }
    void add_cond_blocks(std::vector<CondBlock>&& cond_blocks) {
        std::ranges::copy(cond_blocks, std::back_inserter(cond_blocks_));
    }

   protected:
    std::vector<CondBlock> cond_blocks_;
};
}  // namespace Garnet::ast
#endif
