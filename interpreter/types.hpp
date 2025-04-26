#ifndef WOMUYURO_INTERPRETER_TYPES
#define WOMUYURO_INTERPRETER_TYPES
#include <boost/operators.hpp>
#include <cmath>
#include <cstdint>
namespace WomuYuro {
class F64 : private boost::operators<F64> {
    double value_;

   public:
    F64() : F64(0) {}
    explicit F64(double value) : value_(value) {}

    bool operator<(const F64& x) const { return value_ < x.value_; }
    bool operator==(const F64& x) const { return value_ == x.value_; }
    F64& operator+=(const F64& x) {
        value_ += x.value_;
        return *this;
    }
    F64& operator-=(const F64& x) {
        value_ -= x.value_;
        return *this;
    }
    F64& operator*=(const F64& x) {
        value_ *= x.value_;
        return *this;
    }
    F64& operator/=(const F64& x) {
        value_ /= x.value_;
        return *this;
    }
    F64& operator%=(const F64& x) {
        value_ = std::fmod(value_, x.value_);
        return *this;
    }
    F64& operator++() {
        ++value_;
        return *this;
    }
    F64& operator--() {
        --value_;
        return *this;
    }
};
class F32 : private boost::operators<F32> {
    float value_;

   public:
    F32() : F32(0) {}
    explicit F32(float value) : value_(value) {}

    bool operator<(const F32& x) const { return value_ < x.value_; }
    bool operator==(const F32& x) const { return value_ == x.value_; }
    F32& operator+=(const F32& x) {
        value_ += x.value_;
        return *this;
    }
    F32& operator-=(const F32& x) {
        value_ -= x.value_;
        return *this;
    }
    F32& operator*=(const F32& x) {
        value_ *= x.value_;
        return *this;
    }
    F32& operator/=(const F32& x) {
        value_ /= x.value_;
        return *this;
    }
    F32& operator%=(const F32& x) {
        value_ = std::fmod(value_, x.value_);
        return *this;
    }
    F32& operator++() {
        ++value_;
        return *this;
    }
    F32& operator--() {
        --value_;
        return *this;
    }
};
class U8 : private boost::operators<U8> {
    uint8_t value_;

   public:
    U8() : U8(0) {}
    explicit U8(uint8_t value) : value_(value) {}

    bool operator<(const U8& x) const { return value_ < x.value_; }
    bool operator==(const U8& x) const { return value_ == x.value_; }
    U8& operator+=(const U8& x) {
        value_ += x.value_;
        return *this;
    }
    U8& operator-=(const U8& x) {
        value_ -= x.value_;
        return *this;
    }
    U8& operator*=(const U8& x) {
        value_ *= x.value_;
        return *this;
    }
    U8& operator/=(const U8& x) {
        value_ /= x.value_;
        return *this;
    }
    U8& operator%=(const U8& x) {
        value_ %= x.value_;
        return *this;
    }
    U8& operator|=(const U8& x) {
        value_ |= x.value_;
        return *this;
    }
    U8& operator&=(const U8& x) {
        value_ &= x.value_;
        return *this;
    }
    U8& operator^=(const U8& x) {
        value_ ^= x.value_;
        return *this;
    }
    U8& operator++() {
        ++value_;
        return *this;
    }
    U8& operator--() {
        --value_;
        return *this;
    }
};
class I8 : private boost::operators<I8> {
    int8_t value_;

   public:
    I8() : I8(0) {}
    explicit I8(int8_t value) : value_(value) {}

    bool operator<(const I8& x) const { return value_ < x.value_; }
    bool operator==(const I8& x) const { return value_ == x.value_; }
    I8& operator+=(const I8& x) {
        value_ += x.value_;
        return *this;
    }
    I8& operator-=(const I8& x) {
        value_ -= x.value_;
        return *this;
    }
    I8& operator*=(const I8& x) {
        value_ *= x.value_;
        return *this;
    }
    I8& operator/=(const I8& x) {
        value_ /= x.value_;
        return *this;
    }
    I8& operator%=(const I8& x) {
        value_ %= x.value_;
        return *this;
    }
    I8& operator|=(const I8& x) {
        value_ |= x.value_;
        return *this;
    }
    I8& operator&=(const I8& x) {
        value_ &= x.value_;
        return *this;
    }
    I8& operator^=(const I8& x) {
        value_ ^= x.value_;
        return *this;
    }
    I8& operator++() {
        ++value_;
        return *this;
    }
    I8& operator--() {
        --value_;
        return *this;
    }
};
class U16 : private boost::operators<U16> {
    uint16_t value_;

   public:
    U16() : U16(0) {}
    explicit U16(uint16_t value) : value_(value) {}

    bool operator<(const U16& x) const { return value_ < x.value_; }
    bool operator==(const U16& x) const { return value_ == x.value_; }
    U16& operator+=(const U16& x) {
        value_ += x.value_;
        return *this;
    }
    U16& operator-=(const U16& x) {
        value_ -= x.value_;
        return *this;
    }
    U16& operator*=(const U16& x) {
        value_ *= x.value_;
        return *this;
    }
    U16& operator/=(const U16& x) {
        value_ /= x.value_;
        return *this;
    }
    U16& operator%=(const U16& x) {
        value_ %= x.value_;
        return *this;
    }
    U16& operator|=(const U16& x) {
        value_ |= x.value_;
        return *this;
    }
    U16& operator&=(const U16& x) {
        value_ &= x.value_;
        return *this;
    }
    U16& operator^=(const U16& x) {
        value_ ^= x.value_;
        return *this;
    }
    U16& operator++() {
        ++value_;
        return *this;
    }
    U16& operator--() {
        --value_;
        return *this;
    }
};
class I16 : private boost::operators<I16> {
    int16_t value_;

   public:
    I16() : I16(0) {}
    explicit I16(int16_t value) : value_(value) {}

    bool operator<(const I16& x) const { return value_ < x.value_; }
    bool operator==(const I16& x) const { return value_ == x.value_; }
    I16& operator+=(const I16& x) {
        value_ += x.value_;
        return *this;
    }
    I16& operator-=(const I16& x) {
        value_ -= x.value_;
        return *this;
    }
    I16& operator*=(const I16& x) {
        value_ *= x.value_;
        return *this;
    }
    I16& operator/=(const I16& x) {
        value_ /= x.value_;
        return *this;
    }
    I16& operator%=(const I16& x) {
        value_ %= x.value_;
        return *this;
    }
    I16& operator|=(const I16& x) {
        value_ |= x.value_;
        return *this;
    }
    I16& operator&=(const I16& x) {
        value_ &= x.value_;
        return *this;
    }
    I16& operator^=(const I16& x) {
        value_ ^= x.value_;
        return *this;
    }
    I16& operator++() {
        ++value_;
        return *this;
    }
    I16& operator--() {
        --value_;
        return *this;
    }
};
class U32 : private boost::operators<U32> {
    uint32_t value_;

   public:
    U32() : U32(0) {}
    explicit U32(uint32_t value) : value_(value) {}

    bool operator<(const U32& x) const { return value_ < x.value_; }
    bool operator==(const U32& x) const { return value_ == x.value_; }
    U32& operator+=(const U32& x) {
        value_ += x.value_;
        return *this;
    }
    U32& operator-=(const U32& x) {
        value_ -= x.value_;
        return *this;
    }
    U32& operator*=(const U32& x) {
        value_ *= x.value_;
        return *this;
    }
    U32& operator/=(const U32& x) {
        value_ /= x.value_;
        return *this;
    }
    U32& operator%=(const U32& x) {
        value_ %= x.value_;
        return *this;
    }
    U32& operator|=(const U32& x) {
        value_ |= x.value_;
        return *this;
    }
    U32& operator&=(const U32& x) {
        value_ &= x.value_;
        return *this;
    }
    U32& operator^=(const U32& x) {
        value_ ^= x.value_;
        return *this;
    }
    U32& operator++() {
        ++value_;
        return *this;
    }
    U32& operator--() {
        --value_;
        return *this;
    }
};
class I32 : private boost::operators<I32> {
    int32_t value_;

   public:
    I32() : I32(0) {}
    explicit I32(int32_t value) : value_(value) {}

    bool operator<(const I32& x) const { return value_ < x.value_; }
    bool operator==(const I32& x) const { return value_ == x.value_; }
    I32& operator+=(const I32& x) {
        value_ += x.value_;
        return *this;
    }
    I32& operator-=(const I32& x) {
        value_ -= x.value_;
        return *this;
    }
    I32& operator*=(const I32& x) {
        value_ *= x.value_;
        return *this;
    }
    I32& operator/=(const I32& x) {
        value_ /= x.value_;
        return *this;
    }
    I32& operator%=(const I32& x) {
        value_ %= x.value_;
        return *this;
    }
    I32& operator|=(const I32& x) {
        value_ |= x.value_;
        return *this;
    }
    I32& operator&=(const I32& x) {
        value_ &= x.value_;
        return *this;
    }
    I32& operator^=(const I32& x) {
        value_ ^= x.value_;
        return *this;
    }
    I32& operator++() {
        ++value_;
        return *this;
    }
    I32& operator--() {
        --value_;
        return *this;
    }
};
class U64 : private boost::operators<U64> {
    uint64_t value_;

   public:
    U64() : U64(0) {}
    explicit U64(uint64_t value) : value_(value) {}

    bool operator<(const U64& x) const { return value_ < x.value_; }
    bool operator==(const U64& x) const { return value_ == x.value_; }
    U64& operator+=(const U64& x) {
        value_ += x.value_;
        return *this;
    }
    U64& operator-=(const U64& x) {
        value_ -= x.value_;
        return *this;
    }
    U64& operator*=(const U64& x) {
        value_ *= x.value_;
        return *this;
    }
    U64& operator/=(const U64& x) {
        value_ /= x.value_;
        return *this;
    }
    U64& operator%=(const U64& x) {
        value_ %= x.value_;
        return *this;
    }
    U64& operator|=(const U64& x) {
        value_ |= x.value_;
        return *this;
    }
    U64& operator&=(const U64& x) {
        value_ &= x.value_;
        return *this;
    }
    U64& operator^=(const U64& x) {
        value_ ^= x.value_;
        return *this;
    }
    U64& operator++() {
        ++value_;
        return *this;
    }
    U64& operator--() {
        --value_;
        return *this;
    }
};
class I64 : private boost::operators<I64> {
    int64_t value_;

   public:
    I64() : I64(0) {}
    explicit I64(int64_t value) : value_(value) {}

    bool operator<(const I64& x) const { return value_ < x.value_; }
    bool operator==(const I64& x) const { return value_ == x.value_; }
    I64& operator+=(const I64& x) {
        value_ += x.value_;
        return *this;
    }
    I64& operator-=(const I64& x) {
        value_ -= x.value_;
        return *this;
    }
    I64& operator*=(const I64& x) {
        value_ *= x.value_;
        return *this;
    }
    I64& operator/=(const I64& x) {
        value_ /= x.value_;
        return *this;
    }
    I64& operator%=(const I64& x) {
        value_ %= x.value_;
        return *this;
    }
    I64& operator|=(const I64& x) {
        value_ |= x.value_;
        return *this;
    }
    I64& operator&=(const I64& x) {
        value_ &= x.value_;
        return *this;
    }
    I64& operator^=(const I64& x) {
        value_ ^= x.value_;
        return *this;
    }
    I64& operator++() {
        ++value_;
        return *this;
    }
    I64& operator--() {
        --value_;
        return *this;
    }
};
}  // namespace WomuYuro
#endif
