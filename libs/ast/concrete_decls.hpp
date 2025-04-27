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
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();
    FunctionInfo info() const;
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   protected:
    SourceFunctionIdentifier name_;
    std::vector<VariableInfo> args_;
    std::optional<VariableInfo> result_;
};
class VariableDecl : public DeclBase {
   public:
    VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type,
                 std::optional<std::shared_ptr<Expression>> init = std::nullopt);
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();
    SourceVariableIdentifier name() const { return name_; }
    SourceTypeIdentifier type() const { return type_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
    std::optional<std::shared_ptr<Expression>> init() const { return init_; };

   protected:
    SourceVariableIdentifier name_;
    SourceTypeIdentifier type_;
    std::optional<std::shared_ptr<Expression>> init_;
};
class TypeDecl : public DeclBase {
   public:
    TypeDecl(SourceTypeIdentifier name);
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
    SourceTypeIdentifier name() const { return name_; }

   protected:
    SourceTypeIdentifier name_;
};
}  // namespace WomuYuro::ast
#endif
