#ifndef GARNET_LIBS_DEFS_LOCATION
#define GARNET_LIBS_DEFS_LOCATION
#include <filesystem>
namespace Garnet::location {
struct SourcePosition {
    std::filesystem::path source_file = "";
    int line = -1;
    int column = -1;
};
struct SourceRegion {
    SourcePosition begin;
    SourcePosition end;
};
}  // namespace Garnet::location
#endif
