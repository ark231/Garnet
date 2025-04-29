#include "mudig_converter.hpp"

#include <fmt/format.h>
#include <unicode/errorcode.h>
#include <unicode/normalizer2.h>
#include <unicode/schriter.h>
#include <unicode/unistr.h>

#include <stack>
namespace Garnet {
constexpr std::string CONS = "KGSZTDNHMRPB";

int64_t mudig_to_int(std::string mudig) {
    auto u8_string = icu::UnicodeString::fromUTF8(mudig);
    icu::ErrorCode err;
    auto normalizer = icu::Normalizer2::getNFDInstance(err);
    if (err.isFailure()) {
        throw std::runtime_error(fmt::format("icu error: {}", err.errorName()));
    }
    err.reset();
    u8_string = normalizer->normalize(u8_string, err);
    if (err.isFailure()) {
        throw std::runtime_error(fmt::format("icu error: {}", err.errorName()));
    }
    int64_t result = 0;
    int tmp = 0;
    icu::StringCharacterIterator iter(u8_string);
    for (UChar32 ch = iter.first32(); ch != icu::StringCharacterIterator::DONE; ch = iter.next32()) {
        switch (ch) {
            case 0x0305:
                tmp *= -1;
                break;
            case 'K':
            case 'G':
            case 'S':
            case 'Z':
            case 'T':
            case 'D':
            case 'N':
            case 'H':
            case 'M':
            case 'R':
            case 'P':
            case 'B':
                result *= 12;
                result += tmp;
                tmp = CONS.find_first_of(ch);
                break;
            default:
                throw NumberFormatError(fmt::format("mudig string '{}' contains invalid character U+{:X}", mudig, ch));
        }
    }
    result *= 12;
    result += tmp;
    return result;
}
double mudig_to_float(std::string mudig) {
    auto u8_string = icu::UnicodeString::fromUTF8(mudig);
    icu::ErrorCode err;
    auto normalizer = icu::Normalizer2::getNFDInstance(err);
    if (err.isFailure()) {
        throw std::runtime_error(fmt::format("icu error: {}", err.errorName()));
    }
    err.reset();
    u8_string = normalizer->normalize(u8_string, err);
    if (err.isFailure()) {
        throw std::runtime_error(fmt::format("icu error: {}", err.errorName()));
    }
    int64_t result_int = 0;
    double result_frac = 0;
    std::stack<double> tmp;
    tmp.push(0);
    icu::StringCharacterIterator iter(u8_string);
    bool is_fraction = false;
    for (UChar32 ch = iter.first32(); ch != icu::StringCharacterIterator::DONE; ch = iter.next32()) {
        switch (ch) {
            case 0x0305:
                tmp.top() *= -1;
                break;
            case 0x0323:
                is_fraction = true;
                break;
            case 'K':
            case 'G':
            case 'S':
            case 'Z':
            case 'T':
            case 'D':
            case 'N':
            case 'H':
            case 'M':
            case 'R':
            case 'P':
            case 'B':
                if (not is_fraction) {
                    result_int *= 12;
                    result_int += tmp.top();
                    tmp.pop();
                }
                tmp.push(CONS.find_first_of(ch));
                break;
            default:
                throw NumberFormatError(fmt::format("mudig string '{}' contains invalid character U+{:X}", mudig, ch));
        }
    }
    if (is_fraction) {
        while (not tmp.empty()) {
            result_frac += tmp.top();
            result_frac /= 12.0;
            tmp.pop();
        }
    } else {
        result_int *= 12;
        result_int += tmp.top();
        tmp.pop();
    }
    return result_int + result_frac;
}
}  // namespace Garnet
