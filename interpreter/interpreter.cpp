#include "interpreter.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <concepts>
#include <functional>
#include <limits>
#include <type_traits>

#include "compilation_unit.hpp"
#include "concrete_decls.hpp"
#include "concrete_defs.hpp"
#include "concrete_expressions.hpp"
#include "concrete_statements.hpp"
#include "error_nodes.hpp"
#include "exceptions.hpp"
#include "format.hpp"
namespace WomuYuro::interpreter {
void Interpreter::visit(const ast::FunctionDecl* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}

template <typename From, typename To>
concept StaticConvertible = requires(From a, To b) {
    { static_cast<To>(a) } -> std::convertible_to<To>;
};

void Interpreter::visit(const ast::VariableDecl* node) {
    Value value;
    if (node->init().has_value()) {
        node->init().value()->accept(*this);
        value = expr_result_;
    }
    auto key = encode_variable_key_(node->name().source_name());
    if (variables_.contains(key)) {
        throw InvalidRedeclarationError(
            std::format("variable {} is already declared in this scope.", node->name().source_name()));
    }
    Variable var;
    var.name = node->name().source_name();
    var.value = types_[encode_type_key_(node->type().source_name())]();
    std::visit(
        [this](auto& target, auto& source) mutable {
            auto assign_only = [](auto& target, auto& source) {
                if constexpr (StaticConvertible<decltype(source), std::remove_cvref_t<decltype(target)>>) {
                    target = static_cast<std::remove_cvref_t<decltype(target)>>(source);
                } else {
                    throw TypeError(fmt::format("cannot convert {} to {}", typeid(source), typeid(target)));
                }
            };
            using namespace std::placeholders;
            if constexpr (std::is_convertible_v<decltype(source), VariableReference> &&
                          std::is_convertible_v<decltype(target), VariableReference>) {
                std::visit(assign_only, variables_[target.key].value, variables_[source.key].value);
            } else if constexpr (std::is_convertible_v<decltype(source), VariableReference>) {
                std::visit([&target, assign_only](auto& source) { assign_only(target, source); },
                           variables_[source.key].value);
            } else if constexpr (std::is_convertible_v<decltype(target), VariableReference>) {
                std::visit(std::bind(assign_only, std::ref(_1), source), variables_[target.key].value);
            } else {
                assign_only(target, source);
            }
        },
        var.value, value);
    variables_[key] = var;
}
void Interpreter::visit(const ast::TypeDecl* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ErrorNode* node) { throw SyntaxError("invalid node"); }
void Interpreter::visit(const ast::ErrorSentence* node) { throw SyntaxError("invalid sentence"); }
void Interpreter::visit(const ast::ErrorExpression* node) { throw SyntaxError("invalid expresssion"); }
void Interpreter::visit(const ast::ErrorStatement* node) { throw SyntaxError("invalid statement"); }
void Interpreter::visit(const ast::BinaryOperator* node) {
    node->left()->accept(*this);

    auto lhs = expr_result_;

    node->right()->accept(*this);

    auto rhs = expr_result_;
    auto deref_and_apply = [this](auto func, auto& lhs, auto& rhs) {
        using LeftType = decltype(lhs);
        using RightType = decltype(rhs);
        using namespace std::placeholders;
        if constexpr (std::is_convertible_v<LeftType, VariableReference> &&
                      std::is_convertible_v<RightType, VariableReference>) {
            std::visit(func, variables_[lhs.key].value, variables_[rhs.key].value);
        } else if constexpr (std::is_convertible_v<LeftType, VariableReference>) {
            std::visit(std::bind(func, _1, rhs), variables_[lhs.key].value);
        } else if constexpr (std::is_convertible_v<RightType, VariableReference>) {
            std::visit([lhs, func](auto rhs) { func(lhs, rhs); }, variables_[rhs.key].value);
        } else {
            func(lhs, rhs);
        }
    };
    switch (node->op()) {
        using enum ast::BinaryOperator::OperatorType;
        case ADD:
            std::visit(
                [this, deref_and_apply](auto lhs, auto rhs) {
                    auto add_only = [this](auto left, auto right) {
                        using LeftType = decltype(left);
                        using RightType = decltype(right);
                        if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                                      std::is_convertible_v<RightType, VariableReference>) {
                            throw TypeError(fmt::format("cannot apply ADD operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)));
                        } else {
                            using Left = std::numeric_limits<LeftType>;
                            using Right = std::numeric_limits<RightType>;
                            if constexpr (Left::is_integer && (not Right::is_integer)) {
                                this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) + right);
                            } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                                this->expr_result_ = static_cast<LeftType>(left + static_cast<LeftType>(right));
                            } else {
                                if constexpr (Left::digits >= Right::digits) {
                                    this->expr_result_ = static_cast<LeftType>(left + static_cast<LeftType>(right));
                                } else {
                                    this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) + right);
                                }
                            }
                        }
                    };
                    deref_and_apply(add_only, lhs, rhs);
                },
                lhs, rhs);
            break;
        // case SUB:
        //     std::visit(
        //         [this](auto lhs, auto rhs) {
        //             using LeftType = decltype(lhs);
        //             using RightType = decltype(lhs);
        //             using Left = std::numeric_limits<LeftType>;
        //             using Right = std::numeric_limits<RightType>;
        //             if constexpr (Left::is_integer && (not Right::is_integer)) {
        //                 this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) - rhs);
        //             } else if constexpr ((not Left::is_integer) && Right::is_integer) {
        //                 this->expr_result_ = static_cast<LeftType>(lhs - static_cast<LeftType>(rhs));
        //             } else {
        //                 if constexpr (Left::digits >= Right::digits) {
        //                     this->expr_result_ = static_cast<LeftType>(lhs - static_cast<LeftType>(rhs));
        //                 } else {
        //                     this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) - rhs);
        //                 }
        //             }
        //         },
        //         lhs, rhs);
        //     break;
        // case MUL:
        //     std::visit(
        //         [this](auto lhs, auto rhs) {
        //             using LeftType = decltype(lhs);
        //             using RightType = decltype(lhs);
        //             using Left = std::numeric_limits<LeftType>;
        //             using Right = std::numeric_limits<RightType>;
        //             if constexpr (Left::is_integer && (not Right::is_integer)) {
        //                 this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) * rhs);
        //             } else if constexpr ((not Left::is_integer) && Right::is_integer) {
        //                 this->expr_result_ = static_cast<LeftType>(lhs * static_cast<LeftType>(rhs));
        //             } else {
        //                 if constexpr (Left::digits >= Right::digits) {
        //                     this->expr_result_ = static_cast<LeftType>(lhs * static_cast<LeftType>(rhs));
        //                 } else {
        //                     this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) * rhs);
        //                 }
        //             }
        //         },
        //         lhs, rhs);
        //     break;
        // case DIV:
        //     std::visit(
        //         [this](auto lhs, auto rhs) {
        //             using LeftType = decltype(lhs);
        //             using RightType = decltype(lhs);
        //             using Left = std::numeric_limits<LeftType>;
        //             using Right = std::numeric_limits<RightType>;
        //             if constexpr (Left::is_integer && Right::is_integer) {
        //                 this->expr_result_ = static_cast<double>(static_cast<double>(lhs) /
        //                 static_cast<double>(rhs));
        //             } else if constexpr (Left::is_integer && (not Right::is_integer)) {
        //                 this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) / rhs);
        //             } else if constexpr ((not Left::is_integer) && Right::is_integer) {
        //                 this->expr_result_ = static_cast<LeftType>(lhs / static_cast<LeftType>(rhs));
        //             } else {
        //                 if constexpr (Left::digits >= Right::digits) {
        //                     this->expr_result_ = static_cast<LeftType>(lhs / static_cast<LeftType>(rhs));
        //                 } else {
        //                     this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) / rhs);
        //                 }
        //             }
        //         },
        //         lhs, rhs);
        //     break;
        // case MOD:
        //     std::visit(
        //         [this](auto lhs, auto rhs) {
        //             using LeftType = decltype(lhs);
        //             using RightType = decltype(lhs);
        //             using Left = std::numeric_limits<LeftType>;
        //             using Right = std::numeric_limits<RightType>;
        //             if constexpr (Left::is_integer && (not Right::is_integer)) {
        //                 this->expr_result_ = static_cast<RightType>(std::fmod(static_cast<RightType>(lhs), rhs));
        //             } else if constexpr ((not Left::is_integer) && Right::is_integer) {
        //                 this->expr_result_ = static_cast<LeftType>(std::fmod(lhs, static_cast<LeftType>(rhs)));
        //             } else if constexpr (Left::is_integer && Right::is_integer) {
        //                 if constexpr (Left::digits >= Right::digits) {
        //                     this->expr_result_ = static_cast<LeftType>(lhs % static_cast<LeftType>(rhs));
        //                 } else {
        //                     this->expr_result_ = static_cast<RightType>(static_cast<RightType>(lhs) % rhs);
        //                 }
        //             } else {
        //                 if constexpr (Left::digits >= Right::digits) {
        //                     this->expr_result_ = static_cast<LeftType>(std::fmod(lhs, static_cast<LeftType>(rhs)));
        //                 } else {
        //                     this->expr_result_ = static_cast<RightType>(std::fmod(static_cast<RightType>(lhs), rhs));
        //                 }
        //             }
        //         },
        //         lhs, rhs);
        //     break;
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
    expr_result_ = VariableReference(encode_variable_key_(node->name().source_name()));
}
void Interpreter::visit(const ast::SignedIntegerLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::UnsignedIntegerLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::FloatingPointLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::FunctionCall* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::CompilationUnit* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::FunctionDef* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::VariableDeclStatement* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ReturnStatement* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
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
void Interpreter::debug_print() const { fmt::println("variables: {}", variables_); }
std::string Interpreter::Variable::to_string() const {
    return fmt::format("Variable(name: {}, value: {})", name, value);
}
std::string Interpreter::VariableReference::to_string() const { return fmt::format("VariableReference(key: {})", key); }
}  // namespace WomuYuro::interpreter
