#ifndef WOMUYURO_LIB_AST_BASE
#define WOMUYURO_LIB_AST_BASE
#include <memory>
#include <vector>

#include "../utils/format_support.hpp"
#include "visitor/visitor.hpp"
namespace WomuYuro::ast {
class Base : public IIndentedFormattable {
   public:
    virtual std::vector<std::shared_ptr<Base>> children() const = 0;
    virtual void accept(Visitor& visitor) const = 0;
};
}  // namespace WomuYuro::ast
#endif
