#include "concrete_infos.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <magic_enum_format.hpp>

#include "../utils/format.hpp"
namespace Garnet::ast {
VariableInfo::VariableInfo() : VariableInfo({"__Unspecified__"}, {{"__Unspecified__"}}) {}
VariableInfo::VariableInfo(SourceVariableIdentifier name, TypeInfo type, ValRef valref, bool is_const)
    : name_(name), type_(type), valref_(valref), is_const_(is_const) {}
std::string VariableInfo::to_string() const {
    return fmt::format("VariableInfo<{}: {} {} {}>", name_, type_, valref_, is_const_ ? "const" : "mut");
}
std::string TypeInfo::to_string() const { return fmt::format("TypeInfo<{}>", name_); }
TypeInfo::TypeInfo(SourceTypeIdentifier name) : name_(name) {}
std::string FunctionInfo::to_string() const {
    return fmt::format("FunctionInfo<{}: [({}) → ({})]>", name_, args_,
                       result_.has_value() ? result_->to_string() : "void");
}
FunctionInfo::FunctionInfo(SourceFunctionIdentifier name, const std::vector<VariableInfo>& args,
                           std::optional<VariableInfo> result)
    : name_(name), args_(args), result_(result) {}
}  // namespace Garnet::ast
