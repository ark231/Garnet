#ifndef WOMUYURO_MUDIG_CONVERTER
#define WOMUYURO_MUDIG_CONVERTER
#include <stdexcept>
#include <string>
namespace WomuYuro {
class NumberFormatError : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};
int64_t mudig_to_int(std::string mudig);
double mudig_to_float(std::string mudig);
}  // namespace WomuYuro
#endif
