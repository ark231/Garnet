#ifndef GARNET_LIBS_AST_CONCRETE_EXPRESSIONS
#define GARNET_LIBS_AST_CONCRETE_EXPRESSIONS

#include <magic_enum.hpp>
#include <magic_enum_format.hpp>
#include <memory>
#include <vector>

#include "concrete_source_identifiers.hpp"
#include "enums.hpp"
#include "expression.hpp"
namespace Garnet::ast {
class BinaryOperator : public Expression {
   public:
    enum class OperatorType {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        ASSIGN,
    };
    BinaryOperator(OperatorType op, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : op_(op), left_(left), right_(right) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    OperatorType op() const { return op_; }
    const std::shared_ptr<Expression> left() const { return left_; }
    const std::shared_ptr<Expression> right() const { return right_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    OperatorType op_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};
class VariableReference : public Expression {
   public:
    VariableReference(SourceVariableIdentifier name, ValRef valref) : name_(name), valref_(valref) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

    SourceVariableIdentifier name() const { return name_; }
    ValRef valref() const { return valref_; }

   private:
    SourceVariableIdentifier name_;
    ValRef valref_;
};
class SignedIntegerLiteral : public Expression {
   public:
    SignedIntegerLiteral(int64_t value) : value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    int64_t value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    int64_t value_;
};
class UnsignedIntegerLiteral : public Expression {
   public:
    UnsignedIntegerLiteral(uint64_t value) : value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    uint64_t value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    uint64_t value_;
};
class FloatingPointLiteral : public Expression {
   public:
    FloatingPointLiteral(double value) : value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    double value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    double value_;
};
class FunctionCall : public Expression {
   public:
    FunctionCall(SourceFunctionIdentifier name, std::vector<std::shared_ptr<Expression>>&& args)
        : name_(name), args_(args) {}
    std::vector<std::shared_ptr<Expression>> args() const { return args_; }
    SourceFunctionIdentifier name() const { return name_; }
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    SourceFunctionIdentifier name_;
    std::vector<std::shared_ptr<Expression>> args_;
};
}  // namespace Garnet::ast
#endif
