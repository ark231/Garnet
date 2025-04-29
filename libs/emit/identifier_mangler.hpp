#ifndef GARNET_LIBS_EMIT_IDENTIFIERMANGLER
#define GARNET_LIBS_EMIT_IDENTIFIERMANGLER
#include "concrete_infos.hpp"
#include "concrete_source_identifiers.hpp"
namespace Garnet::emit {
class IndentifilerMangler {
   public:
    static std::string mangle_function_name(ast::SourceFunctionIdentifier name, ast::FunctionInfo signature);
};
}  // namespace Garnet::emit
#endif
