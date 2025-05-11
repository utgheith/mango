#pragma once

#include <cstdint>

#include "mango/bits.h"
#include "mango/nat.h"

namespace mango {

template <uint16_t N, typename MaskType, typename ValueType> struct MaskedBits {

  static constexpr MaskType mask{};
  static constexpr ValueType value{};
  const mango::Bits<N> bits;

  MaskedBits(const mango::Bits<N> &bs) : bits(bs) {}

  consteval static bool is_fixed() noexcept {
    return (MaskType{} + mango::Nat<1>{}) ==
           (mango::Nat<1>{} << mango::Nat<N>{});
  }

  template <uint16_t H, uint16_t L>
    requires((H < N) && (L <= H))
  constexpr MaskedBits<H - L + 1, decltype(mask.template extract<H, L>()),
                       decltype(value.template extract<H, L>())>
  extract() const noexcept {
    return {bits.template extract<H, L>()};
  }

  constexpr static auto fixed() noexcept {
    // static_assert(is_fixed());
    return value.template trim<N>();
  }

  template <typename Rhs> constexpr Cmp cmp(const Rhs &rhs) const noexcept {
    if constexpr (is_fixed()) {
      return value.template trim<N>().cmp(rhs);
    } else {
      return bits.cmp(rhs);
    }
  }

  template <typename Rhs>
  constexpr bool operator==(const Rhs &rhs) const noexcept {
    return this->cmp(rhs) == Cmp::EQ;
  }
};

} // namespace mango