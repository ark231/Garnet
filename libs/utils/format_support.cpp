#include "format_support.hpp"
namespace WomuYuro {
IndentLevel operator""_ind(unsigned long long value) { return {static_cast<int>(value)}; };

}  // namespace WomuYuro
