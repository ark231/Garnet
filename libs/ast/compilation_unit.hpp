#ifndef WOMUYURO_LIB_AST_UNIT_BASE
#define WOMUYURO_LIB_AST_UNIT_BASE
#include <memory>
#include <vector>

#include "base.hpp"
namespace WomuYuro::ast {
class CompilationUnit : public Base {
   public:
    virtual std::string to_string(IndentLevel level) const override;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    void add_child(std::shared_ptr<Base> decl);

   private:
    std::vector<std::shared_ptr<Base>> children_;
};
};  // namespace WomuYuro::ast
#endif
