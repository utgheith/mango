#pragma once

#include <cstdint>

template <uint16_t N, typename MaskType, typename ValueType>
struct MaskedBits {

    static constexpr MaskType mask{};
    static constexpr ValueType value{};
    const mango::Bits<N> bits;

    MaskedBits(const mango::Bits<N>& bs) : bits(bs) {}

    template <uint16_t H, uint16_t L>
    requires ((H < N) && (L <= H))
    constexpr MaskedBits<H-L+1, decltype(mask.template extract<H,L>()), decltype(value.template extract<H,L>())> extract() const noexcept {
        return {bits.template extract<H,L>()};
    }

    

};