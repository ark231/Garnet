#ifndef WOMUYURO_COMPILER_LIBS_AST_STATEMENT
#define WOMUYURO_COMPILER_LIBS_AST_STATEMENT
#include "enums.hpp"
#include "sentence.hpp"
namespace WomuYuro::ast {
class Statement : public Sentence {
   public:
    virtual StatementType type() const = 0;

   protected:
};
}  // namespace WomuYuro::ast
#endif
