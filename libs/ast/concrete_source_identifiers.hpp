#ifndef GARNET_LIBS_AST_CONCRETE_SOURCE_IDENTIFIERS
#define GARNET_LIBS_AST_CONCRETE_SOURCE_IDENTIFIERS
#include "source_identifier_base.hpp"
namespace Garnet::ast {
class SourceTypeIdentifier : public SourceIdentifierBase {
   public:
    using SourceIdentifierBase::SourceIdentifierBase;
};
class SourceVariableIdentifier : public SourceIdentifierBase {
   public:
    using SourceIdentifierBase::SourceIdentifierBase;
};
class SourceFunctionIdentifier : public SourceIdentifierBase {
   public:
    using SourceIdentifierBase::SourceIdentifierBase;
};

}  // namespace Garnet::ast
#endif
