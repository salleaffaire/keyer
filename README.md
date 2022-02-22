# Keyer implementation - Pure C vs. SIMD (SSE, AVX, AVX2)

## Compiling

```bash
g++ main.cpp -std=c++17 -mavx -mavx2 -O3 -o main
```

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

```c++
//  Original image
// |-----------------------|                       |-----------------------|                         <-- curBgGroup16Lo
//                         |-----------------------|                       |-----------------------| <-- curBgGroup16Hi
//  80 81 82 83 84 85 f0 87 80 80 80 80 80 80 80 80 1e 28 32 3c 42 00 40 4e 17 21 2c 37 42 4d 58 63
```
