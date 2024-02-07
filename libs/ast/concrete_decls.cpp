#include "concrete_decls.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iterator>

#include "../utils/format.hpp"
#include "sentence.hpp"
namespace WomuYuro::ast {
// boost::uuidはto_string()を持ってる
FunctionDecl::FunctionDecl()
    : name_(fmt::format("_LF{}", boost::lexical_cast<std::string>(boost::uuids::random_generator()()))),
      args_({}),
      result_("__Auto") {}
FunctionDecl::FunctionDecl(SourceFunctionIdentifier name, std::vector<SourceTypeIdentifier> args,
                           SourceTypeIdentifier result)
    : name_(name), args_(args), result_(result) {}
std::string FunctionDecl::to_string() const {
    std::string result;
    fmt::format_to(std::back_inserter(result), "FunctionDecl '{}' {} -> {}\n", name_, args_, result_);
    for (const auto sentence : sentences_) {
        fmt::format_to(std::back_inserter(result), "    {}\n", sentence->to_string());
    }
    return result;
}
std::vector<std::shared_ptr<Base>> FunctionDecl::children() const {
    std::vector<std::shared_ptr<Base>> result;
    std::ranges::transform(sentences_, std::back_inserter(result),
                           [](auto& child) { return std::dynamic_pointer_cast<Base>(child); });
    return result;
}
void FunctionDecl::add_sentence(std::shared_ptr<Sentence> sentence) { sentences_.push_back(sentence); }
const std::vector<std::shared_ptr<Sentence>>& FunctionDecl::sentences() { return sentences_; }

VariableDecl::VariableDecl(SourceVariableIdentifier name, SourceTypeIdentifier type) : name_(name), type_(type) {}
std::string VariableDecl::to_string() const { return fmt::format("VariableDecl {}: {}", name_, type_); }
std::vector<std::shared_ptr<Base>> VariableDecl::children() const { return {}; }
std::string VariableDecl::mangled_name() {
    return fmt::format("_V{}{}_T{}{}", name_.length(), name_, type_.length(), type_);
}

TypeDecl::TypeDecl(SourceTypeIdentifier name) : name_(name) {}
std::string TypeDecl::to_string() const { return fmt::format("TypeDecl {}", name_); }
std::vector<std::shared_ptr<Base>> TypeDecl::children() const { return {}; }
std::string TypeDecl::mangled_name() { return fmt::format("_T{}{}", name_.length(), name_); }

}  // namespace WomuYuro::ast
