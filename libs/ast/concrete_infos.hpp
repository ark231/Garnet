#ifndef GARNET_LIBS_AST_CONCRETE_INFOS
#define GARNET_LIBS_AST_CONCRETE_INFOS
#include <optional>
#include <vector>

#include "concrete_source_identifiers.hpp"
#include "enums.hpp"
#include "info_base.hpp"
namespace Garnet::ast {
class TypeInfo : public InfoBase {
   public:
    TypeInfo(SourceTypeIdentifier name);
    SourceTypeIdentifier name() const { return name_; };
    virtual std::string to_string() const override;

   private:
    SourceTypeIdentifier name_;
};
class VariableInfo : public InfoBase {
   public:
    VariableInfo();
    VariableInfo(SourceVariableIdentifier name, TypeInfo type, ValRef valref = ValRef::VALUE, bool is_const = false);
    virtual std::string to_string() const override;
    SourceVariableIdentifier name() const { return name_; }
    TypeInfo type() const { return type_; }
    ValRef valref() const { return valref_; }
    bool is_const() const { return is_const_; }

   private:
    SourceVariableIdentifier name_;
    TypeInfo type_;
    ValRef valref_;
    bool is_const_;
};
class FunctionInfo : public InfoBase {
   public:
    FunctionInfo();
    FunctionInfo(SourceFunctionIdentifier name, const std::vector<VariableInfo>& args,
                 std::optional<VariableInfo> result);
    virtual std::string to_string() const override;
    SourceFunctionIdentifier name() const { return name_; }
    std::vector<VariableInfo> args() const { return args_; }
    std::optional<VariableInfo> result() const { return result_; }
    std::string result_to_string() const { return result_.has_value() ? result_->to_string() : "void"; };

   private:
    SourceFunctionIdentifier name_;
    std::vector<VariableInfo> args_;
    std::optional<VariableInfo> result_;
};

}  // namespace Garnet::ast
#endif
