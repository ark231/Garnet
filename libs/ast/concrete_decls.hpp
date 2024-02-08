#ifndef WOMUYURO_LIBS_AST_CONCRETE_DECLS
#define WOMUYURO_LIBS_AST_CONCRETE_DECLS
#include <memory>
#include <vector>

#include "concrete_source_identifiers.hpp"
#include "decl_base.hpp"
namespace WomuYuro::ast {
class Sentence;
class FunctionDecl : public DeclBase {
   public:
    FunctionDecl();
    FunctionDecl(SourceFunctionIdentifier name, std::vector<SourceTypeIdentifier> args, SourceTypeIdentifier result);
    virtual std::string to_string(IndentLevel level) const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    void add_sentence(std::shared_ptr<Sentence> sentence);
    const std::vector<std::shared_ptr<Sentence>>& sentences();
    std::string mangled_name();

   protected:
    std::vector<std::shared_ptr<Sentence>> sentences_;
    SourceFunctionIdentifier name_;
    std::vector<SourceTypeIdentifier> args_;
    SourceTypeIdentifier result_;
};
class VariableDecl : public DeclBase {
   public:
    VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type);
    virtual std::string to_string(IndentLevel level) const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    std::string mangled_name();

   protected:
    SourceVariableIdentifier name_;
    SourceTypeIdentifier type_;
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
