#include "concrete_defs.hpp"

#include <bits/ranges_algobase.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <algorithm>
#include <iterator>
#include <memory>

namespace Garnet::ast {
std::vector<std::shared_ptr<Base>> FunctionDef::children() const { return {std::static_pointer_cast<Base>(block_)}; }
}  // namespace Garnet::ast
