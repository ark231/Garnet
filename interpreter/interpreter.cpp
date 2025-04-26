#include "interpreter.hpp"

#include <fmt/core.h>

#include "compilation_unit.hpp"
#include "concrete_decls.hpp"
#include "concrete_defs.hpp"
#include "concrete_expressions.hpp"
#include "concrete_statements.hpp"
#include "error_nodes.hpp"
namespace WomuYuro {
void Interpreter::visit(const ast::FunctionDecl* node) {
    fmt::println("visit FunctionDecl");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::VariableDecl* node) {
    fmt::println("visit VariableDecl");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::TypeDecl* node) {
    fmt::println("visit TypeDecl");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ErrorNode* node) {
    fmt::println("visit ErrorNode");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ErrorSentence* node) {
    fmt::println("visit ErrorSentence");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ErrorExpression* node) {
    fmt::println("visit ErrorExpression");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ErrorStatement* node) {
    fmt::println("visit ErrorStatement");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::BinaryOperator* node) {
    fmt::println("visit BinaryOperator");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::VariableReference* node) {
    fmt::println("visit VariableReference");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::SignedIntegerLiteral* node) {
    fmt::println("visit SignedIntegerLiteral");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::UnsignedIntegerLiteral* node) {
    fmt::println("visit UnsignedIntegerLiteral");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::FloatingPointLiteral* node) {
    fmt::println("visit FloatingPointLiteral");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::FunctionCall* node) {
    fmt::println("visit FunctionCall");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::CompilationUnit* node) {
    fmt::println("visit CompilationUnit");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::FunctionDef* node) {
    fmt::println("visit FunctionDef");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::VariableDeclStatement* node) {
    fmt::println("visit VariableDeclStatement");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ReturnStatement* node) {
    fmt::println("visit ReturnStatement");
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
}  // namespace WomuYuro
