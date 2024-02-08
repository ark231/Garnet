#ifndef WOMUYURO_LIBS_UTILS_FORMAT
#define WOMUYURO_LIBS_UTILS_FORMAT
#include <fmt/format.h>

#include <concepts>
#include <utility>
#include <vector>

#include "format_support.hpp"
namespace WomuYuro {
template <class T>
concept HasIndentableToString = requires(T v) {
    { v.to_string(std::declval<IndentLevel>()) } -> std::convertible_to<std::string>;
};
template <class T>
concept HasToString = requires(T v) {
    { v.to_string() } -> std::convertible_to<std::string>;
};

}  // namespace WomuYuro
template <class T>
    requires WomuYuro::HasToString<T> && (not WomuYuro::HasIndentableToString<T>)
struct fmt::formatter<T> : public fmt::formatter<std::string> {
    auto format(const T& v, fmt::format_context& ctx) const {
        return fmt::formatter<std::string>::format(v.to_string(), ctx);
    }
};
#endif
