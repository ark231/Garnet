#include "format_support.hpp"
namespace WomuYuro {
inline namespace operators {
IndentLevel operator""_ind(unsigned long long value) { return {static_cast<int>(value)}; };
IndentLevel operator+(const IndentLevel& one, const int theother) { return {one.level + theother}; }
}  // namespace operators
}  // namespace WomuYuro
