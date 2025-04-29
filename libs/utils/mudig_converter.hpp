#ifndef GARNET_MUDIG_CONVERTER
#define GARNET_MUDIG_CONVERTER
#include <stdexcept>
#include <string>
namespace Garnet {
class NumberFormatError : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};
int64_t mudig_to_int(std::string mudig);
double mudig_to_float(std::string mudig);
}  // namespace Garnet
#endif
