#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

#include "aligned.hpp"
#include "keyer.hpp"
#include "profiler.hpp"

// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
// http://const.me/articles/simd/simd.pdf

#define PROFILE_IN_TIME true

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

void function() { std::cout << "DO YOU SEE THIS!!!" << std::endl; }

int main(int argc, char *argv[]) {
  int alphaWidth;
  int alphaHeight;
  int alphaBpp;
  int keyWidth;
  int keyHeight;
  int keyBpp;
  int bgWidth;
  int bgHeight;
  int bgBpp;

  // Read all images
  std::string alpha_fn = "alpha.png";
  std::string key_fn = "key.png";
  std::string background_fn = "background.png";

  // Load images: alpha, key and background
  // ------------------------------------------------------------------------
  uint8_t *rgba_alpha =
      stbi_load(alpha_fn.c_str(), &alphaWidth, &alphaHeight, &alphaBpp, 4);
  uint8_t *rgba_key =
      stbi_load(key_fn.c_str(), &keyWidth, &keyHeight, &keyBpp, 4);
  uint8_t *rgba_background =
      stbi_load(background_fn.c_str(), &bgWidth, &bgHeight, &bgBpp, 4);

  if (alphaWidth != keyWidth || alphaWidth != bgWidth || keyWidth != bgWidth) {
    std::cout << "Error: Width mismatch" << std::endl;
    std::cout << "Alpha Width: " << alphaWidth << std::endl;
    std::cout << "Key Width: " << keyWidth << std::endl;
    std::cout << "Bg Width: " << bgWidth << std::endl;
    return -1;
  }

  if (alphaHeight != keyHeight || alphaHeight != bgHeight ||
      keyHeight != bgHeight) {
    std::cout << "Error: Height mismatch" << std::endl;
    std::cout << "Alpha Height: " << alphaHeight << std::endl;
    std::cout << "Key Height: " << keyHeight << std::endl;
    std::cout << "Bg Height: " << bgHeight << std::endl;
    return -1;
  }

  if (alphaBpp != keyBpp || alphaBpp != bgBpp || keyBpp != bgBpp) {
    std::cout << "Warning: Bpp mismatch" << std::endl;
    std::cout << "Alpha BPP: " << alphaBpp << std::endl;
    std::cout << "Key BPP: " << keyBpp << std::endl;
    std::cout << "Bg BPP: " << bgBpp << std::endl;
    // return -1;
  }

  const int width = alphaWidth;
  const int height = alphaHeight;
  const int bpp = alphaBpp;

  const int limageSize = width * height * 4;

  // Aligned buffers
  // ------------------------------------------------------------------------
  Aligned<uint8_t> bg(limageSize);
  Aligned<uint8_t> bg_sse(limageSize);
  Aligned<uint8_t> key(limageSize);
  Aligned<uint8_t> alpha(limageSize);

  std::memcpy(bg.geta(), rgba_background, limageSize);
  std::memcpy(bg_sse.geta(), rgba_background, limageSize);
  std::memcpy(key.geta(), rgba_key, limageSize);
  std::memcpy(alpha.geta(), rgba_alpha, limageSize);

  std::cout << "Images copied" << std::endl;

  stbi_image_free(rgba_alpha);
  stbi_image_free(rgba_key);
  stbi_image_free(rgba_background);

  // Simple non intrinsic implementation
  // ------------------------------------------------------------------------
  // ------------------------------------------------------------------------
  __PROFILE(
      [&]() { apply_key(bg.geta(), key.geta(), alpha.geta(), width, height); },
      bool(PROFILE_IN_TIME));

  // Intrinsic implementation
  // ------------------------------------------------------------------------
  // ------------------------------------------------------------------------
  __PROFILE(
      [&]() {
        apply_key_avx_2(bg_sse.geta(), key.geta(), alpha.geta(), width, height);
      },
      bool(PROFILE_IN_TIME));

  stbi_write_png("result-simple.png", width, height, 4, bg.geta(), 0);
  stbi_write_png("result-sse.png", width, height, 4, bg_sse.geta(), 0);

  return 0;
}
