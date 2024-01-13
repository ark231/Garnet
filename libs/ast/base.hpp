#ifndef WOMUYURO_LIB_AST_BASE
#define WOMUYURO_LIB_AST_BASE
#include <string>
namespace WomuYuro::Ast {
class Base {
   public:
    virtual std::string to_string() = 0;
};
}  // namespace WomuYuro::Ast
#endif
