#ifndef GARNET_LIB_AST_BASE
#define GARNET_LIB_AST_BASE
#include <memory>
#include <vector>

#include "visitor/visitor.hpp"
namespace Garnet::ast {
class Base {
   public:
    virtual std::vector<std::shared_ptr<Base>> children() const = 0;
    virtual void accept(Visitor& visitor) const = 0;
};
}  // namespace Garnet::ast
#endif
