#ifndef GARNET_LIBS_AST_INFO_BASE
#define GARNET_LIBS_AST_INFO_BASE
#include "format_support.hpp"
#include "location.hpp"
namespace Garnet::ast {
class InfoBase : public IFormattable {
   protected:
    location::SourceRegion location_;

   public:
    InfoBase(location::SourceRegion location = {}) : location_(location) {}
    location::SourceRegion location() const { return location_; }
    void set_location(location::SourceRegion location) { this->location_ = location; }
};
}  // namespace Garnet::ast
#endif
