#include <chrono>
#include <iomanip>
#include <iostream>

#include "aligned.hpp"
#include "keyer.hpp"
#include "profiler.hpp"

// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
// http://const.me/articles/simd/simd.pdf

#define _IMAGE_WIDTH 1980
#define _IMAGE_HEIGHT 1080
// #define _IMAGE_WIDTH 8
// #define _IMAGE_HEIGHT 1

#define OUTPUT_IMAGE 0

// This is for the purpose of testing
// 8 pixels of RGBX - 2 loops with SSE2 and AXV_2
//                  - 1 loop with AXV
void fill8Pixels(uint8_t *bg, uint8_t *key, uint8_t *alpha) {
  uint8_t bgs[32] = {128, 129, 130, 131, 132, 133, 240, 135, 128, 128, 128,
                     128, 128, 128, 128, 128, 30,  40,  50,  60,  66,  0,
                     64,  78,  23,  33,  44,  55,  66,  77,  88,  99};
  uint8_t keys[32] = {40,  40,  40,  40,  40,  40,  40,  40,  255, 255, 255,
                      255, 255, 255, 255, 255, 40,  40,  40,  40,  40,  40,
                      40,  40,  255, 255, 255, 255, 255, 255, 255, 255};
  uint8_t alphas[32] = {10, 10, 10, 10, 10, 10, 10, 10, 50, 50, 50,
                        50, 50, 50, 50, 50, 10, 10, 10, 10, 10, 10,
                        10, 10, 50, 50, 50, 50, 50, 50, 50, 50};

  for (int i = 0; i < 32; i++) {
    bg[i] = bgs[i];
    key[i] = keys[i];
    alpha[i] = alphas[i];
  }
}

void outputImage(uint8_t *bg, int width, int height) {
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

int main(int argc, char *argv[]) {
  // Setup data
  // Allocating more memory than needed to align the pointer to a 32 byte
  // boundary.
  // ------------------------------------------------------------------------
  Aligned<uint8_t> bg(_IMAGE_WIDTH * _IMAGE_HEIGHT * 4);
  Aligned<uint8_t> bg_sse(_IMAGE_WIDTH * _IMAGE_HEIGHT * 4);

  Aligned<uint8_t> key(_IMAGE_WIDTH * _IMAGE_HEIGHT * 4);

  Aligned<uint8_t> alpha(_IMAGE_WIDTH * _IMAGE_HEIGHT * 4);

  fill8Pixels(bg.geta(), key.geta(), alpha.geta());
  fill8Pixels(bg_sse.geta(), key.geta(), alpha.geta());

#if (OUTPUT_IMAGE == 1)
  std::cout << "Original image" << std::endl;
  outputImage(bg.geta(), _IMAGE_WIDTH, _IMAGE_HEIGHT);
#endif

  // Simple non intrinsic implementation
  // ------------------------------------------------------------------------
  // ------------------------------------------------------------------------
  uint64_t start = rdtsc();
  apply_key(bg.geta(), key.geta(), alpha.geta(), _IMAGE_WIDTH, _IMAGE_HEIGHT);
  uint64_t total = rdtsc() - start;

#if (OUTPUT_IMAGE == 1)
  std::cout << "Keyed image" << std::endl;
  outputImage(bg.geta(), _IMAGE_WIDTH, _IMAGE_HEIGHT);
#endif

  std::cout << "Cycles :: " << total << std::endl;

  // Intrinsic implementation
  // ------------------------------------------------------------------------
  // ------------------------------------------------------------------------
  start = rdtsc();
  apply_key_avx_2(bg_sse.geta(), key.geta(), alpha.geta(), _IMAGE_WIDTH,
                  _IMAGE_HEIGHT);
  total = rdtsc() - start;

#if (OUTPUT_IMAGE == 1)
  std::cout << "Keyed image" << std::endl;
  outputImage(bg_sse.geta(), _IMAGE_WIDTH, _IMAGE_HEIGHT);
#endif

  std::cout << "Cycles :: " << total << std::endl;

  return 0;
}
