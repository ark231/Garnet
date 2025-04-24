#ifndef WOMUYURO_LIBS_AST_CONCRETE_DECLS
#define WOMUYURO_LIBS_AST_CONCRETE_DECLS
#include <memory>
#include <optional>
#include <vector>

#include "concrete_infos.hpp"
#include "concrete_source_identifiers.hpp"
#include "decl_base.hpp"
#include "expression.hpp"
namespace WomuYuro::ast {
class Sentence;
class FunctionDecl : public DeclBase {
   public:
    FunctionDecl();
    FunctionDecl(SourceFunctionIdentifier name, std::vector<VariableInfo> args, std::optional<VariableInfo> result);
    virtual std::string to_string(IndentLevel level) const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();
    FunctionInfo info() const;

   protected:
    SourceFunctionIdentifier name_;
    std::vector<VariableInfo> args_;
    std::optional<VariableInfo> result_;
};
class VariableDecl : public DeclBase {
   public:
    VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type,
                 std::optional<std::shared_ptr<Expression>> init = std::nullopt);
    virtual std::string to_string(IndentLevel level) const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();

   protected:
    SourceVariableIdentifier name_;
    SourceTypeIdentifier type_;
    std::optional<std::shared_ptr<Expression>> init_;
};
class TypeDecl : public DeclBase {
   public:
    TypeDecl(SourceTypeIdentifier name);
    virtual std::string to_string(IndentLevel level) const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();

   protected:
    SourceTypeIdentifier name_;
};
}  // namespace WomuYuro::ast
#endif
