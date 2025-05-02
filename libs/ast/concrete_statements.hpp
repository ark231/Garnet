#ifndef GARNET_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#define GARNET_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
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
}  // namespace Garnet::ast
#endif
