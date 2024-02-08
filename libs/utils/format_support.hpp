#ifndef WOMUYURO_COMPILER_LIBS_UTILS_FORMAT_SUPPORT
#define WOMUYURO_COMPILER_LIBS_UTILS_FORMAT_SUPPORT
namespace WomuYuro {
struct IndentLevel {
    int level;
};
inline namespace operators {
IndentLevel operator""_ind(unsigned long long value);
IndentLevel operator+(const IndentLevel& one, const int theother);
}  // namespace operators
}  // namespace WomuYuro
#endif
