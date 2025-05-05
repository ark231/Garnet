#ifndef GARNET_LIB_AST_BASE
#define GARNET_LIB_AST_BASE
#include <any>
#include <memory>
#include <vector>

#include "location.hpp"
#include "visitor/visitor.hpp"
namespace Garnet::ast {
class Base {
    location::SourceRegion location_;

   public:
    Base(location::SourceRegion location = location::SourceRegion()) : location_(location) {};
    virtual std::vector<std::shared_ptr<Base>> children() const = 0;
    virtual void accept(Visitor& visitor) const = 0;
    location::SourceRegion location() const { return location_; }
    virtual ~Base() = default;

    std::any userdata;
};
}  // namespace Garnet::ast
#endif
