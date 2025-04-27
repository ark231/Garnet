#ifndef WOMUYURO_COMPILER_LIBS_UTILS_FORMAT_UTILS
#define WOMUYURO_COMPILER_LIBS_UTILS_FORMAT_UTILS
#include <fmt/core.h>

#include "format_support.hpp"
namespace WomuYuro {
template <int tabwidth = 4, typename... Args>
std::string format_with_indent(IndentLevel indent, fmt::format_string<Args...> format_str, Args&&... args) {
    return fmt::format("{:{}}", "", indent.level * tabwidth) + fmt::format(format_str, std::forward<Args>(args)...);
}
template <int tabwidth = 4, typename OutputIt, typename... Args>
void format_to_with_indent(IndentLevel indent, OutputIt out, fmt::format_string<Args...> format_str, Args&&... args) {
    fmt::format_to(out, "{:{}}", "", indent.level * tabwidth);
    fmt::format_to(out, format_str, std::forward<Args>(args)...);
}
template <int tabwidth = 4, typename... Args>
auto print_with_indent(IndentLevel indent, fmt::format_string<Args...> format_str, Args&&... args) {
    return fmt::print("{}", format_with_indent<tabwidth>(indent, format_str, std::forward<Args>(args)...));
}
template <int tabwidth = 4, typename... Args>
auto println_with_indent(IndentLevel indent, fmt::format_string<Args...> format_str, Args&&... args) {
    return fmt::println("{}", format_with_indent<tabwidth>(indent, format_str, std::forward<Args>(args)...));
}
}  // namespace WomuYuro
#endif
