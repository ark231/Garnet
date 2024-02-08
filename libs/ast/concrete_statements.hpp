#ifndef WOMUYURO_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#define WOMUYURO_COMPILER_LIBS_AST_CONCRETE_STATEMENTS
#include <memory>
#include <vector>

#include "concrete_decls.hpp"
#include "statement.hpp"

namespace WomuYuro::ast {
class VariableDeclStatement : public Statement {
   public:
    VariableDeclStatement(std::shared_ptr<VariableDecl> decl) : decl_(decl) {}
    virtual std::string to_string(IndentLevel level) const override;
    virtual StatementType type() const override { return StatementType::VARDECL; };
    virtual std::vector<std::shared_ptr<Base>> children() const override;

   protected:
    std::shared_ptr<VariableDecl> decl_;
};
}  // namespace WomuYuro::ast
#endif
