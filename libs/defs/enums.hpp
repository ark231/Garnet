#ifndef GARNET_ENUMS
#define GARNET_ENUMS
namespace Garnet {
enum class ValRef { VALUE, REFERENCE };
enum class ConstMut { CONST, MUT };
constexpr const char* RETURN_SPECIAL_VARNAME = "__return__";
}  // namespace Garnet
#endif
