#include "interpreter.hpp"

#include <fmt/core.h>
#include <fmt/std.h>

#include <limits>
#include <type_traits>

#include "compilation_unit.hpp"
#include "concrete_decls.hpp"
#include "concrete_defs.hpp"
#include "concrete_expressions.hpp"
#include "concrete_statements.hpp"
#include "error_nodes.hpp"
#include "exceptions.hpp"
#include "format_utils.hpp"
namespace WomuYuro::interpreter {
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
    auto key = encode_variable_key_(node->name().source_name());
    if (variables_.contains(key)) {
        throw InvalidRedeclarationError(
            std::format("variable {} is already declared in this scope.", node->name().source_name()));
    }
    Variable var;
    var.name = node->name().source_name();
    var.value = types_[encode_type_key_(node->type().source_name())]();
    std::visit([](auto& target, auto& source) { target = static_cast<std::remove_cvref_t<decltype(target)>>(source); },
               var.value, value);
    variables_[key] = var;
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
    throw SyntaxError("invalid node");
}
void Interpreter::visit(const ast::ErrorSentence* node) {
    println_with_indent(indent_, "visit ErrorSentence");
    throw SyntaxError("invalid sentence");
}
void Interpreter::visit(const ast::ErrorExpression* node) {
    println_with_indent(indent_, "visit ErrorExpression");
    throw SyntaxError("invalid expresssion");
}
void Interpreter::visit(const ast::ErrorStatement* node) {
    println_with_indent(indent_, "visit ErrorStatement");
    throw SyntaxError("invalid statement");
}
void Interpreter::visit(const ast::BinaryOperator* node) {
    println_with_indent(indent_, "visit BinaryOperator");
    indent_ = indent_ + 1;
    node->left()->accept(*this);
    indent_ = indent_ + (-1);
    auto lhs = expr_result_;
    indent_ = indent_ + 1;
    node->right()->accept(*this);
    indent_ = indent_ + (-1);
    auto rhs = expr_result_;
    switch (node->op()) {
        using enum ast::BinaryOperator::OperatorType;
        case ADD:
            std::visit(
                [this](auto lhs, auto rhs) {
                    using LeftType = decltype(lhs);
                    using RightType = decltype(lhs);
                    using Left = std::numeric_limits<LeftType>;
                    using Right = std::numeric_limits<RightType>;
                    if constexpr (Left::is_integer && (not Right::is_integer)) {
                        this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) + rhs);
                    } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                        this->expr_result_ = static_cast<LeftType>(lhs + static_cast<LeftType>(rhs));
                    } else {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(lhs + static_cast<LeftType>(rhs));
                        } else {
                            this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) + rhs);
                        }
                    }
                },
                lhs, rhs);
            break;
        case SUB:
            std::visit(
                [this](auto lhs, auto rhs) {
                    using LeftType = decltype(lhs);
                    using RightType = decltype(lhs);
                    using Left = std::numeric_limits<LeftType>;
                    using Right = std::numeric_limits<RightType>;
                    if constexpr (Left::is_integer && (not Right::is_integer)) {
                        this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) - rhs);
                    } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                        this->expr_result_ = static_cast<LeftType>(lhs - static_cast<LeftType>(rhs));
                    } else {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(lhs - static_cast<LeftType>(rhs));
                        } else {
                            this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) - rhs);
                        }
                    }
                },
                lhs, rhs);
            break;
        case MUL:
            std::visit(
                [this](auto lhs, auto rhs) {
                    using LeftType = decltype(lhs);
                    using RightType = decltype(lhs);
                    using Left = std::numeric_limits<LeftType>;
                    using Right = std::numeric_limits<RightType>;
                    if constexpr (Left::is_integer && (not Right::is_integer)) {
                        this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) * rhs);
                    } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                        this->expr_result_ = static_cast<LeftType>(lhs * static_cast<LeftType>(rhs));
                    } else {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(lhs * static_cast<LeftType>(rhs));
                        } else {
                            this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) * rhs);
                        }
                    }
                },
                lhs, rhs);
            break;
        case DIV:
            std::visit(
                [this](auto lhs, auto rhs) {
                    using LeftType = decltype(lhs);
                    using RightType = decltype(lhs);
                    using Left = std::numeric_limits<LeftType>;
                    using Right = std::numeric_limits<RightType>;
                    if constexpr (Left::is_integer && Right::is_integer) {
                        this->expr_result_ = static_cast<double>(static_cast<double>(lhs) / static_cast<double>(rhs));
                    } else if constexpr (Left::is_integer && (not Right::is_integer)) {
                        this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) / rhs);
                    } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                        this->expr_result_ = static_cast<LeftType>(lhs / static_cast<LeftType>(rhs));
                    } else {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(lhs / static_cast<LeftType>(rhs));
                        } else {
                            this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) / rhs);
                        }
                    }
                },
                lhs, rhs);
            break;
        case MOD:
            std::visit(
                [this](auto lhs, auto rhs) {
                    using LeftType = decltype(lhs);
                    using RightType = decltype(lhs);
                    using Left = std::numeric_limits<LeftType>;
                    using Right = std::numeric_limits<RightType>;
                    if constexpr (Left::is_integer && (not Right::is_integer)) {
                        this->expr_result_ = static_cast<RightType>(std::fmod(static_cast<RightType>(lhs), rhs));
                    } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                        this->expr_result_ = static_cast<LeftType>(std::fmod(lhs, static_cast<LeftType>(rhs)));
                    } else if constexpr (Left::is_integer && Right::is_integer) {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(lhs % static_cast<LeftType>(rhs));
                        } else {
                            this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) % rhs);
                        }
                    } else {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(std::fmod(lhs, static_cast<LeftType>(rhs)));
                        } else {
                            this->expr_result_ = static_cast<RightType>(std::fmod(static_cast<RightType>(lhs), rhs));
                        }
                    }
                },
                lhs, rhs);
            break;
        case ASSIGN:
            throw UnImplementedError("ASSIGN operator is not implemented yet");
            // std::visit(
            //     [this](auto lhs, auto rhs) {
            //         using LeftType = decltype(lhs);
            //         using RightType = decltype(lhs);
            //         using Left = std::numeric_limits<LeftType>;
            //         using Right = std::numeric_limits<RightType>;
            //         if constexpr (Left::is_integer && (not Right::is_integer)) {
            //             this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) - rhs);
            //         } else if constexpr ((not Left::is_integer) && Right::is_integer) {
            //             this->expr_result_ = static_cast<LeftType>(lhs - static_cast<LeftType>(rhs));
            //         } else {
            //             if constexpr (Left::digits >= Right::digits) {
            //                 this->expr_result_ = static_cast<LeftType>(lhs - static_cast<LeftType>(rhs));
            //             } else {
            //                 this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) - rhs);
            //             }
            //         }
            //     },
            //     lhs, rhs);
            break;
    }
}
void Interpreter::visit(const ast::VariableReference* node) {
    println_with_indent(indent_, "visit VariableReference");
    expr_result_ = variables_[encode_variable_key_(node->name().source_name())].value;
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
Interpreter::VariableKey Interpreter::encode_variable_key_(std::string name) const { return name; }
Interpreter::TypeKey Interpreter::encode_type_key_(std::string name) const { return name; }
Interpreter::Interpreter() {
    types_[encode_type_key_("u8")] = [] { return Value(static_cast<std::uint8_t>(0)); };
    types_[encode_type_key_("i8")] = [] { return Value(static_cast<std::int8_t>(0)); };
    types_[encode_type_key_("u16")] = [] { return Value(static_cast<std::uint16_t>(0)); };
    types_[encode_type_key_("i16")] = [] { return Value(static_cast<std::int16_t>(0)); };
    types_[encode_type_key_("u32")] = [] { return Value(static_cast<std::uint32_t>(0)); };
    types_[encode_type_key_("i32")] = [] { return Value(static_cast<std::int32_t>(0)); };
    types_[encode_type_key_("u64")] = [] { return Value(static_cast<std::uint64_t>(0)); };
    types_[encode_type_key_("i64")] = [] { return Value(static_cast<std::int64_t>(0)); };
    types_[encode_type_key_("f32")] = [] { return Value(static_cast<float>(0)); };
    types_[encode_type_key_("f64")] = [] { return Value(static_cast<double>(0)); };
}
}  // namespace WomuYuro::interpreter
