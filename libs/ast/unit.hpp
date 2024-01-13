#ifndef WOMUYURO_LIB_AST_UNIT_BASE
#define WOMUYURO_LIB_AST_UNIT_BASE
#include <memory>
#include <vector>

#include "base.hpp"
#include "decl_base.hpp"
namespace WomuYuro::Ast {
class Unit : public Base {
   public:
    virtual std::string to_string() override;

   private:
    std::vector<std::shared_ptr<DeclBase>> decls_;
};
};  // namespace WomuYuro::Ast
#endif
