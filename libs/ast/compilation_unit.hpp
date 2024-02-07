#ifndef WOMUYURO_LIB_AST_UNIT_BASE
#define WOMUYURO_LIB_AST_UNIT_BASE
#include <memory>
#include <vector>

#include "base.hpp"
#include "decl_base.hpp"
namespace WomuYuro::ast {
class CompilationUnit : public Base {
   public:
    virtual std::string to_string() const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    void add_decl(std::shared_ptr<DeclBase> decl);
    const std::vector<std::shared_ptr<DeclBase>>& decls() const;

   private:
    std::vector<std::shared_ptr<DeclBase>> decls_;
};
};  // namespace WomuYuro::ast
#endif
