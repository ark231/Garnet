#ifndef GARNET_LIBS_AST_SOURCE_TYPE
#define GARNET_LIBS_AST_SOURCE_TYPE
#include <string>

#include "flyweight.hpp"
namespace Garnet::ast {
class SourceIdentifierBase {
   public:
    SourceIdentifierBase() : SourceIdentifierBase("__unspecified__") {}
    SourceIdentifierBase(std::string name) : name_id_(SimpleFlyWeight::instance().id(name)) {}
    const std::string source_name() const { return SimpleFlyWeight::instance().value(name_id_); }
    SimpleFlyWeight::id_type source_id() const { return name_id_; }
    const std::string to_string() const { return source_name(); }
    size_t length() const { return source_name().length(); }

   protected:
    // std::string name_;
    SimpleFlyWeight::id_type name_id_;
};

inline namespace operators {

inline bool operator==(const SourceIdentifierBase& one, const SourceIdentifierBase& theother) {
    return one.source_name() == theother.source_name();
}
}  // namespace operators

}  // namespace Garnet::ast
#endif
