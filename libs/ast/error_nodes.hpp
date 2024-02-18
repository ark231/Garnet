#ifndef WOMUYURO_COMPILER_LIBS_AST_ERROR_NODES
#define WOMUYURO_COMPILER_LIBS_AST_ERROR_NODES
#include "base.hpp"
#include "expression.hpp"
#include "format_utils.hpp"
#include "sentence.hpp"
#include "statement.hpp"
namespace WomuYuro::ast {
class ErrorNode : public Base {
    virtual std::string to_string(IndentLevel level) const override { return format_with_indent(level, "ErrorNode"); }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
};
class ErrorSentence : public Sentence {
    virtual std::string to_string(IndentLevel level) const override {
        return format_with_indent(level, "ErrorSentence");
    }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
};
class ErrorExpression : public Expression {
    virtual std::string to_string(IndentLevel level) const override {
        return format_with_indent(level, "ErrorExpression");
    }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
};
class ErrorStatement : public Statement {
    virtual std::string to_string(IndentLevel level) const override {
        return format_with_indent(level, "ErrorStatement");
    }
    virtual std::vector<std::shared_ptr<Base>> children() const override { return {}; };
};
}  // namespace WomuYuro::ast
#endif
