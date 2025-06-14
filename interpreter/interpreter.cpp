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
#include "format.hpp"  // NOLINT
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
template <typename T, typename U>
concept NotEqualComparable = requires(T a, U b) {
    { a != b } -> std::convertible_to<bool>;
};
template <typename T, typename U>
concept SignednessMatch =
    (std::signed_integral<T> && std::signed_integral<U>) || (std::unsigned_integral<T> && std::unsigned_integral<U>);
template <typename T, typename U>
concept BothFloat = (std::is_floating_point_v<T> && std::is_floating_point_v<U>);
template <typename T, typename U>
concept StrictComparable = BothFloat<T, U> || SignednessMatch<T, U>;
}  // namespace
void Interpreter::visit(const ast::BinaryOperator* node) {
    node->left()->accept(*this);

    auto lhs = expr_result_;

    node->right()->accept(*this);

    auto rhs = expr_result_;
    auto location = node->location();
    switch (node->op()) {
        using enum ast::BinaryOperator::OperatorType;
        using namespace std::placeholders;
        case ADD:
            apply_ADD_(lhs, rhs, location);
            break;
        case SUB:
            apply_SUB_(lhs, rhs, location);
            break;
        case MUL:
            apply_MUL_(lhs, rhs, location);
            break;
        case DIV:
            apply_DIV_(lhs, rhs, location);
            break;
        case MOD:
            apply_MOD_(lhs, rhs, location);
            break;
        case ASSIGN:
            apply_ASSIGN_(lhs, rhs, location);
            break;
        case LESS:
            apply_LESS_(lhs, rhs, location);
            break;
        case LESS_EQUAL:
            apply_LESS_EQUAL_(lhs, rhs, location);
            break;
        case GREATER:
            apply_GREATER_(lhs, rhs, location);
            break;
        case GREATER_EQUAL:
            apply_GREATER_EQUAL_(lhs, rhs, location);
            break;
        case EQUAL:
            apply_EQUAL_(lhs, rhs, location);
            break;
        case NOT_EQUAL:
            apply_NOT_EQUAL_(lhs, rhs, location);
            break;
        case BOOL_AND:
            apply_BOOL_AND_(lhs, rhs, location);
            break;
        case BIT_AND:
            apply_BIT_AND_(lhs, rhs, location);
            break;
        case BOOL_OR:
            apply_BOOL_OR_(lhs, rhs, location);
            break;
        case BIT_OR:
            apply_BIT_OR_(lhs, rhs, location);
            break;
        case BIT_XOR:
            apply_BIT_XOR_(lhs, rhs, location);
            break;
        case LEFT_SHIFT:
            apply_LEFT_SHIFT_(lhs, rhs, location);
            break;
        case RIGHT_SHIFT:
            apply_RIGHT_SHIFT_(lhs, rhs, location);
            break;
    }
}
void Interpreter::visit(const ast::UnaryOperator* node) {
    node->operand()->accept(*this);

    auto operand = expr_result_;

    while (std::holds_alternative<VariableReference>(operand)) {
        operand = variables_[std::get<VariableReference>(operand).key].value;
    }

    auto location = node->location();
    switch (node->op()) {
        using enum ast::UnaryOperator::OperatorType;
        using namespace std::placeholders;
        case PLUS:
            std::visit(
                [this, &location](auto operand) {
                    using OperandType = decltype(operand);
                    using Operand = std::numeric_limits<OperandType>;
                    if constexpr (std::is_convertible_v<OperandType, VariableReference>) {
                        throw TypeError(fmt::format("cannot apply unary PLUS operator to {}", typeid(OperandType)),
                                        location);
                    } else if constexpr (Operand::is_specialized) {
                        this->expr_result_ = static_cast<OperandType>(+operand);
                    } else {
                        throw TypeError(fmt::format("cannot apply unary PLUS operator to {}", typeid(OperandType)),
                                        location);
                    }
                },
                operand);
            break;
        case MINUS:
            std::visit(
                [this, &location](auto operand) {
                    using OperandType = decltype(operand);
                    using Operand = std::numeric_limits<OperandType>;
                    if constexpr (std::is_convertible_v<OperandType, VariableReference>) {
                        throw TypeError(fmt::format("cannot apply unary PLUS operator to {}", typeid(OperandType)),
                                        location);
                    } else if constexpr (Operand::is_specialized) {
                        this->expr_result_ = static_cast<OperandType>(-operand);
                    } else {
                        throw TypeError(fmt::format("cannot apply unary MINUS operator to {}", typeid(OperandType)),
                                        location);
                    }
                },
                operand);
            break;
        case BOOL_NOT:
            std::visit(
                [this, &location](auto operand) {
                    using OperandType = decltype(operand);
                    if constexpr (std::is_same_v<std::remove_cvref_t<OperandType>, bool>) {
                        this->expr_result_ = static_cast<bool>(not operand);
                    } else {
                        throw TypeError(fmt::format("cannot apply BOOL_NOT operator to {}", typeid(OperandType)),
                                        location);
                    }
                },
                operand);
            break;
        case BIT_NOT:
            std::visit(
                [this, &location](auto operand) {
                    using OperandType = decltype(operand);
                    using Operand = std::numeric_limits<OperandType>;
                    if constexpr (Operand::is_specialized && Operand::is_integer) {
                        if constexpr (not std::is_same_v<std::remove_cvref_t<OperandType>, bool>) {
                            this->expr_result_ = static_cast<OperandType>(~operand);
                        } else {
                            this->expr_result_ = !operand;
                        }
                    } else {
                        throw TypeError(fmt::format("cannot apply BIT_NOT operator to {}", typeid(OperandType)),
                                        location);
                    }
                },
                operand);
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
    throw NameError(fmt::format("variable '{}' is not defined", SimpleFlyWeight::instance().value(name)),
                    node->location());
}
void Interpreter::visit(const ast::SignedIntegerLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::UnsignedIntegerLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::FloatingPointLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::StringLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::BooleanLiteral* node) { expr_result_ = node->value(); }
void Interpreter::visit(const ast::NilLiteral*) { expr_result_ = NilType{}; }
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
                for (const auto& arg : node->args()) {
                    arg->accept(*this);
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
        if (is_broken_ || is_returned_) {
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
void Interpreter::visit(const ast::BreakStatement*) { is_broken_ = true; }
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
void Interpreter::visit(const ast::AssertStatement* node) {
    node->cond()->accept(*this);
    auto cond_loc = node->cond()->location();
    std::visit(
        [this, cond_loc, node](auto cond) {
            if constexpr (not std::is_same_v<std::remove_cvref_t<decltype(cond)>, bool>) {
                throw TypeError(fmt::format("{} is not bool", typeid(cond)), cond_loc);
            } else if (not cond) {
                if (node->msg().has_value()) {
                    node->msg().value()->accept(*this);
                    throw AssertionError(fmt::format("{}", expr_result_), cond_loc);
                } else {
                    throw AssertionError("", cond_loc);
                }
            }
        },
        expr_result_);
}
void Interpreter::visit(const ast::StructDef* node) {
    throw UnImplementedError("excetution of StructDef is not implemented", node->location());
}
void Interpreter::visit(const ast::MemberAccess* node) {
    throw UnImplementedError("excetution of MemberAccess is not implemented", node->location());
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
    (void)kwargs;
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

// 計算結果やOperandの値を保持するためのvariant
using ResultVariant = std::variant<std::uint64_t, std::int64_t, double, std::string>;

// 元のValueの型情報をタグとして保持するenum
enum class TypeTag { Nil, U8, I8, U16, I16, U32, I32, U64, I64, F32, F64, Bool, String, Other };

// 演算に必要な情報をまとめた中間表現オブジェクト
struct Operand {
    TypeTag original_tag = TypeTag::Other;
    ResultVariant value;
};

// --- ヘルパー関数群 (ボトムアップに定義) ---

// [ヘルパー 1] Valueを中間表現Operandに変換する
Operand to_operand(const Interpreter::Value& val) {
    Operand op;
    std::visit(
        [&op](auto v) {
            using T = std::remove_cvref_t<decltype(v)>;
            // 元の型タグを設定
            if constexpr (std::is_same_v<T, std::uint8_t>)
                op.original_tag = TypeTag::U8;
            else if constexpr (std::is_same_v<T, std::int8_t>)
                op.original_tag = TypeTag::I8;
            else if constexpr (std::is_same_v<T, std::uint16_t>)
                op.original_tag = TypeTag::U16;
            else if constexpr (std::is_same_v<T, std::int16_t>)
                op.original_tag = TypeTag::I16;
            else if constexpr (std::is_same_v<T, std::uint32_t>)
                op.original_tag = TypeTag::U32;
            else if constexpr (std::is_same_v<T, std::int32_t>)
                op.original_tag = TypeTag::I32;
            else if constexpr (std::is_same_v<T, std::uint64_t>)
                op.original_tag = TypeTag::U64;
            else if constexpr (std::is_same_v<T, std::int64_t>)
                op.original_tag = TypeTag::I64;
            else if constexpr (std::is_same_v<T, float>)
                op.original_tag = TypeTag::F32;
            else if constexpr (std::is_same_v<T, double>)
                op.original_tag = TypeTag::F64;
            else if constexpr (std::is_same_v<T, std::string>)
                op.original_tag = TypeTag::String;

            // カテゴリに合った型でvariantに値を格納
            if constexpr (std::is_unsigned_v<T> && std::is_integral_v<T>) {
                op.value = static_cast<std::uint64_t>(v);
            } else if constexpr (std::is_signed_v<T> && std::is_integral_v<T>) {
                op.value = static_cast<std::int64_t>(v);
            } else if constexpr (std::is_floating_point_v<T>) {
                op.value = static_cast<double>(v);
            } else if constexpr (std::is_same_v<T, std::string>) {
                op.value = v;
            }
        },
        val);
    return op;
}

// [ヘルパー 2] 型タグを判定・評価するための関数群
bool is_unsigned_int(TypeTag tag) {
    return tag == TypeTag::U8 || tag == TypeTag::U16 || tag == TypeTag::U32 || tag == TypeTag::U64;
}
bool is_signed_int(TypeTag tag) {
    return tag == TypeTag::I8 || tag == TypeTag::I16 || tag == TypeTag::I32 || tag == TypeTag::I64;
}
bool is_int(TypeTag tag) { return is_unsigned_int(tag) || is_signed_int(tag); }
int get_rank(TypeTag tag) {
    switch (tag) {
        case TypeTag::I8:
        case TypeTag::U8:
            return 1;
        case TypeTag::I16:
        case TypeTag::U16:
            return 2;
        case TypeTag::I32:
        case TypeTag::U32:
            return 3;
        case TypeTag::F32:
            return 4;
        case TypeTag::I64:
        case TypeTag::U64:
            return 5;
        case TypeTag::F64:
            return 6;
        default:
            return 0;
    }
}

// [ヘルパー 3] 計算結果のvariantから暫定的な型タグを導出
TypeTag get_preliminary_tag_from_result(const ResultVariant& result_val) {
    return std::visit(
        [](auto val) -> TypeTag {
            using T = std::remove_cvref_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::uint64_t>) return TypeTag::U64;
            if constexpr (std::is_same_v<T, std::int64_t>) return TypeTag::I64;
            if constexpr (std::is_same_v<T, double>) return TypeTag::F64;
            if constexpr (std::is_same_v<T, std::string>) return TypeTag::String;
            return TypeTag::Other;
        },
        result_val);
}

// [ヘルパー 4] 3つの型タグからプロモーションルールに基づき最終的な型タグを決定
TypeTag derive_final_tag(TypeTag leftTag, TypeTag rightTag, TypeTag preliminary_result_tag) {
    if (is_unsigned_int(leftTag) && is_unsigned_int(rightTag) && is_unsigned_int(preliminary_result_tag)) {
        return (get_rank(leftTag) >= get_rank(rightTag)) ? leftTag : rightTag;
    } else if ((is_signed_int(leftTag) || is_signed_int(rightTag)) && is_int(preliminary_result_tag)) {
        int l_rank = get_rank(leftTag);
        int r_rank = get_rank(rightTag);
        if (l_rank > r_rank) return leftTag;
        if (r_rank > l_rank) return rightTag;
        return is_signed_int(leftTag) ? leftTag : rightTag;
    } else {
        return preliminary_result_tag;
    }
}

// [ヘルパー 5] 最終的な型タグと計算結果からValueオブジェクトを生成
Interpreter::Value make_value_from_result(TypeTag final_tag, const ResultVariant& result_val) {
    auto caster = [&](auto numeric_val) -> Interpreter::Value {
        using Value = Interpreter::Value;
        if constexpr (std::is_same_v<decltype(numeric_val), std::string>) {
            if (final_tag != TypeTag::String) {
                throw TypeError("cannot convert string to non-string type", location::SourceRegion{});
            }
            return Value{std::get<std::string>(result_val)};
        } else {
            switch (final_tag) {
                case TypeTag::U8:
                    return Value{static_cast<std::uint8_t>(numeric_val)};
                case TypeTag::I8:
                    return Value{static_cast<std::int8_t>(numeric_val)};
                case TypeTag::U16:
                    return Value{static_cast<std::uint16_t>(numeric_val)};
                case TypeTag::I16:
                    return Value{static_cast<std::int16_t>(numeric_val)};
                case TypeTag::U32:
                    return Value{static_cast<std::uint32_t>(numeric_val)};
                case TypeTag::I32:
                    return Value{static_cast<std::int32_t>(numeric_val)};
                case TypeTag::U64:
                    return Value{static_cast<std::uint64_t>(numeric_val)};
                case TypeTag::I64:
                    return Value{static_cast<std::int64_t>(numeric_val)};
                case TypeTag::F32:
                    return Value{static_cast<float>(numeric_val)};
                case TypeTag::F64:
                    return Value{static_cast<double>(numeric_val)};
                case TypeTag::String:
                    return Value{std::get<std::string>(result_val)};
                default:
                    return {};
            }
        }
    };
    if (final_tag == TypeTag::String) return caster(0);
    return std::visit(caster, result_val);
}

// --- 演算実行のオーケストレーター ---

// 任意の二項演算を処理する、汎用的なテンプレート関数
template <typename Operation>
Interpreter::Value calculate_operands(const Operand& lhs, const Operand& rhs, Operation op) {
    return std::visit(
        [&](auto l_val, auto r_val) -> Interpreter::Value {
            // ステップ1: 渡された関数オブジェクト `op` で計算を実行
            ResultVariant result_val = op(l_val, r_val);
            if (result_val.index() == 0 && !std::holds_alternative<std::uint64_t>(result_val)) {
                return {};
            }

            // ステップ2: 暫定的な結果型の導出
            TypeTag preliminary_tag = get_preliminary_tag_from_result(result_val);

            // ステップ3: 最終的な結果型の決定
            TypeTag final_tag = derive_final_tag(lhs.original_tag, rhs.original_tag, preliminary_tag);

            // ステップ4: Valueオブジェクトの最終生成
            return make_value_from_result(final_tag, result_val);
        },
        lhs.value, rhs.value);
}

// --- Interpreterクラスのメソッド実装 ---

const Interpreter::Value& Interpreter::deref_(const Interpreter::Value& val) {
    if (std::holds_alternative<VariableReference>(val)) {
        const auto& ref = std::get<VariableReference>(val);
        return variables_.at(ref.key).value;
    }
    return val;
}

const std::type_info& Interpreter::get_type_info(const Interpreter::Value& val) const {
    return std::visit(
        [](const auto& v) -> const std::type_info& {
            using T = std::remove_cvref_t<decltype(v)>;
            return typeid(T);
        },
        val);
}

// private:
// 全ての二項演算の実行パイプライン
template <typename Operation>
void Interpreter::execute_binary_operation_(const Value& lhs, const Value& rhs, Operation op,
                                            const char* op_name,  // エラーメッセージ用
                                            const location::SourceRegion& location) {
    // パイプライン 1: 変数参照の解決
    const Value& left_val = deref_(lhs);
    const Value& right_val = deref_(rhs);

    // パイプライン 2: 中間表現への変換
    Operand lhs_op = to_operand(left_val);
    Operand rhs_op = to_operand(right_val);

    // パイプライン 3: 演算の実行と型決定
    try {
        Value result = calculate_operands(lhs_op, rhs_op, op);
        this->expr_result_ = result;
    } catch (const std::exception& e) {
        throw TypeError(fmt::format("cannot apply {} operator to {} and {}: {}", op_name, get_type_info(left_val),
                                    get_type_info(right_val), e.what()),
                        location);
    }
}

void Interpreter::apply_ADD_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    // 足し算の計算ロジックをラムダとして定義
    auto add_op = [](auto l_val, auto r_val) -> ResultVariant {
        if constexpr (std::is_same_v<decltype(l_val), std::string> && std::is_same_v<decltype(r_val), std::string>) {
            return l_val + r_val;
        } else if constexpr (std::is_arithmetic_v<decltype(l_val)> && std::is_arithmetic_v<decltype(r_val)>) {
            if constexpr (std::is_same_v<decltype(l_val), double> || std::is_same_v<decltype(r_val), double>)
                return static_cast<double>(l_val) + static_cast<double>(r_val);
            else if constexpr (std::is_same_v<decltype(l_val), std::uint64_t> &&
                               std::is_same_v<decltype(r_val), std::uint64_t>)
                return l_val + r_val;
            else
                return static_cast<std::int64_t>(l_val) + static_cast<std::int64_t>(r_val);
        }
        return {};
    };

    // 新設した共通パイプラインに、足し算ロジックを渡して実行
    execute_binary_operation_(lhs, rhs, add_op, "ADD", location);
}

void Interpreter::apply_SUB_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
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
}
void Interpreter::apply_MUL_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
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
}
void Interpreter::apply_DIV_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
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
}
void Interpreter::apply_MOD_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(
        std::bind(
            deref_and_apply_func_(),
            [this, &location](auto left, auto right) {
                using LeftType = decltype(left);
                using RightType = decltype(right);
                using Left = std::numeric_limits<LeftType>;
                using Right = std::numeric_limits<RightType>;
                if constexpr (std::is_convertible_v<LeftType, VariableReference> ||
                              std::is_convertible_v<RightType, VariableReference>) {
                    throw TypeError(
                        fmt::format("cannot apply MOD operator to {} and {}", typeid(LeftType), typeid(RightType)),
                        location);
                } else if constexpr (Left::is_specialized && Right::is_specialized) {
                    if constexpr (Left::is_integer && (not Right::is_integer)) {
                        this->expr_result_ = static_cast<RightType>(std::fmod(static_cast<RightType>(left), right));
                    } else if constexpr ((not Left::is_integer) && Right::is_integer) {
                        this->expr_result_ = static_cast<LeftType>(std::fmod(left, static_cast<LeftType>(right)));
                    } else if constexpr (Left::is_integer && Right::is_integer) {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(left % static_cast<LeftType>(right));
                        } else {
                            this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) % right);
                        }
                    } else {
                        if constexpr (Left::digits >= Right::digits) {
                            this->expr_result_ = static_cast<LeftType>(std::fmod(left, static_cast<LeftType>(right)));
                        } else {
                            this->expr_result_ = static_cast<RightType>(std::fmod(static_cast<RightType>(left), right));
                        }
                    }
                } else {
                    throw TypeError(
                        fmt::format("cannot apply MOD operator to {} and {}", typeid(LeftType), typeid(RightType)),
                        location);
                }
            },
            _1, _2),
        lhs, rhs);
}
void Interpreter::apply_ASSIGN_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
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
            if constexpr (not StaticConvertible<std::remove_cvref_t<RightType>, std::remove_cvref_t<LeftType>>) {
                throw TypeError(fmt::format("cannot ASSIGN a value with type {} into a variable with type {}",
                                            typeid(RightType), typeid(LeftType)),
                                location);
            } else {
                left = static_cast<std::remove_cvref_t<LeftType>>(right);
            }
        },
        left.value, right);
}
void Interpreter::apply_LESS_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = decltype(left);
                       using RightType = decltype(right);
                       if constexpr (LessComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                           this->expr_result_ = left < right;
                       } else {
                           throw TypeError(fmt::format("cannot apply LESS operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_GREATER_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = decltype(left);
                       using RightType = decltype(right);
                       if constexpr (GreaterComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                           this->expr_result_ = left > right;
                       } else {
                           throw TypeError(fmt::format("cannot apply GREATER operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_LESS_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(
        std::bind(
            deref_and_apply_func_(),
            [this, &location](auto left, auto right) {
                using LeftType = decltype(left);
                using RightType = decltype(right);
                if constexpr (LessEqualComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                    this->expr_result_ = left <= right;
                } else {
                    throw TypeError(fmt::format("cannot apply LESS_EQUAL operator to {} and {}", typeid(LeftType),
                                                typeid(RightType)),
                                    location);
                }
            },
            _1, _2),
        lhs, rhs);
}
void Interpreter::apply_GREATER_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(
        std::bind(
            deref_and_apply_func_(),
            [this, &location](auto left, auto right) {
                using LeftType = decltype(left);
                using RightType = decltype(right);
                if constexpr (GreaterEqualComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                    this->expr_result_ = left >= right;
                } else {
                    throw TypeError(fmt::format("cannot apply GREATER_EQUAL operator to {} and {}", typeid(LeftType),
                                                typeid(RightType)),
                                    location);
                }
            },
            _1, _2),
        lhs, rhs);
}
void Interpreter::apply_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = decltype(left);
                       using RightType = decltype(right);
                       if constexpr (EqualComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                           this->expr_result_ = left == right;
                       } else {
                           throw TypeError(fmt::format("cannot apply EQUAL operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_NOT_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(
        std::bind(
            deref_and_apply_func_(),
            [this, &location](auto left, auto right) {
                using LeftType = decltype(left);
                using RightType = decltype(right);
                if constexpr (NotEqualComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                    this->expr_result_ = left != right;
                } else if constexpr (EqualComparable<LeftType, RightType> && StrictComparable<LeftType, RightType>) {
                    this->expr_result_ = not(left == right);
                } else {
                    throw TypeError(fmt::format("cannot apply NOT_EQUAL operator to {} and {}", typeid(LeftType),
                                                typeid(RightType)),
                                    location);
                }
            },
            _1, _2),
        lhs, rhs);
}
void Interpreter::apply_BOOL_AND_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       if constexpr (std::is_same_v<LeftType, bool> && std::is_same_v<RightType, bool>) {
                           this->expr_result_ = left && right;
                       } else {
                           throw TypeError(fmt::format("cannot apply BOOL_AND operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_BIT_AND_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       using Left = std::numeric_limits<LeftType>;
                       using Right = std::numeric_limits<RightType>;
                       if constexpr (std::is_integral_v<LeftType> && std::is_integral_v<RightType>) {
                           if constexpr (Left::digits > Right::digits) {
                               this->expr_result_ = static_cast<LeftType>(left & static_cast<LeftType>(right));
                           } else {
                               this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) & right);
                           }
                       } else {
                           throw TypeError(fmt::format("cannot apply BIT_AND operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_BOOL_OR_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       if constexpr (std::is_same_v<LeftType, bool> && std::is_same_v<RightType, bool>) {
                           this->expr_result_ = left || right;
                       } else {
                           throw TypeError(fmt::format("cannot apply BOOL_OR operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_BIT_OR_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       using Left = std::numeric_limits<LeftType>;
                       using Right = std::numeric_limits<RightType>;
                       if constexpr (std::is_integral_v<LeftType> && std::is_integral_v<RightType>) {
                           if constexpr (Left::digits > Right::digits) {
                               this->expr_result_ = static_cast<LeftType>(left | static_cast<LeftType>(right));
                           } else {
                               this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) | right);
                           }
                       } else {
                           throw TypeError(fmt::format("cannot apply BIT_OR operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_BIT_XOR_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       using Left = std::numeric_limits<LeftType>;
                       using Right = std::numeric_limits<RightType>;
                       if constexpr (std::is_integral_v<LeftType> && std::is_integral_v<RightType>) {
                           if constexpr (Left::digits > Right::digits) {
                               this->expr_result_ = static_cast<LeftType>(left ^ static_cast<LeftType>(right));
                           } else {
                               this->expr_result_ = static_cast<RightType>(static_cast<RightType>(left) ^ right);
                           }
                       } else {
                           throw TypeError(fmt::format("cannot apply BIT_XOR operator to {} and {}", typeid(LeftType),
                                                       typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_LEFT_SHIFT_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       if constexpr (std::is_integral_v<LeftType> && std::is_integral_v<RightType>) {
                           this->expr_result_ = static_cast<LeftType>(left << right);
                       } else {
                           throw TypeError(fmt::format("cannot apply LEFT_SHIFT operator to {} and {}",
                                                       typeid(LeftType), typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
void Interpreter::apply_RIGHT_SHIFT_(const Value& lhs, const Value& rhs, location::SourceRegion& location) {
    using namespace std::placeholders;
    std::visit(std::bind(
                   deref_and_apply_func_(),
                   [this, &location](auto left, auto right) {
                       using LeftType = std::remove_cvref_t<decltype(left)>;
                       using RightType = std::remove_cvref_t<decltype(right)>;
                       if constexpr (std::is_integral_v<LeftType> && std::is_integral_v<RightType>) {
                           this->expr_result_ = static_cast<LeftType>(left >> right);
                       } else {
                           throw TypeError(fmt::format("cannot apply RIGHT_SHIFT operator to {} and {}",
                                                       typeid(LeftType), typeid(RightType)),
                                           location);
                       }
                   },
                   _1, _2),
               lhs, rhs);
}
}  // namespace Garnet::interpreter
