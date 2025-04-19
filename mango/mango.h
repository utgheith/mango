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
  return (a < b) ? 0 : a - b;
}

template <typename T>
constexpr T max(T a, T b) {
  return (a > b) ? a : b;
}

}  // namespace mango
