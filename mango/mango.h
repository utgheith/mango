#pragma once

#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

namespace mango {

typedef unsigned int uint128_t __attribute__((mode(TI)));

constexpr uint16_t safe_sub(uint16_t a, uint16_t b) {
  return (a < b) ? uint16_t(0) : uint16_t(a - b);
}

consteval uint16_t clz(uint64_t a) {
#ifdef __clang__
  // __builtin_clzll is not constexpr in clang :-(
  uint16_t count = 0;
  if (a == 0)
    return 64;

  if ((a & 0xFFFFFFFF00000000) == 0) {
    count += 32;
    a <<= 32;
  }
  if ((a & 0xFFFF000000000000) == 0) {
    count += 16;
    a <<= 16;
  }
  if ((a & 0xFF00000000000000) == 0) {
    count += 8;
    a <<= 8;
  }
  if ((a & 0xF000000000000000) == 0) {
    count += 4;
    a <<= 4;
  }
  if ((a & 0xC000000000000000) == 0) {
    count += 2;
    a <<= 2;
  }
  if ((a & 0x8000000000000000) == 0) {
    count += 1;
  }
  return count;
#else
  return __builtin_clzll(a);
#endif
}

template <typename T> constexpr T max(T a, T b) { return (a > b) ? a : b; }

enum struct Cmp { LT = -1, EQ = 0, GT = 1 };

constexpr Cmp inverse(const Cmp c) noexcept { return Cmp(-int(c)); }

template <typename T1, typename T2>
  requires std::is_integral_v<T1> && std::is_integral_v<T2>
constexpr Cmp cmp(const T1 v1, const T2 v2) {
  if constexpr (sizeof(T1) == sizeof(T2)) {
    if constexpr (std::is_signed<T1>() == std::is_signed<T2>()) {
      if (v1 < v2) {
        return Cmp::LT;
      } else if (v1 > v2) {
        return Cmp::GT;
      } else {
        return Cmp::EQ;
      }
    } else {
      if constexpr (std::is_signed<T1>()) {
        if ((v1 < 0) || (T2(v1) < v2)) {
          return Cmp::LT;
        } else if (T2(v1) == v2) {
          return Cmp::EQ;
        } else {
          return Cmp::GT;
        }
      } else {
        return inverse(cmp(v2, v1));
      }
    }
  }
}

} // namespace mango
