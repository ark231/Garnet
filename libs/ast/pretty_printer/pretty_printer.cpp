#include "pretty_printer.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include "compilation_unit.hpp"
#include "concrete_decls.hpp"
#include "concrete_defs.hpp"
#include "concrete_expressions.hpp"
#include "concrete_statements.hpp"
#include "error_nodes.hpp"
#include "format.hpp"
#include "format_utils.hpp"
namespace Garnet::ast {
void PrettyPrinter::visit(const ast::VariableDecl* node) {
    auto name = node->name();
    auto type = node->type();
    auto init = node->init();
    println_with_indent_("VariableDecl {}: {}", name, type);
    {
        AutoIndent ind(indent_);
        println_with_indent_("init:");

        {
            AutoIndent ind(indent_);
            if (init) {
                init.value()->accept(*this);
                fmt::println("");
            } else {
                println_with_indent_("null");
            }
        }
    }
}
void PrettyPrinter::visit(const ast::TypeDecl* node) { println_with_indent_("TypeDecl {}", node->name()); }
void PrettyPrinter::visit(const ast::ErrorNode*) { println_with_indent_("ErrorNode"); }
void PrettyPrinter::visit(const ast::ErrorSentence*) { println_with_indent_("ErrorSentence"); }
void PrettyPrinter::visit(const ast::ErrorExpression*) { println_with_indent_("ErrorExpression"); }
void PrettyPrinter::visit(const ast::ErrorStatement*) { println_with_indent_("ErrorStatement"); }
void PrettyPrinter::visit(const ast::BinaryOperator* node) {
    println_with_indent_("BinaryOperator {}", node->op());
    {
        AutoIndent ind(indent_);
        node->left()->accept(*this);
        force_line_beginning_();
        node->right()->accept(*this);
        force_line_beginning_();
    }
}
void PrettyPrinter::visit(const ast::UnaryOperator* node) {
    println_with_indent_("UnaryOperator {}", node->op());
    {
        AutoIndent ind(indent_);
        node->operand()->accept(*this);
        force_line_beginning_();
    }
}
void PrettyPrinter::visit(const ast::VariableReference* node) {
    print_with_indent_("VariableReference<{} {}>", node->name(), node->valref());
}
void PrettyPrinter::visit(const ast::SignedIntegerLiteral* node) {
    print_with_indent_("SignedIntegerLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::UnsignedIntegerLiteral* node) {
    print_with_indent_("UnsignedIntegerLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::FloatingPointLiteral* node) {
    print_with_indent_("FloatingPointLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::StringLiteral* node) { print_with_indent_("StringLiteral<{}>", node->value()); }
void PrettyPrinter::visit(const ast::FunctionCall* node) {
    println_with_indent_("FunctionCall");
    {
        AutoIndent ind(indent_);
        println_with_indent_("callee:");
        {
            AutoIndent ind(indent_);
            node->callee()->accept(*this);
            force_line_beginning_();
        }
    }
    {
        AutoIndent ind(indent_);
        for (const auto& arg : node->args()) {
            arg->accept(*this);
            force_line_beginning_();
        }
    }
}
void PrettyPrinter::visit(const ast::CompilationUnit* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void PrettyPrinter::visit(const ast::FunctionDef* node) {
    auto info = node->info();
    println_with_indent_("FunctionDef '{}' ({}) -> ({})", info.name(), fmt::join(info.args(), ","),
                         info.result_to_string());
    {
        AutoIndent ind(indent_);
        node->block()->accept(*this);
    }
}
void PrettyPrinter::visit(const ast::VariableDeclStatement* node) {
    println_with_indent_("VariableDeclStatement");
    for (const auto& child : node->children()) {
        AutoIndent ind(indent_);
        child->accept(*this);
    }
}
void PrettyPrinter::visit(const ast::ReturnStatement* node) {
    println_with_indent_("ReturnStatement");
    for (const auto& child : node->children()) {
        AutoIndent ind(indent_);
        child->accept(*this);
        fmt::println("");
    }
}
void PrettyPrinter::visit(const ast::Block* node) {
    println_with_indent_("Block");
    for (const auto& sentence : node->sentences()) {
        AutoIndent ind(indent_);
        sentence->accept(*this);
    }
}
void PrettyPrinter::visit(const ast::LoopStatement* node) {
    println_with_indent_("LoopStatement");
    {
        AutoIndent ind(indent_);
        println_with_indent_("block:");
        {
            AutoIndent ind(indent_);
            node->block()->accept(*this);
        }
    }
}
void PrettyPrinter::visit(const ast::BreakStatement*) { println_with_indent_("BreakStatement"); }
void PrettyPrinter::visit(const ast::IfStatement* node) {
    println_with_indent_("IfStatement");
    for (auto [cond, block] : node->cond_blocks()) {
        AutoIndent ind(indent_);
        println_with_indent_("cond:");
        {
            AutoIndent ind(indent_);
            if (cond) {
                cond->accept(*this);
            } else {
                println_with_indent_("else");
            }
            println_with_indent_("block:");
            {
                AutoIndent ind(indent_);
                block->accept(*this);
            }
        }
    }
}
void PrettyPrinter::force_line_beginning_() {
    if (not at_line_beginning_) {
        fmt::println("");
    }
}
}  // namespace Garnet::ast
