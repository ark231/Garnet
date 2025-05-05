#ifndef GARNET_INTERPRETER_INTERPRETER
#define GARNET_INTERPRETER_INTERPRETER
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "format_support.hpp"
#include "visitor/visitor.hpp"
namespace Garnet::interpreter {
class Interpreter : public ast::Visitor {
    using VariableKey = boost::uuids::uuid;
    boost::uuids::random_generator key_generator_;
    struct VariableReference {
        VariableKey key;
        explicit VariableReference() = default;
        explicit VariableReference(VariableKey key) : key(key) {}

        std::string to_string() const;
    };

    using FunctionKey = std::string;
    FunctionKey encode_function_key_(std::string name) const;
    struct FunctionReference {
        FunctionKey key;
        explicit FunctionReference() = default;
        explicit FunctionReference(FunctionKey key) : key(key) {}

        std::string to_string() const;
    };

    using Value = std::variant<std::monostate, std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t,
                               std::int32_t, std::int64_t, std::uint64_t, float, double, bool, std::string,
                               VariableReference, FunctionReference>;

    Value expr_result_;
    IndentLevel indent_ = 0_ind;

    struct Variable {
        std::string name;
        Value value;

        std::string to_string() const;
    };

    using VariableMap = std::unordered_map<VariableKey, Variable>;
    VariableMap variables_;

    struct Scope {
        Scope* parent = nullptr;

        std::unordered_map<std::string, VariableKey> keymap;

       private:
    };

    Scope* current_scope_ = nullptr;

    using Function = std::function<Value(std::vector<Value>, std::unordered_map<std::string, Value>)>;

    std::unordered_map<FunctionKey, Function> functions_;

    using TypeKey = std::string;
    TypeKey encode_type_key_(std::string name) const;
    std::unordered_map<TypeKey, std::function<Value()>> types_;

    void gather_global_decls_();

   public:
    Interpreter();
    virtual void visit(const ast::FunctionDecl*) override;
    virtual void visit(const ast::VariableDecl*) override;
    virtual void visit(const ast::TypeDecl*) override;
    virtual void visit(const ast::ErrorNode*) override;
    virtual void visit(const ast::ErrorSentence*) override;
    virtual void visit(const ast::ErrorExpression*) override;
    virtual void visit(const ast::ErrorStatement*) override;
    virtual void visit(const ast::BinaryOperator*) override;
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

    void debug_print() const;
};

}  // namespace Garnet::interpreter
#endif
