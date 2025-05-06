#ifndef GARNET_LIBS_UTILS_FORMAT
#define GARNET_LIBS_UTILS_FORMAT
#include <fmt/format.h>

#include <concepts>
#include <utility>

#include "format_support.hpp"
namespace Garnet {
template <class T>
concept HasIndentableToString = requires(T v) {
    { v.to_string(std::declval<IndentLevel>()) } -> std::convertible_to<std::string>;
};
template <class T>
concept HasToString = requires(T v) {
    { v.to_string() } -> std::convertible_to<std::string>;
};

}  // namespace Garnet
template <class T>
    requires Garnet::HasToString<T> && (not Garnet::HasIndentableToString<T>)
struct fmt::formatter<T> : public fmt::formatter<std::string> {
    auto format(const T& v, fmt::format_context& ctx) const { return fmt::format_to(ctx.out(), "{}", v.to_string()); }
};
#endif
