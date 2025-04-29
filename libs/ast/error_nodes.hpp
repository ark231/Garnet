#ifndef GARNET_COMPILER_LIBS_AST_ERROR_NODES
#define GARNET_COMPILER_LIBS_AST_ERROR_NODES
#include "base.hpp"
#include "expression.hpp"
#include "sentence.hpp"
#include "statement.hpp"
namespace Garnet::ast {
class ErrorNode : public Base {
   public:
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
class ErrorSentence : public Sentence {
   public:
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
class ErrorExpression : public Expression {
   public:
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
class ErrorStatement : public Statement {
   public:
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
}  // namespace Garnet::ast
#endif
