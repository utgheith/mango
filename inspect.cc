#include <mango/bits.h>
#include <mango/nat.h>

using namespace mango;

const Nat<12> add(const Nat<5> a, const Nat<7> b) { return a + b; }

auto add(const Bits<7> &a, const Bits<12> &b) { return a + b; }

auto add(const Bits<66> &a, const Bits<61> &b) { return a + b; }
