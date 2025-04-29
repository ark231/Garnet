#ifndef GARNET_COMPILER_LIBS_UTILS_FORMAT_SUPPORT
#define GARNET_COMPILER_LIBS_UTILS_FORMAT_SUPPORT
#include <string>
namespace Garnet {
struct IndentLevel {
    int level;
};
class IFormattable {
   public:
    virtual std::string to_string() const = 0;
    virtual ~IFormattable() = default;
};
class IIndentedFormattable {
   public:
    virtual std::string to_string(IndentLevel level) const = 0;
    virtual ~IIndentedFormattable() = default;
};
inline namespace operators {
IndentLevel operator""_ind(unsigned long long value);
IndentLevel operator+(const IndentLevel& one, const int theother);
}  // namespace operators
}  // namespace Garnet
#endif
