# Keyer implementation - Pure C vs. SIMD (SSE, AVX, AVX2)

## Compiling

Compile with the simple command,

```bash
g++ main.cpp -std=c++17 -mavx -mavx2 -O3 -o main
```

## Git submodules

This project has a dependency on [STB](https://github.com/nothings/stb) that we manage with git submodules.

Created by,

```bash
git submodule add https://github.com/nothings/stb.git
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

##

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
