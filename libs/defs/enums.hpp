#ifndef WOMUYURO_ENUMS
#define WOMUYURO_ENUMS
namespace WomuYuro {
enum class ValRef { VALUE, REFERENCE };
enum class ConstMut { CONST, MUT };
enum class StatementType { FOR, WHILE, VARDECL, RETURN };
}  // namespace WomuYuro
#endif
