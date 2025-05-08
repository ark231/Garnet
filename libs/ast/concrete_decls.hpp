#ifndef GARNET_LIBS_AST_CONCRETE_DECLS
#define GARNET_LIBS_AST_CONCRETE_DECLS
#include <memory>
#include <optional>
#include <vector>

#include "concrete_infos.hpp"
#include "concrete_source_identifiers.hpp"
#include "decl_base.hpp"
#include "expression.hpp"
namespace Garnet::ast {
class Sentence;
class VariableDecl : public DeclBase {
   public:
    VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type,
                 std::optional<std::shared_ptr<Expression>> init = std::nullopt, location::SourceRegion location = {});
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
    TypeDecl(SourceTypeIdentifier name, location::SourceRegion location = {});
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }
    SourceTypeIdentifier name() const { return name_; }

   protected:
    SourceTypeIdentifier name_;
};
}  // namespace Garnet::ast
#endif
