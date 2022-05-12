# Keyer implementation - Pure C vs. SIMD (SSE, AVX, AVX2)

Intrinsics [documentation](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html) is at that link.

## Git submodules

This project has a dependency on [STB](https://github.com/nothings/stb) that we manage with git submodules.

Created by,

```bash
git submodule add https://github.com/nothings/stb.git
```

Clone with submodules,

```bash
git clone git@github.com:salleaffaire/keyer.git
git submodule init
git submodule update
```

## Compiling

Compile with the simple command,

```bash
g++ main.cpp -std=c++17 -mavx -mavx2 -O3 -o main
```

## Switching between g++/gcc versions on Ubuntu 20.04

Because the Cuda C compiler only supports g++ 8.4 or lower and 20.04 comes with 9.3.0 by default,
this trick below is useful to go down a version while keeping your current version installed.

[Handling multiple compiler versions](https://www.fosslinux.com/39386/how-to-install-multiple-versions-of-gcc-and-g-on-ubuntu-20-04.htm)

Install multiple

```bash
sudo apt install build-essential
sudo apt -y install gcc-7 g++-7 gcc-8 g++-8 gcc-9 g++-9
```

Register versions

```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 7
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 8
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 9
```

Switch

```bash
sudo update-alternatives --config gcc
sudo update-alternatives --config g++
```

## Misc

SWAP hi/low 128-bit

```c++
x = _mm256_castps_si256(
       _mm256_permute2f128_ps(
         _mm256_castsi256_ps(x), 
         _mm256_castsi256_ps(x), 
         1)
    );
```

## AVX Implementation

From `debug.cpp`, `bgdata`:

```c++
//   
//  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
// 
// |-----------------------|                       |-----------------------|                         <-- curBgGroup16Lo
//                         |-----------------------|                       |-----------------------| <-- curBgGroup16Hi
//  00 01 02 FF 10 11 12 FF 20 21 22 FF 30 31 32 FF 40 41 42 FF 50 51 52 FF 60 61 62 FF 70 71 72 FF  <-- curBgGroup
//  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  <-- zero

// 00 00 01 00 ...                                                                                   <-- curBgGroup16Lo
// 20 00 21 00 ...                                                                                   <-- curBgGroup16Hi 

```

## Divide by 255

For any integer number in the range of [0, 65536], there is a faster way to calculate x/255:

```c++
#define div_255_fast(x)    (((x) + (((x) + 257) >> 8)) >> 8)
```

It is twice as faster as x/255:

[Quick C++ Benchmark](http://quick-bench.com/t3Y2-b4isYIwnKwMaPQi3n9dmtQ)

And the SIMD version:

```c++
// (x + ((x + 257) >> 8)) >> 8
static inline __m128i _mm_fast_div_255_epu16(__m128i x) {
    return _mm_srli_epi16(_mm_adds_epu16(x, 
        _mm_srli_epi16(_mm_adds_epu16(x, _mm_set1_epi16(0x0101)), 8)), 8);
}
```
