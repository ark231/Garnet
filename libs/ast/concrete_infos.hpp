#ifndef WOMUYURO_LIBS_AST_CONCRETE_INFOS
#define WOMUYURO_LIBS_AST_CONCRETE_INFOS
#include <vector>

#include "concrete_source_identifiers.hpp"
#include "info_base.hpp"
namespace WomuYuro::ast {
class TypeInfo : public InfoBase {
   public:
    TypeInfo(SourceTypeIdentifier name);
};
class FunctionInfo : public InfoBase {
   public:
    FunctionInfo(SourceFunctionIdentifier name, std::vector<TypeInfo> args, TypeInfo result);
};

}  // namespace WomuYuro::ast
#endif
