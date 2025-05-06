#ifndef GARNET_LIBS_AST_SOURCE_TYPE
#define GARNET_LIBS_AST_SOURCE_TYPE
#include <string>
namespace Garnet::ast {
class SourceIdentifierBase {
   public:
    SourceIdentifierBase() : SourceIdentifierBase("__unspecified__") {}
    SourceIdentifierBase(std::string name) : name_(name) {}
    const std::string source_name() const { return name_; }
    const std::string to_string() const { return name_; }
    size_t length() const { return name_.length(); }

   protected:
    std::string name_;
};

inline namespace operators {

inline bool operator==(const SourceIdentifierBase& one, const SourceIdentifierBase& theother) {
    return one.source_name() == theother.source_name();
}
}  // namespace operators

}  // namespace Garnet::ast
#endif
