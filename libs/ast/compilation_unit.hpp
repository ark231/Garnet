#ifndef GARNET_LIB_AST_UNIT_BASE
#define GARNET_LIB_AST_UNIT_BASE
#include <memory>
#include <vector>

#include "base.hpp"
namespace Garnet::ast {
class CompilationUnit : public Base {
   public:
    using Base::Base;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    void add_child(std::shared_ptr<Base> decl);
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    std::vector<std::shared_ptr<Base>> children_;
};
};  // namespace Garnet::ast
#endif
