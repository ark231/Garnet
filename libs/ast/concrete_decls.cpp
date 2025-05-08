#include "concrete_decls.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include "../utils/format.hpp"
#include "enums.hpp"
namespace Garnet::ast {

VariableDecl::VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type,
                           std::optional<std::shared_ptr<Expression>> init, location::SourceRegion location)
    : DeclBase(location), name_(name), type_(type), init_(init) {}
std::vector<std::shared_ptr<Base>> VariableDecl::children() const { return {}; }
std::string VariableDecl::mangled_name() {
    return fmt::format("_V{}{}_T{}{}", name_.length(), name_, type_.length(), type_);
}

TypeDecl::TypeDecl(SourceTypeIdentifier name, location::SourceRegion location) : DeclBase(location), name_(name) {}
std::vector<std::shared_ptr<Base>> TypeDecl::children() const { return {}; }
std::string TypeDecl::mangled_name() { return fmt::format("_T{}{}", name_.length(), name_); }

}  // namespace Garnet::ast
