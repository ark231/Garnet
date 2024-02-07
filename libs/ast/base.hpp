#ifndef WOMUYURO_LIB_AST_BASE
#define WOMUYURO_LIB_AST_BASE
#include <memory>
#include <string>
#include <vector>
namespace WomuYuro::ast {
class Base {
   public:
    virtual std::string to_string() const = 0;
    virtual std::vector<std::shared_ptr<Base>> children() const = 0;
};
}  // namespace WomuYuro::ast
#endif
