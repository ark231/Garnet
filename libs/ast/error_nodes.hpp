#ifndef WOMUYURO_COMPILER_LIBS_AST_ERROR_NODES
#define WOMUYURO_COMPILER_LIBS_AST_ERROR_NODES
#include "base.hpp"
#include "expression.hpp"
#include "format_utils.hpp"
#include "sentence.hpp"
#include "statement.hpp"
namespace WomuYuro::ast {
class ErrorNode : public Base {
   public:
    virtual std::string to_string(IndentLevel level) const override { return format_with_indent(level, "ErrorNode"); }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
class ErrorSentence : public Sentence {
   public:
    virtual std::string to_string(IndentLevel level) const override {
        return format_with_indent(level, "ErrorSentence");
    }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
class ErrorExpression : public Expression {
   public:
    virtual std::string to_string(IndentLevel level) const override {
        return format_with_indent(level, "ErrorExpression");
    }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
class ErrorStatement : public Statement {
   public:
    virtual std::string to_string(IndentLevel level) const override {
        return format_with_indent(level, "ErrorStatement");
    }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); };
};
}  // namespace WomuYuro::ast
#endif
