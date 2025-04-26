#ifndef WOMUYURO_INTERPRETER_INTERPRETER
#define WOMUYURO_INTERPRETER_INTERPRETER
#include <cstdint>
#include <variant>

// #include "types.hpp"
#include "format_support.hpp"
#include "visitor/visitor.hpp"
namespace WomuYuro::interpreter {
class Interpreter : public ast::Visitor {
    using Value = std::variant<std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t,
                               std::int64_t, std::uint64_t, float, double>;

    Value expr_result_;
    IndentLevel indent_ = 0_ind;

   public:
    virtual void visit(const ast::FunctionDecl*) override;
    virtual void visit(const ast::VariableDecl*) override;
    virtual void visit(const ast::TypeDecl*) override;
    virtual void visit(const ast::ErrorNode*) override;
    virtual void visit(const ast::ErrorSentence*) override;
    virtual void visit(const ast::ErrorExpression*) override;
    virtual void visit(const ast::ErrorStatement*) override;
    virtual void visit(const ast::BinaryOperator*) override;
    virtual void visit(const ast::VariableReference*) override;
    virtual void visit(const ast::SignedIntegerLiteral*) override;
    virtual void visit(const ast::UnsignedIntegerLiteral*) override;
    virtual void visit(const ast::FloatingPointLiteral*) override;
    virtual void visit(const ast::FunctionCall*) override;
    virtual void visit(const ast::CompilationUnit*) override;
    virtual void visit(const ast::FunctionDef*) override;
    virtual void visit(const ast::VariableDeclStatement*) override;
    virtual void visit(const ast::ReturnStatement*) override;
};

}  // namespace WomuYuro::interpreter
#endif
