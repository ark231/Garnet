#ifndef GARNET_LIBS_DEFS_LOCATION
#define GARNET_LIBS_DEFS_LOCATION
// #include <filesystem>
#include <string>

#include "flyweight.hpp"
namespace Garnet::location {
struct SourcePosition {
    using PathType = std::string;
    // std::filesystem::path source_file = "";
    int line;
    int column;
    SourcePosition() : SourcePosition("", -1, -1) {}
    SourcePosition(const PathType &source_file, int line, int column) : line(line), column(column) {
        source_file_id_ = SimpleFlyWeight::instance().id(source_file);
    }
    PathType source_file() const { return SimpleFlyWeight::instance().value(source_file_id_); }

   private:
    SimpleFlyWeight::id_type source_file_id_;
};
struct SourceRegion {
    SourcePosition begin;
    SourcePosition end;
};
}  // namespace Garnet::location
#endif
