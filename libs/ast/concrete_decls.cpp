#include "concrete_decls.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../utils/format.hpp"
#include "enums.hpp"
namespace WomuYuro::ast {
// boost::uuidはto_string()を持ってる
FunctionDecl::FunctionDecl()
    : name_(fmt::format("_LF{}", boost::lexical_cast<std::string>(boost::uuids::random_generator()()))),
      args_({}),
      result_({{"__Auto"}, {{"__Auto"}}, ValRef::VALUE, false}) {}
FunctionDecl::FunctionDecl(SourceFunctionIdentifier name, std::vector<VariableInfo> args,
                           std::optional<VariableInfo> result)
    : name_(name), args_(args), result_(result) {}
std::vector<std::shared_ptr<Base>> FunctionDecl::children() const { return {}; }

FunctionInfo FunctionDecl::info() const { return {name_, args_, result_}; }

VariableDecl::VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type,
                           std::optional<std::shared_ptr<Expression>> init)
    : name_(name), type_(type), init_(init) {}
std::vector<std::shared_ptr<Base>> VariableDecl::children() const { return {}; }
std::string VariableDecl::mangled_name() {
    return fmt::format("_V{}{}_T{}{}", name_.length(), name_, type_.length(), type_);
}

TypeDecl::TypeDecl(SourceTypeIdentifier name) : name_(name) {}
std::vector<std::shared_ptr<Base>> TypeDecl::children() const { return {}; }
std::string TypeDecl::mangled_name() { return fmt::format("_T{}{}", name_.length(), name_); }

}  // namespace WomuYuro::ast
