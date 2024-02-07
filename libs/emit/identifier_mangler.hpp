#ifndef WOMUYURO_LIBS_EMIT_IDENTIFIERMANGLER
#define WOMUYURO_LIBS_EMIT_IDENTIFIERMANGLER
#include "concrete_infos.hpp"
#include "concrete_source_identifiers.hpp"
namespace WomuYuro::emit {
class IndentifilerMangler {
   public:
    static std::string mangle_function_name(ast::SourceFunctionIdentifier name, ast::FunctionInfo signature);
};
}  // namespace WomuYuro::emit
#endif
