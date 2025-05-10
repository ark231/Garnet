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
    VariableDeclStatement(std::shared_ptr<VariableDecl> decl, location::SourceRegion location = {})
        : Statement(location), decl_(decl) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   protected:
    std::shared_ptr<VariableDecl> decl_;
};
class ReturnStatement : public Statement {
   public:
    ReturnStatement(std::shared_ptr<Expression> retval, location::SourceRegion location = {})
        : Statement(location), retval_(retval) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
    std::shared_ptr<Expression> retval() const { return retval_; }

   protected:
    std::shared_ptr<Expression> retval_;
};
class LoopStatement : public Statement {
   public:
    LoopStatement(std::shared_ptr<Block> block, location::SourceRegion location = {})
        : Statement(location), block_(block) {}
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
    IfStatement(std::vector<CondBlock> cond_blocks, location::SourceRegion location = {})
        : Statement(location), cond_blocks_{cond_blocks} {}
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
class BreakStatement : public Statement {
   public:
    BreakStatement(location::SourceRegion location = {}) : Statement(location) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
};
class AssertStatement : public Statement {
   public:
    AssertStatement(std::shared_ptr<Expression> cond, std::optional<std::shared_ptr<Expression>> msg,
                    location::SourceRegion location = {})
        : Statement(location), cond_(cond), msg_(msg) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

    std::shared_ptr<Expression> cond() const { return cond_; }
    std::optional<std::shared_ptr<Expression>> msg() const { return msg_; }

   private:
    std::shared_ptr<Expression> cond_;
    std::optional<std::shared_ptr<Expression>> msg_;
};
}  // namespace Garnet::ast
#endif
