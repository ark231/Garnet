#ifndef GARNET_INTERPRETER_INTERPRETER
#define GARNET_INTERPRETER_INTERPRETER
// #include <boost/uuid/random_generator.hpp>
// #include <boost/uuid/uuid.hpp>
#include <fmt/base.h>
#include <fmt/format.h>

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include "concrete_source_identifiers.hpp"
#include "flyweight.hpp"
#include "format_support.hpp"  // NOLINT
#include "instance_pool.hpp"
#include "location.hpp"
#include "visitor/visitor.hpp"
namespace Garnet {
namespace ast {
class Expression;
}
namespace interpreter {
class Interpreter : public ast::Visitor {
   public:
    // using VariableKey = boost::uuids::uuid;
    // boost::uuids::random_generator key_generator_;
    using VariableKey = std::uint64_t;
    VariableKey counter_ = 0;
    VariableKey key_generator_() { return counter_++; }
    struct VariableReference {
        VariableKey key;
        explicit VariableReference() = default;
        explicit VariableReference(VariableKey key) : key(key) {}

        std::string to_string() const;
    };

    using FunctionKey = SimpleFlyWeight::id_type;
    FunctionKey encode_function_key_(const std::string& name) const;
    struct FunctionReference {
        FunctionKey key;
        explicit FunctionReference() = default;
        explicit FunctionReference(FunctionKey key) : key(key) {}

        std::string to_string() const;
    };

    struct NilType : public std::monostate {
        using std::monostate::monostate;
    };
    friend fmt::formatter<NilType>;
    using Value = std::variant<NilType, std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t,
                               std::int32_t, std::int64_t, std::uint64_t, float, double, bool, std::string,
                               VariableReference, FunctionReference>;

   private:
    Value expr_result_;
    Value func_result_;

    using VariableNameType = SimpleFlyWeight::id_type;
    struct Variable {
        VariableNameType name_id;
        Value value;

        std::string to_string() const;
    };

    void declare_variable_(ast::SourceVariableIdentifier name, ast::SourceTypeIdentifier type,
                           std::optional<Value> value, location::SourceRegion location = {});

    using VariableMap = std::unordered_map<VariableKey, Variable>;
    VariableMap variables_;

    struct Scope {
        Scope* parent = nullptr;
        using KeyMapType = std::unordered_map<VariableNameType, VariableKey>;
        KeyMapType* keymap;
        Scope(Scope* parent, VariableMap* varmap)
            : parent(parent), keymap(InstancePool<KeyMapType>::aquire()), varmap_(varmap) {}

        ~Scope();

       private:
        VariableMap* varmap_;
    };

    Scope* current_scope_ = nullptr;
    Scope* global_scope_ = nullptr;

    using ArgType = std::vector<Value>;
    using KwArgType = std::unordered_map<VariableNameType, Value>;
    using Function = std::function<Value(const ArgType&, const KwArgType&)>;

    std::unordered_map<FunctionKey, Function> functions_;

    void register_function_(const std::string& name, Function func);

    /* builtin functions */
    Value print_(ArgType args, KwArgType kwargs);
    Value println_(ArgType args, KwArgType kwargs);

    void init_builtin_functions_();

    using TypeKey = SimpleFlyWeight::id_type;
    TypeKey encode_type_key_(std::string name) const;
    std::unordered_map<TypeKey, std::function<Value()>> types_;

    void gather_global_decls_();

    bool is_broken_ = false;
    bool is_returned_ = false;

    auto deref_and_apply_(auto func, auto& lhs, auto& rhs) {
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
    }

    auto deref_and_apply_func_() {
        return [this](auto&&... args) { deref_and_apply_(args...); };
    }

const Value& deref_(const Interpreter::Value& val) ;
const std::type_info& get_type_info(const Interpreter::Value& val) const ;

    void apply_ADD_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_SUB_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_MUL_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_DIV_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_MOD_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_ASSIGN_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_LESS_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_GREATER_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_LESS_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_GREATER_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_BOOL_AND_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_BIT_AND_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_BOOL_OR_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_BIT_OR_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_BIT_XOR_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_NOT_EQUAL_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_LEFT_SHIFT_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    void apply_RIGHT_SHIFT_(const Value& lhs, const Value& rhs, location::SourceRegion& location);
    
    // private:
// 全ての二項演算の実行パイプライン
template<typename Operation>
void execute_binary_operation_(
    const Value& lhs, 
    const Value& rhs, 
    Operation op, 
    const char* op_name, // エラーメッセージ用
    const location::SourceRegion& location
); 

   public:
    Interpreter();
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

    void debug_print() const;
};

}  // namespace interpreter
}  // namespace Garnet
template <>
struct fmt::formatter<Garnet::interpreter::Interpreter::NilType> : public fmt::formatter<std::string_view> {
    auto format(const Garnet::interpreter::Interpreter::NilType&, fmt::format_context& ctx) const {
        return fmt::formatter<std::string_view>::format("nil", ctx);
    }
};
#endif
