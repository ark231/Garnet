#ifndef GARNET_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#define GARNET_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#include <memory>
#include <vector>

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
}  // namespace Garnet::ast
#endif
