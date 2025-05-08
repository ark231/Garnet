#include "interpreter.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <concepts>
#include <functional>
#include <limits>
#include <memory>
#include <type_traits>

#include "compilation_unit.hpp"
#include "concrete_decls.hpp"
#include "concrete_defs.hpp"
#include "concrete_expressions.hpp"
#include "concrete_source_identifiers.hpp"
#include "concrete_statements.hpp"
#include "error_nodes.hpp"
#include "exceptions.hpp"
#include "flyweight.hpp"
#include "format.hpp"  // NOLINT(clang-diagnostic-unused-header)
#include "instance_pool.hpp"
#include "location.hpp"
namespace Garnet::interpreter {

template <typename From, typename To>
concept StaticConvertible = requires(From a, To b) {
    { static_cast<To>(a) } -> std::convertible_to<To>;
};

void Interpreter::visit(const ast::VariableDecl* node) {
    std::optional<Value> init;
    if (node->init().has_value()) {
        node->init().value()->accept(*this);
        Value value = expr_result_;
        init = value;
    } else if (node->userdata.has_value()) {
        auto value = std::any_cast<Value>(node->userdata);
        init = value;
    }
    declare_variable_(node->name(), node->type(), init);
}
void Interpreter::declare_variable_(ast::SourceVariableIdentifier name, ast::SourceTypeIdentifier type,
                                    std::optional<Value> value, location::SourceRegion location) {
    if (current_scope_->keymap->contains(name.source_id())) {
        throw InvalidRedeclarationError(
            std::format("variable {} is already declared in this scope.", name.source_name()), location);
    }
    Variable var;
    auto key = key_generator_();
    var.name_id = name.source_id();
    var.value = types_.at(type.source_id())();
    if (value.has_value()) {
        std::visit(
            [this, &location](auto& target, auto& source) mutable {
                auto assign_only = [&location](auto& target, auto& source) {
                    if constexpr (StaticConvertible<decltype(source), std::remove_cvref_t<decltype(target)>>) {
                        target = static_cast<std::remove_cvref_t<decltype(target)>>(source);
                    } else {
                        throw TypeError(fmt::format("cannot convert {} to {}", typeid(source), typeid(target)),
                                        location);
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
            var.value, *value);
    }
    variables_[key] = var;
    (*(current_scope_->keymap))[var.name_id] = key;
}
void Interpreter::visit(const ast::TypeDecl* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ErrorNode* node) { throw SyntaxError("invalid node", node->location()); }
void Interpreter::visit(const ast::ErrorSentence* node) { throw SyntaxError("invalid sentence", node->location()); }
void Interpreter::visit(const ast::ErrorExpression* node) {
    throw SyntaxError("invalid expresssion", node->location());
}
void Interpreter::visit(const ast::ErrorStatement* node) { throw SyntaxError("invalid statement", node->location()); }

namespace {
template <typename T, typename U>
concept LessComparable = requires(T a, U b) {
    { a < b } -> std::convertible_to<bool>;
};
template <typename T, typename U>
concept LessEqualComparable = requires(T a, U b) {
    { a <= b } -> std::convertible_to<bool>;
};
template <typename T, typename U>
concept GreaterComparable = requires(T a, U b) {
    { a > b } -> std::convertible_to<bool>;
};
template <typename T, typename U>
concept GreaterEqualComparable = requires(T a, U b) {
    { a >= b } -> std::convertible_to<bool>;
};
template <typename T, typename U>
concept EqualComparable = requires(T a, U b) {
    { a == b } -> std::convertible_to<bool>;
};
}  // namespace
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
            std::visit(std::bind(func, _1, std::ref(rhs)), variables_[lhs.key].value);
        } else if constexpr (std::is_convertible_v<RightType, VariableReference>) {
            std::visit([&lhs, func](auto& rhs) { func(lhs, rhs); }, variables_[rhs.key].value);
        } else {
            func(lhs, rhs);
        }
    };
    auto location = node->location();
    switch (node->op()) {
        using enum ast::BinaryOperator::OperatorType;
        using namespace std::placeholders;
        case ADD:
            std::visit(
                std::bind(
                    deref_and_apply,
                    [this, &location](auto left, auto right) {
                        using LeftType = decltype(left);
                        using RightType = decltype(right);
                        using Left = std::numeric_limits<LeftType>;
                        using Right = std::numeric_limits<RightType>;
                        if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                                      std::is_convertible_v<RightType, VariableReference>) {
                            throw TypeError(fmt::format("cannot apply ADD operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        } else if constexpr (Left::is_specialized && Right::is_specialized) {
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
                        } else if constexpr (std::is_convertible_v<LeftType, std::string> &&
                                             std::is_convertible_v<RightType, std::string>) {
                            this->expr_result_ = left + right;
                        } else {
                            throw TypeError(fmt::format("cannot apply ADD operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        }
                    },
                    _1, _2),
                lhs, rhs);
            break;
        case SUB:
            std::visit(
                std::bind(
                    deref_and_apply,
                    [this, &location](auto left, auto right) {
                        using LeftType = decltype(left);
                        using RightType = decltype(right);
                        using Left = std::numeric_limits<LeftType>;
                        using Right = std::numeric_limits<RightType>;
                        if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                                      std::is_convertible_v<RightType, VariableReference>) {
                            throw TypeError(fmt::format("cannot apply SUB operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        } else if constexpr (Left::is_specialized && Right::is_specialized) {
                            if constexpr (Left::is_integer && (not Right::is_integer)) {
                                this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) - right);
                            } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                                this->expr_result_ = static_cast<LeftType>(left - static_cast<LeftType>(right));
                            } else {
                                if constexpr (Left::digits >= Right::digits) {
                                    this->expr_result_ = static_cast<LeftType>(left - static_cast<LeftType>(right));
                                } else {
                                    this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) - right);
                                }
                            }
                        } else {
                            throw TypeError(fmt::format("cannot apply SUB operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        }
                    },
                    _1, _2),
                lhs, rhs);
            break;
        case MUL:
            std::visit(
                std::bind(
                    deref_and_apply,
                    [this, &location](auto left, auto right) {
                        using LeftType = decltype(left);
                        using RightType = decltype(right);
                        using Left = std::numeric_limits<LeftType>;
                        using Right = std::numeric_limits<RightType>;
                        if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                                      std::is_convertible_v<RightType, VariableReference>) {
                            throw TypeError(fmt::format("cannot apply MUL operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        } else if constexpr (Left::is_specialized && Right::is_specialized) {
                            if constexpr (Left::is_integer && (not Right::is_integer)) {
                                this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) * right);
                            } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                                this->expr_result_ = static_cast<LeftType>(left * static_cast<LeftType>(right));
                            } else {
                                if constexpr (Left::digits >= Right::digits) {
                                    this->expr_result_ = static_cast<LeftType>(left * static_cast<LeftType>(right));
                                } else {
                                    this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) * right);
                                }
                            }
                        } else {
                            throw TypeError(fmt::format("cannot apply MUL operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        }
                    },
                    _1, _2),
                lhs, rhs);
            break;
        case DIV:
            std::visit(
                std::bind(
                    deref_and_apply,
                    [this, &location](auto left, auto right) {
                        using LeftType = decltype(left);
                        using RightType = decltype(right);
                        using Left = std::numeric_limits<LeftType>;
                        using Right = std::numeric_limits<RightType>;
                        if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                                      std::is_convertible_v<RightType, VariableReference>) {
                            throw TypeError(fmt::format("cannot apply DIV operator to {} and {}", typeid(LeftType),

                                                        typeid(RightType)),
                                            location);
                        } else if constexpr (Left::is_specialized && Right::is_specialized) {
                            if constexpr (Left::is_integer && Right::is_integer) {
                                this->expr_result_ =
                                    static_cast<double>(static_cast<double>(left) / static_cast<double>(right));
                            } else if constexpr (Left::is_integer && (not Right::is_integer)) {
                                this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) / right);
                            } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                                this->expr_result_ = static_cast<LeftType>(left / static_cast<LeftType>(right));
                            } else {
                                if constexpr (Left::digits >= Right::digits) {
                                    this->expr_result_ = static_cast<LeftType>(left / static_cast<LeftType>(right));
                                } else {
                                    this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) / right);
                                }
                            }
                        } else {
                            throw TypeError(fmt::format("cannot apply DIV operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        }
                    },
                    _1, _2),
                lhs, rhs);
            break;
        case MOD:
            std::visit(
                std::bind(
                    deref_and_apply,
                    [this, &location](auto left, auto right) {
                        using LeftType = decltype(left);
                        using RightType = decltype(right);
                        using Left = std::numeric_limits<LeftType>;
                        using Right = std::numeric_limits<RightType>;
                        if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                                      std::is_convertible_v<RightType, VariableReference>) {
                            throw TypeError(fmt::format("cannot apply MOD operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        } else if constexpr (Left::is_specialized && Right::is_specialized) {
                            if constexpr (Left::is_integer && (not Right::is_integer)) {
                                this->expr_result_ =
                                    static_cast<RightType>(std::fmod(static_cast<RightType>(left), right));
                            } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                                this->expr_result_ =
                                    static_cast<LeftType>(std::fmod(left, static_cast<LeftType>(right)));
                            } else if constexpr (Left::is_integer && Right::is_integer) {
                                if constexpr (Left::digits >= Right::digits) {
                                    this->expr_result_ = static_cast<LeftType>(left % static_cast<LeftType>(right));
                                } else {
                                    this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) % right);
                                }
                            } else {
                                if constexpr (Left::digits >= Right::digits) {
                                    this->expr_result_ =
                                        static_cast<LeftType>(std::fmod(left, static_cast<LeftType>(right)));
                                } else {
                                    this->expr_result_ =
                                        static_cast<RightType>(std::fmod(static_cast<RightType>(left), right));
                                }
                            }
                        } else {
                            throw TypeError(fmt::format("cannot apply MOD operator to {} and {}", typeid(LeftType),
                                                        typeid(RightType)),
                                            location);
                        }
                    },
                    _1, _2),
                lhs, rhs);
            break;
        case ASSIGN: {
            // 左辺は必ず参照でなくてはならないので、deref_and_applyは使えない
            if (not std::holds_alternative<VariableReference>(lhs)) {
                throw TypeError("cannot assign to rvalue", location);
            }
            auto& left = variables_.at(std::get<VariableReference>(lhs).key);
            Value right = rhs;
            if (std::holds_alternative<VariableReference>(rhs)) {
                right = variables_.at(std::get<VariableReference>(rhs).key).value;
            }
            std::visit(
                [this, &location](auto& left, auto right) {
                    using LeftType = decltype(left);
                    using RightType = decltype(right);
                    if constexpr (not StaticConvertible<std::remove_cvref_t<RightType>,
                                                        std::remove_cvref_t<LeftType>>) {
                        throw TypeError(fmt::format("cannot ASSIGN a value with type {} into a variable with type {}",
                                                    typeid(RightType), typeid(LeftType)),
                                        location);
                    } else {
                        left = static_cast<std::remove_cvref_t<LeftType>>(right);
                    }
                },
                left.value, right);
        } break;
        case LESS:
            std::visit(std::bind(
                           deref_and_apply,
                           [this, &location](auto left, auto right) {
                               using LeftType = decltype(left);
                               using RightType = decltype(right);
                               if constexpr (LessComparable<LeftType, RightType>) {
                                   this->expr_result_ = left < right;
                               } else {
                                   throw TypeError(fmt::format("cannot apply LESS operator to {} and {}",
                                                               typeid(LeftType), typeid(RightType)),
                                                   location);
                               }
                           },
                           _1, _2),
                       lhs, rhs);
            break;
        case LESS_EQUAL:
            std::visit(std::bind(
                           deref_and_apply,
                           [this, &location](auto left, auto right) {
                               using LeftType = decltype(left);
                               using RightType = decltype(right);
                               if constexpr (LessEqualComparable<LeftType, RightType>) {
                                   this->expr_result_ = left <= right;
                               } else {
                                   throw TypeError(fmt::format("cannot apply LESS_EQUAL operator to {} and {}",
                                                               typeid(LeftType), typeid(RightType)),
                                                   location);
                               }
                           },
                           _1, _2),
                       lhs, rhs);
            break;
        case GREATER:
            std::visit(std::bind(
                           deref_and_apply,
                           [this, &location](auto left, auto right) {
                               using LeftType = decltype(left);
                               using RightType = decltype(right);
                               if constexpr (GreaterComparable<LeftType, RightType>) {
                                   this->expr_result_ = left > right;
                               } else {
                                   throw TypeError(fmt::format("cannot apply GREATER operator to {} and {}",
                                                               typeid(LeftType), typeid(RightType)),
                                                   location);
                               }
                           },
                           _1, _2),
                       lhs, rhs);
            break;
        case GREATER_EQUAL:
            std::visit(std::bind(
                           deref_and_apply,
                           [this, &location](auto left, auto right) {
                               using LeftType = decltype(left);
                               using RightType = decltype(right);
                               if constexpr (GreaterEqualComparable<LeftType, RightType>) {
                                   this->expr_result_ = left >= right;
                               } else {
                                   throw TypeError(fmt::format("cannot apply GREATER_EQUAL operator to {} and {}",
                                                               typeid(LeftType), typeid(RightType)),
                                                   location);
                               }
                           },
                           _1, _2),
                       lhs, rhs);
            break;
        case EQUAL:
            std::visit(std::bind(
                           deref_and_apply,
                           [this, &location](auto left, auto right) {
                               using LeftType = decltype(left);
                               using RightType = decltype(right);
                               if constexpr (EqualComparable<LeftType, RightType>) {
                                   this->expr_result_ = left == right;
                               } else {
                                   throw TypeError(fmt::format("cannot apply GREATER_EQUAL operator to {} and {}",
                                                               typeid(LeftType), typeid(RightType)),
                                                   location);
                               }
                           },
                           _1, _2),
                       lhs, rhs);
            break;
    }
}
void Interpreter::visit(const ast::VariableReference* node) {
    Scope* scope = current_scope_;
    auto name = node->name().source_id();
    while (scope != nullptr) {
        if (scope->keymap->contains(name)) {
            // ASSIGN演算子の都合上値ではなく参照を返す
            expr_result_ = VariableReference((*(scope->keymap))[name]);
            return;
        }
        scope = scope->parent;
    }
    throw NameError(fmt::format("variable {} is not defined", name), node->location());
}
void Interpreter::visit(const ast::SignedIntegerLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::UnsignedIntegerLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::FloatingPointLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::StringLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::FunctionCall* node) {
    node->callee()->accept(*this);
    auto raw_callee = expr_result_;
    while (std::holds_alternative<VariableReference>(raw_callee)) {
        raw_callee = variables_.at(std::get<VariableReference>(raw_callee).key).value;
    }
    std::visit(
        [this, node](auto callee) {
            if constexpr (not std::is_convertible_v<decltype(callee), FunctionReference>) {
                throw TypeError(fmt::format("{} cannot be called", typeid(callee)), node->location());
            } else {
                ArgType args;
                for (const auto arg : node->args()) {
                    arg->accept(*this);
                    auto& raw = *arg;
                    args.push_back(expr_result_);
                }
                expr_result_ = functions_[callee.key](args, {});
            }
        },
        raw_callee);
}
void Interpreter::visit(const ast::CompilationUnit* node) {
    Scope scope(nullptr, &variables_);
    current_scope_ = &scope;
    global_scope_ = &scope;
    init_builtin_functions_();
    for (const auto& child : node->children()) {
        const auto& raw = *child;
        if (typeid(raw) == typeid(ast::FunctionDef)) {
            child->accept(*this);
        } else if (typeid(raw) == typeid(ast::VariableDecl)) {
            child->accept(*this);
        }
    }
    functions_[encode_function_key_("main")]({0}, {});
    current_scope_ = nullptr;
}
void Interpreter::visit(const ast::FunctionDef* node) {
    auto info = node->info();
    register_function_(info.name().source_name(), [this, node, info](const ArgType& args, const KwArgType& kwargs) {
        auto previous_scope = current_scope_;
        Scope arg_scope(global_scope_, &variables_);
        current_scope_ = &arg_scope;
        auto arg_iter = args.begin();
        for (const auto& arginfo : info.args()) {
            Value arg_value;
            auto name = arginfo.name();
            if (kwargs.contains(name.source_id())) {
                arg_value = kwargs.at(name.source_id());
            } else if (arg_iter != args.end()) {
                arg_value = *arg_iter;
                ++arg_iter;
            } else {
                throw InvalidArgument("insufficient argument", node->location());
            }
            declare_variable_(name, arginfo.type().name(), arg_value, arginfo.location());
        }
        auto return_type = info.result()->type().name();
        using namespace ast::operators;
        static const ast::SourceTypeIdentifier void_type{"void"};
        static const ast::SourceTypeIdentifier nil_type{"NilType"};
        if (return_type == void_type) {
            return_type = nil_type;
        }
        auto return_loc = info.result()->location();
        declare_variable_(info.result()->name(), return_type, std::nullopt, return_loc);
        node->block()->accept(*this);
        auto result = variables_.at(current_scope_->keymap->at(RETURN_SPECIAL_VARNAME_ID));
        is_returned_ = false;
        current_scope_ = previous_scope;
        return result.value;
    });
}
void Interpreter::visit(const ast::VariableDeclStatement* node) {
    for (const auto& child : node->children()) {
        child->accept(*this);
    }
}
void Interpreter::visit(const ast::ReturnStatement* node) {
    ast::BinaryOperator(ast::BinaryOperator::OperatorType::ASSIGN,
                        std::make_shared<ast::VariableReference>(ast::SourceVariableIdentifier{RETURN_SPECIAL_VARNAME},
                                                                 ValRef::VALUE, node->location()),
                        node->retval(), node->location())
        .accept(*this);
    is_returned_ = true;
}
void Interpreter::visit(const ast::Block* node) {
    Scope scope(current_scope_, &variables_);
    current_scope_ = &scope;
    for (const auto& sentence : node->sentences()) {
        sentence->accept(*this);
        if (is_broken_ && is_returned_) {
            break;
        }
    }
    current_scope_ = scope.parent;
}
void Interpreter::visit(const ast::LoopStatement* node) {
    while (not is_broken_) {
        node->block()->accept(*this);
    }
    is_broken_ = false;
}
void Interpreter::visit(const ast::BreakStatement* node) { is_broken_ = true; }
void Interpreter::visit(const ast::IfStatement* node) {
    for (auto [raw_cond, block] : node->cond_blocks()) {
        if (raw_cond.use_count() == 0) {
            block->accept(*this);
            break;
        }
        raw_cond->accept(*this);
        bool cond = false;
        std::visit(
            [this, raw_cond, &cond](auto value) {
                if constexpr (std::is_convertible_v<decltype(value), bool>) {
                    cond = value;
                } else {
                    throw TypeError(fmt::format("{} cannot be converted to bool", typeid(value)), raw_cond->location());
                }
            },
            expr_result_);
        if (cond) {
            block->accept(*this);
            break;
        }
    }
}
Interpreter::FunctionKey Interpreter::encode_function_key_(const std::string& name) const {
    return SimpleFlyWeight::instance().id(name);
}
Interpreter::TypeKey Interpreter::encode_type_key_(std::string name) const {
    return SimpleFlyWeight::instance().id(name);
}
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
    types_[encode_type_key_("str")] = [] { return Value(static_cast<std::string>("")); };
    types_[encode_type_key_("NilType")] = [] { return Value(static_cast<NilType>(NilType{})); };
}
void Interpreter::debug_print() const { fmt::println("variables: {}", variables_); }
std::string Interpreter::Variable::to_string() const {
    return fmt::format("Variable(name: {}, value: {})", name_id, value);
}
std::string Interpreter::VariableReference::to_string() const { return fmt::format("VariableReference(key: {})", key); }
std::string Interpreter::FunctionReference::to_string() const { return fmt::format("FunctionReference(key: {})", key); }
Interpreter::Value Interpreter::print_(ArgType args, KwArgType kwargs) {
    for (size_t i = 0; auto arg : args) {
        // VariableReferenceExpressionの都合上どんな変数でもVariableReferenceで覆われているので、剥がす
        // その中身がVariableReferenceでもそれは追わない
        if (std::holds_alternative<VariableReference>(arg)) {
            arg = variables_.at(std::get<VariableReference>(arg).key).value;
        }
        std::visit([](auto value) { fmt::print("{}", value); }, arg);
        if (i < args.size() - 1) {
            fmt::print(" ");
        }
        i++;
    }
    return NilType{};
}
Interpreter::Value Interpreter::println_(ArgType args, KwArgType kwargs) {
    print_(args, kwargs);
    fmt::println("");
    return NilType{};
}
void Interpreter::init_builtin_functions_() {
    using namespace std::placeholders;
    register_function_("print", std::bind(std::mem_fn(&Interpreter::print_), std::ref(*this), _1, _2));
    register_function_("println", std::bind(std::mem_fn(&Interpreter::println_), std::ref(*this), _1, _2));
}
void Interpreter::register_function_(const std::string& name, Function func) {
    functions_[encode_function_key_(name)] = func;
    VariableKey var_key = key_generator_();
    variables_[var_key] = {.name_id = SimpleFlyWeight::instance().id(name),
                           .value = FunctionReference{encode_function_key_(name)}};
    (*(global_scope_->keymap))[SimpleFlyWeight::instance().id(name)] = var_key;
}
Interpreter::Scope::~Scope() {
    for (const auto& [name, key] : *keymap) {
        varmap_->erase(key);
    }
    keymap->clear();
    InstancePool<KeyMapType>::return_instance(keymap);
}
}  // namespace Garnet::interpreter
