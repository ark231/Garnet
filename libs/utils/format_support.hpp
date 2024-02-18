#ifndef WOMUYURO_COMPILER_LIBS_UTILS_FORMAT_SUPPORT
#define WOMUYURO_COMPILER_LIBS_UTILS_FORMAT_SUPPORT
#include <string>
namespace WomuYuro {
struct IndentLevel {
    int level;
};
class IFormattable {
   public:
    virtual std::string to_string() const = 0;
};
class IIndentedFormattable {
   public:
    virtual std::string to_string(IndentLevel level) const = 0;
};
inline namespace operators {
IndentLevel operator""_ind(unsigned long long value);
IndentLevel operator+(const IndentLevel& one, const int theother);
}  // namespace operators
}  // namespace WomuYuro
#endif
