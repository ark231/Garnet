#ifndef GARNET_COMPILER_LIBS_AST_CONCRETE_DEFS
#define GARNET_COMPILER_LIBS_AST_CONCRETE_DEFS
#include <memory>
#include <vector>

#include "block.hpp"
#include "concrete_infos.hpp"
#include "def_base.hpp"
namespace Garnet::ast {
class FunctionDef : public DefBase {
   public:
    FunctionDef(FunctionInfo info, location::SourceRegion location = {}) : DefBase(location), info_(info) {}
    FunctionDef(FunctionInfo info, std::shared_ptr<Block> block, location::SourceRegion location = {})
        : DefBase(location), info_(info), block_(block) {}
    virtual std::vector<std::shared_ptr<Base>> children() const override;
    const FunctionInfo& info() const { return info_; }
    std::shared_ptr<Block> block() const { return block_; }
    virtual void accept(Visitor& visitor) const override { visitor.visit(this); }

   private:
    FunctionInfo info_;
    std::shared_ptr<Block> block_;
};
}  // namespace Garnet::ast
#endif
