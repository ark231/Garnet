#ifndef GARNET_COMPILER_LIBS_AST_STATEMENT
#define GARNET_COMPILER_LIBS_AST_STATEMENT
#include "sentence.hpp"
namespace Garnet::ast {
class Statement : public Sentence {
    using Sentence::Sentence;
};
}  // namespace Garnet::ast
#endif
