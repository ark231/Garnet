#ifndef GARNET_LIBS_AST_PRETTY_PRINTER_PRETTY_PRINTER
#define GARNET_LIBS_AST_PRETTY_PRINTER_PRETTY_PRINTER

#include <fmt/base.h>

#include "format_support.hpp"
#include "format_utils.hpp"
#include "visitor/visitor.hpp"
namespace Garnet::ast {
class PrettyPrinter : public ast::Visitor {
    IndentLevel indent_ = 0_ind;
    bool at_line_beginning_ = false;

    template <int tabwidth = 4, typename... Args>
    auto print_with_indent_(fmt::format_string<Args...> format_str, Args&&... args) {
        at_line_beginning_ = false;
        return print_funcs::print_with_indent(indent_, format_str, std::forward<Args>(args)...);
    }
    template <int tabwidth = 4, typename... Args>
    auto println_with_indent_(fmt::format_string<Args...> format_str, Args&&... args) {
        at_line_beginning_ = true;
        return print_funcs::println_with_indent(indent_, format_str, std::forward<Args>(args)...);
    }
    void force_line_beginning_();
    class AutoIndent {
        IndentLevel* indent_;

       public:
        AutoIndent(IndentLevel& indent) : indent_(&indent) { *indent_ = *indent_ + 1; }
        ~AutoIndent() { *indent_ = *indent_ + (-1); }
    };

   public:
    virtual void visit(const ast::VariableDecl*) override;
    virtual void visit(const ast::TypeDecl*) override;
    virtual void visit(const ast::ErrorNode*) override;
    virtual void visit(const ast::ErrorSentence*) override;
    virtual void visit(const ast::ErrorExpression*) override;
    virtual void visit(const ast::ErrorStatement*) override;
    virtual void visit(const ast::BinaryOperator*) override;
    virtual void visit(const ast::UnaryOperator*) override;
    virtual void visit(const ast::VariableReference*) override;
    virtual void visit(const ast::SignedIntegerLiteral*) override;
    virtual void visit(const ast::UnsignedIntegerLiteral*) override;
    virtual void visit(const ast::FloatingPointLiteral*) override;
    virtual void visit(const ast::StringLiteral*) override;
    virtual void visit(const ast::FunctionCall*) override;
    virtual void visit(const ast::CompilationUnit*) override;
    virtual void visit(const ast::FunctionDef*) override;
    virtual void visit(const ast::VariableDeclStatement*) override;
    virtual void visit(const ast::ReturnStatement*) override;
    virtual void visit(const ast::Block*) override;
    virtual void visit(const ast::LoopStatement*) override;
    virtual void visit(const ast::BreakStatement*) override;
    virtual void visit(const ast::IfStatement*) override;
    virtual void visit(const ast::AssertStatement*) override;
    virtual void visit(const ast::BooleanLiteral*) override;
    virtual void visit(const ast::NilLiteral*) override;
    virtual void visit(const ast::StructDef*) override;
    virtual void visit(const ast::MemberAccess*) override;
};

}  // namespace Garnet::ast
#endif
