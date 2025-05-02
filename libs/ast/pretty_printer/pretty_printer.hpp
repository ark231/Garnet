#ifndef GARNET_LIBS_AST_PRETTY_PRINTER_PRETTY_PRINTER
#define GARNET_LIBS_AST_PRETTY_PRINTER_PRETTY_PRINTER

#include "format_support.hpp"
#include "visitor/visitor.hpp"
namespace Garnet::ast {
class PrettyPrinter : public ast::Visitor {
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
    virtual void visit(const ast::Block*) override;
    virtual void visit(const ast::LoopStatement*) override;
    // virtual void visit(const ast::BreakStatement*) override;
    virtual void visit(const ast::IfStatement*) override;
};

}  // namespace Garnet::ast
#endif
