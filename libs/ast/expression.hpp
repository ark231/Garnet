#ifndef GARNET_LIBS_AST_EXPRESSION
#define GARNET_LIBS_AST_EXPRESSION
#include "sentence.hpp"
namespace Garnet::ast {
class Expression : public Sentence {
    using Sentence::Sentence;
};
}  // namespace Garnet::ast
#endif
