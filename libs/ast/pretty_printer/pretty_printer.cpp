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
void PrettyPrinter::visit(const ast::FunctionDecl* node) {
    std::string result;
    auto info = node->info();
    println_with_indent(indent_, "FunctionDecl '{}' ({}) -> ({})", info.name(), fmt::join(info.args(), ","),
                        info.result().has_value() ? info.result()->to_string() : "void");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void PrettyPrinter::visit(const ast::VariableDecl* node) {
    auto name = node->name();
    auto type = node->type();
    auto init = node->init();
    println_with_indent(indent_, "VariableDecl {}: {}", name, type);
    indent_ = indent_ + 1;
    println_with_indent(indent_, "init:");
    indent_ = indent_ + 1;
    if (init) {
        init.value()->accept(*this);
        fmt::println("");
    } else {
        println_with_indent(indent_, "null");
    }
    indent_ = indent_ + (-1);
    indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::TypeDecl* node) { println_with_indent(indent_, "TypeDecl {}", node->name()); }
void PrettyPrinter::visit(const ast::ErrorNode*) { println_with_indent(indent_, "ErrorNode"); }
void PrettyPrinter::visit(const ast::ErrorSentence*) { println_with_indent(indent_, "ErrorSentence"); }
void PrettyPrinter::visit(const ast::ErrorExpression*) { println_with_indent(indent_, "ErrorExpression"); }
void PrettyPrinter::visit(const ast::ErrorStatement*) { println_with_indent(indent_, "ErrorStatement"); }
void PrettyPrinter::visit(const ast::BinaryOperator* node) {
    println_with_indent(indent_, "BinaryOperator {}", node->op());
    indent_ = indent_ + 1;
    node->left()->accept(*this);
    fmt::println("");
    node->right()->accept(*this);
    fmt::println("");
    indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::VariableReference* node) {
    print_with_indent(indent_, "VariableReference<{} {}>", node->name(), node->valref());
}
void PrettyPrinter::visit(const ast::SignedIntegerLiteral* node) {
    print_with_indent(indent_, "SignedIntegerLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::UnsignedIntegerLiteral* node) {
    print_with_indent(indent_, "UnsignedIntegerLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::FloatingPointLiteral* node) {
    print_with_indent(indent_, "FloatingPointLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::StringLiteral* node) {
    print_with_indent(indent_, "StringLiteral<{}>", node->value());
}
void PrettyPrinter::visit(const ast::FunctionCall* node) {
    println_with_indent(indent_, "FunctionCall");
    indent_ = indent_ + 1;
    for (const auto& arg : node->args()) {
        arg->accept(*this);
        fmt::println("");
    }
    indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::CompilationUnit* node) {
    // indent_ = indent_ + 1;
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
    // indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::FunctionDef* node) {
    auto info = node->info();
    println_with_indent(indent_, "FunctionDef '{}' ({}) -> ({})", info.name(), fmt::join(info.args(), ","),
                        info.result_to_string());
    indent_ = indent_ + 1;
    node->block()->accept(*this);
    indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::VariableDeclStatement* node) {
    println_with_indent(indent_, "VariableDeclStatement");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void PrettyPrinter::visit(const ast::ReturnStatement* node) {
    println_with_indent(indent_, "ReturnStatement");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        fmt::println("");
        indent_ = indent_ + (-1);
    }
}
void PrettyPrinter::visit(const ast::Block* node) {
    println_with_indent(indent_, "Block");
    indent_ = indent_ + 1;
    for (const auto& sentence : node->sentences()) {
        sentence->accept(*this);
    }
    indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::LoopStatement* node) {
    println_with_indent(indent_, "LoopStatement");
    indent_ = indent_ + 1;
    println_with_indent(indent_, "block:");
    indent_ = indent_ + 1;
    node->block()->accept(*this);
    indent_ = indent_ + (-1);
    indent_ = indent_ + (-1);
}
void PrettyPrinter::visit(const ast::BreakStatement*) { println_with_indent(indent_, "BreakStatement"); }
void PrettyPrinter::visit(const ast::IfStatement* node) {
    println_with_indent(indent_, "IfStatement");
    indent_ = indent_ + 1;
    for (auto [cond, block] : node->cond_blocks()) {
        println_with_indent(indent_, "cond:");
        indent_ = indent_ + 1;
        if (cond) {
            cond->accept(*this);
        } else {
            println_with_indent(indent_, "else");
        }
        println_with_indent(indent_, "block:");
        indent_ = indent_ + 1;
        block->accept(*this);
        indent_ = indent_ + (-1);
        indent_ = indent_ + (-1);
    }
    indent_ = indent_ + (-1);
}
}  // namespace Garnet::ast
