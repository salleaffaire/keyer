#include <chrono>
#include <iomanip>
#include <iostream>

#include "keyer.hpp"
#include "profiler.hpp"

// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
// http://const.me/articles/simd/simd.pdf

#define _IMAGE_WIDTH 1980
#define _IMAGE_HEIGHT 1080
// #define _IMAGE_WIDTH 8
// #define _IMAGE_HEIGHT 1

#define OUTPUT_IMAGE 0

void fill4Pixels(uint8_t* bg, uint8_t* key, uint8_t* alpha) {
  uint8_t bgs[16] = {128, 129, 130, 131, 132, 133, 240, 135,
                     128, 128, 128, 128, 128, 128, 128, 128};
  uint8_t keys[16] = {40,  40,  40,  40,  40,  40,  40,  40,
                      255, 255, 255, 255, 255, 255, 255, 255};
  uint8_t alphas[16] = {10, 10, 10, 10, 10, 10, 10, 10,
                        50, 50, 50, 50, 50, 50, 50, 50};

  for (int i = 0; i < 16; i++) {
    bg[i] = bgs[i];
    key[i] = keys[i];
    alpha[i] = alphas[i];
  }
}

void outputImage(uint8_t* bg, int width, int height) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      std::cout << std::setw(3) << std::hex
                << (int)bg[(i * width * 4) + (4 * j) + 0] << " ";
      std::cout << std::setw(3) << std::hex
                << (int)bg[(i * width * 4) + (4 * j) + 1] << " ";
      std::cout << std::setw(3) << std::hex
                << (int)bg[(i * width * 4) + (4 * j) + 2] << " ";
      std::cout << std::setw(3) << std::hex
                << (int)bg[(i * width * 4) + (4 * j) + 3] << " ";
    }
    std::cout << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Setup data
  // Allocating more memory than needed to align the pointer to a 32 byte
  // boundary.
  // ------------------------------------------------------------------------
  uint8_t* bg = new uint8_t[_IMAGE_WIDTH * _IMAGE_HEIGHT * 4 + 32];
  uint8_t* aligned_bg = (uint8_t*)(((uintptr_t)bg + 31) & ~(uintptr_t)0x1F);

  uint8_t* key = new uint8_t[_IMAGE_WIDTH * _IMAGE_HEIGHT * 4 + 32];
  uint8_t* aligned_key = (uint8_t*)(((uintptr_t)key + 31) & ~(uintptr_t)0x1F);

  uint8_t* alpha = new uint8_t[_IMAGE_WIDTH * _IMAGE_WIDTH * 4 + 32];
  uint8_t* aligned_alpha =
      (uint8_t*)(((uintptr_t)alpha + 31) & ~(uintptr_t)0x1F);

  fill4Pixels(aligned_bg, aligned_key, aligned_alpha);
  fill4Pixels(aligned_bg + 16, aligned_key + 16, aligned_alpha + 16);

  uint8_t* bg_sse = new uint8_t[_IMAGE_WIDTH * _IMAGE_HEIGHT * 4 + 32];

  uint8_t* aligned_bg_sse =
      (uint8_t*)(((uintptr_t)bg_sse + 31) & ~(uintptr_t)0x1F);

  fill4Pixels(aligned_bg_sse, aligned_key, aligned_alpha);
  fill4Pixels(aligned_bg_sse + 16, aligned_key + 16, aligned_alpha + 16);

#if (OUTPUT_IMAGE == 1)
  std::cout << "Original image" << std::endl;
  outputImage(aligned_bg, _IMAGE_WIDTH, _IMAGE_HEIGHT);
#endif

  // Simple non intrinsic implementation
  // ------------------------------------------------------------------------
  // ------------------------------------------------------------------------
  uint64_t start = rdtsc();
  apply_key(aligned_bg, aligned_key, aligned_alpha, _IMAGE_WIDTH,
            _IMAGE_HEIGHT);
  uint64_t total = rdtsc() - start;

#if (OUTPUT_IMAGE == 1)
  std::cout << "Keyed image" << std::endl;
  outputImage(aligned_bg, _IMAGE_WIDTH, _IMAGE_HEIGHT);
#endif

  std::cout << "Cycles :: " << total << std::endl;

  double giga = 1000000000;
  double fr = 4.0;
  double sec_per_cycles = 1 / (giga * fr);
  double execution_time = sec_per_cycles * total;

  std::cout << sec_per_cycles << " seconds per cycles" << std::endl;
  std::cout << execution_time << " execution time" << std::endl;

  // Intrinsic implementation
  // ------------------------------------------------------------------------
  // ------------------------------------------------------------------------
  start = rdtsc();
  apply_key_avx(aligned_bg_sse, aligned_key, aligned_alpha, _IMAGE_WIDTH,
                _IMAGE_HEIGHT);
  total = rdtsc() - start;

#if (OUTPUT_IMAGE == 1)
  std::cout << "Keyed image" << std::endl;
  outputImage(aligned_bg_sse, _IMAGE_WIDTH, _IMAGE_HEIGHT);
#endif

  std::cout << "Cycles :: " << total << std::endl;

  giga = 1000000000;
  fr = 4.0;
  sec_per_cycles = 1 / (giga * fr);
  execution_time = sec_per_cycles * total;

  std::cout << sec_per_cycles << " seconds per cycles" << std::endl;
  std::cout << execution_time << " execution time" << std::endl;

  delete[] bg;
  delete[] key;
  delete[] alpha;

  delete[] bg_sse;

  return 0;
}
