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
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,
        EQUAL,
        BOOL_AND,
        BIT_AND,
        BOOL_OR,
        BIT_OR,
        BIT_XOR,
    };
    BinaryOperator(OperatorType op, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right,
                   location::SourceRegion location = {})
        : Expression(location), op_(op), left_(left), right_(right) {}
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
class UnaryOperator : public Expression {
   public:
    enum class OperatorType {
        BOOL_NOT,
        BIT_NOT,
        PLUS,
        MINUS,
    };
    UnaryOperator(OperatorType op, std::shared_ptr<Expression> operand, location::SourceRegion location = {})
        : Expression(location), op_(op), operand_(operand) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    OperatorType op() const { return op_; }
    const std::shared_ptr<Expression> operand() const { return operand_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    OperatorType op_;
    std::shared_ptr<Expression> operand_;
};
class VariableReference : public Expression {
   public:
    VariableReference(SourceVariableIdentifier name, ValRef valref, location::SourceRegion location = {})
        : Expression(location), name_(name), valref_(valref) {}
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
    SignedIntegerLiteral(int64_t value, location::SourceRegion location = {}) : Expression(location), value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    int64_t value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    int64_t value_;
};
class UnsignedIntegerLiteral : public Expression {
   public:
    UnsignedIntegerLiteral(uint64_t value, location::SourceRegion location = {})
        : Expression(location), value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    uint64_t value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    uint64_t value_;
};
class FloatingPointLiteral : public Expression {
   public:
    FloatingPointLiteral(double value, location::SourceRegion location = {}) : Expression(location), value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    double value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    double value_;
};
class StringLiteral : public Expression {
   public:
    StringLiteral(std::string value, location::SourceRegion location = {}) : Expression(location), value_(value) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string value() const { return value_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    std::string value_;
};
class FunctionCall : public Expression {
   public:
    FunctionCall(std::shared_ptr<Expression> callee, std::vector<std::shared_ptr<Expression>>&& args,
                 location::SourceRegion location = {})
        : Expression(location), callee_(callee), args_(args) {}
    std::vector<std::shared_ptr<Expression>> args() const { return args_; }
    std::shared_ptr<Expression> callee() const { return callee_; }
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    std::shared_ptr<Expression> callee_;
    std::vector<std::shared_ptr<Expression>> args_;
};
}  // namespace Garnet::ast
#endif
