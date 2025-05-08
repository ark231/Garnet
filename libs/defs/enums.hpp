#ifndef GARNET_ENUMS
#define GARNET_ENUMS
#include "flyweight.hpp"
namespace Garnet {
enum class ValRef { VALUE, REFERENCE };
enum class ConstMut { CONST, MUT };
constexpr const char* RETURN_SPECIAL_VARNAME = "__return__";
const SimpleFlyWeight::id_type RETURN_SPECIAL_VARNAME_ID =  // NOLINT(readability-identifier-naming)
    SimpleFlyWeight::instance().id(RETURN_SPECIAL_VARNAME);
}  // namespace Garnet
#endif
