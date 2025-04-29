#ifndef GARNET_COMPILER_LIBS_AST_CONCRETE_BODIES
#define GARNET_COMPILER_LIBS_AST_CONCRETE_BODIES
#include <memory>
#include <vector>

#include "base.hpp"
#include "body_base.hpp"
#include "format_support.hpp"
namespace Garnet::ast {
class FunctionBody : public BodyBase {
   public:
    virtual std::string to_string(IndentLevel level) const;
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    virtual void accept(Visitor& visitor) const override;
};
}  // namespace Garnet::ast
#endif
