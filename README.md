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


| Type                    | Structure | Value               | Range                 | Notes                                   |
| ------------------------|-----------|---------------------|-----------------------|-----------------------------------------|
| Nat<uint64_t ...Vs>     | Compile   | Compile S(n)        | 0 .. Inf              | limited by memory size at compile time  |
| Neg<uint64_t ...Vs>     | Compile   | Compile S(n)        | -Inf .. 0             | memory size at compile time             |
| Bits<N>                 | Compile   | Run S(N)            | 0 .. 2^N-1            | |         
| Int<Min, Max>           | Compile   | Run S(log(Max-Min)) | Min .. Max            | Min and Max are compile time values     |
| UnsignedInt<uint64_t N> | Compile   | Run S(N)            | 0 .. 2^N-1            | | 
| SignedInt<uint64_t N>   | Compile   | Run S(N)            | -(2^(N-1)) .. (2^N)-1 | |


Supported operations

Addition (+)

|               | Nat<...Rs>         | Neg<...Rs>         | Bits<0>      |Bits<M>            | Int<Mn2,Mx2>          |
|---------------|--------------------|--------------------|--------------|-------------------|-----------------------|
| Nat<...Vs>    | Nat<Vs... + Rs...> | Nat<Vs... - Rs...> | Nat<...Vs>   |                   |                       |
|               |                    | Neg<Rs... - Vs...> |              |                   |                       |
| Neg<...Vs>    | Neg<Vs... - Rs...> | Neg<Vs... + Rs...> | Neg<...Vs>   |                   |                       |
|               | Nat<Rs... - Vs...> |                    |              |                   |                       |
| Bits<0>       | Nat<...Rs>         | Neg<...Rs>         | Bits<M>      | Bits<M>           |                       |
| Bits<N>       |                    |                    | Bits<N>      | Bits<max(M,N)+1>  |                       |
| Int<Mn1,<Mx1> |                    |                    |              |                   | Int<Mn1+Mn2, Mx1+Mx2> |