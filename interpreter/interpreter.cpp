#include "interpreter.hpp"

#include <fmt/core.h>
#include <fmt/std.h>

#include "compilation_unit.hpp"
#include "concrete_decls.hpp"
#include "concrete_defs.hpp"
#include "concrete_expressions.hpp"
#include "concrete_statements.hpp"
#include "error_nodes.hpp"
#include "format_utils.hpp"
namespace WomuYuro {
void Interpreter::visit(const ast::FunctionDecl* node) {
    println_with_indent(indent_, "visit FunctionDecl");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::VariableDecl* node) {
    println_with_indent(indent_, "visit VariableDecl");
    Value value;
    if (node->init().has_value()) {
        node->init().value()->accept(*this);
        value = expr_result_;
    }
    println_with_indent(indent_, "init: {}", value);
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::TypeDecl* node) {
    println_with_indent(indent_, "visit TypeDecl");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::ErrorNode* node) {
    println_with_indent(indent_, "visit ErrorNode");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::ErrorSentence* node) {
    println_with_indent(indent_, "visit ErrorSentence");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::ErrorExpression* node) {
    println_with_indent(indent_, "visit ErrorExpression");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::ErrorStatement* node) {
    println_with_indent(indent_, "visit ErrorStatement");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::BinaryOperator* node) {
    println_with_indent(indent_, "visit BinaryOperator");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::VariableReference* node) {
    println_with_indent(indent_, "visit VariableReference");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::SignedIntegerLiteral* node) {
    println_with_indent(indent_, "visit SignedIntegerLiteral");
    expr_result_ = node->value();
}
void Interpreter::visit(const ast::UnsignedIntegerLiteral* node) {
    println_with_indent(indent_, "visit UnsignedIntegerLiteral");
    expr_result_ = node->value();
}
void Interpreter::visit(const ast::FloatingPointLiteral* node) {
    println_with_indent(indent_, "visit FloatingPointLiteral");
    expr_result_ = node->value();
}
void Interpreter::visit(const ast::FunctionCall* node) {
    println_with_indent(indent_, "visit FunctionCall");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::CompilationUnit* node) {
    println_with_indent(indent_, "visit CompilationUnit");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::FunctionDef* node) {
    println_with_indent(indent_, "visit FunctionDef");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::VariableDeclStatement* node) {
    println_with_indent(indent_, "visit VariableDeclStatement");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
void Interpreter::visit(const ast::ReturnStatement* node) {
    println_with_indent(indent_, "visit ReturnStatement");
    for (const auto& child : node->children()) {
        indent_ = indent_ + 1;
        child->accept(*this);
        indent_ = indent_ + (-1);
    }
}
}  // namespace WomuYuro
