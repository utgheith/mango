![CI](https://github.com/utgheith/mango/actions/workflows/ci.yml/badge.svg)

Arbitrary precision ints.

`Nat<...>` compile-time natural numbers (non-negative)

`Neg<...>` compile-time non-positive numbers

`Bits<N>` n-bit bit vector

`Int<Min, Max>` integer in the inclusive, compile-time range Min .. Max

Operations aspire to be natural, efficient, and loesless:
* natural => support common arithmetic and logic operations (+, -, >>, == , !=, <=, ...>)
* efficient => use the minimum number of required bits as runtime
* lossless => no overflow (types grow in order to accomodate the worst case).