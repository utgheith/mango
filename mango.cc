#include "mango/mango.h"

#include <iostream>

#include "mango/int.h"
#include "mango/ranged_int.h"

using namespace mango;

constexpr Nat<3> x{5};
constexpr Nat<64> y{77};
constexpr Nat<0> zero;
constexpr Nat<1000> big{x};

template <uint16_t N>
void show(const char* msg, Nat<N> const& v) {
  std::cout << "[" << msg << "] ";
  std::cout << "Nat<" << N << ">{" << v << "}";
  std::cout << std::endl;
}

template <uint16_t N>
void show(const char* msg, Int<N> const& v) {
  std::cout << "[" << msg << "] ";
  std::cout << "Int<" << N << ">{" << v << "}";
  std::cout << std::endl;
}

void show(const char* msg, const bool v) {
  std::cout << "[" << msg << "] " << v << std::endl;
}

template <uint16_t A, Int<A> MIN, uint16_t B, Int<B> MAX>
void show(const char* msg, const RangedInt<A, MIN, B, MAX>& v) {
  std::cout << "[" << msg << "] " << v << std::endl;
}

#define SHOW(x) show(#x, x)

int main() {
  printf("hello\n");
  SHOW(x);
  SHOW(y);
  SHOW(zero);
  SHOW(big);
  SHOW(Nat<500>{x});
  SHOW(big.succ());
  SHOW(x + y);
  SHOW(~Nat<100>{});
  SHOW(Int(x));
  SHOW(-Int(x));
  SHOW(x == y);
  SHOW(x == big);


  SHOW(unsignedInt(x));
  SHOW(unsignedInt(x)+unsignedInt(y));

  // std::cout << zero + x << std::endl;
  // std::cout << zero + x << std::endl;
  // std::cout << x + y << " " << (x + y).WIDTH << std::endl;

  return 0;
}
