#ifndef __KEYER_HPP___
#define __KEYER_HPP___

#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>  // Need this for SSE compiler intrinsics

void apply_key(uint8_t *bg, uint8_t *key, uint8_t *alpha, int width,
               int height) {
  for (uint32_t i = 0; i < height; i++) {
    for (uint32_t j = 0; j < width; j++) {
      // R
      uint32_t temp = (bg[0] * alpha[0]) + (key[0] * (255 - alpha[0]));
      // std::cout << "R = " << (temp >> 8) << std::endl;
      bg[0] = (temp >> 8);

      // G
      temp = (bg[1] * alpha[1]) + (key[1] * (255 - alpha[1]));
      // std::cout << "G = " << (temp >> 8) << std::endl;
      bg[1] = (temp >> 8);

      // B
      temp = (bg[2] * alpha[2]) + (key[2] * (255 - alpha[2]));
      // std::cout << "B = " << (temp >> 8) << std::endl;
      bg[2] = (temp >> 8);

      // A
      temp = (bg[3] * alpha[3]) + (key[3] * (255 - alpha[3]));
      // std::cout << "B = " << (temp >> 8) << std::endl;
      bg[3] = (temp >> 8);

      // Skip over
      bg += 4;
      key += 4;
      alpha += 4;
    }
  }
}

void apply_key_sse2(uint8_t *bg, uint8_t *key, uint8_t *alpha, int width,
                    int height) {
  const auto pixel_group = 4;
  const auto pixel_size = 4;
  int num_pixels = width * height;

  for (uint32_t i = 0; i < num_pixels; i += pixel_group) {
    // std::cout << i << " " << pixel_group << " - "
    //           << " Offset = " << std::dec << i * pixel_group << std::endl;
    const __m128i zero = _mm_setzero_si128();
    const __m128i twoFiftyFive =
        _mm_set_epi32(0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF);
    __m128i curBgGroup =
        *(__m128i *)(&bg[i * pixel_size]);  // Load 4 Background pixels
    __m128i curKeyGroup =
        *(__m128i *)(&key[i * pixel_size]);  // Load 4 Key pixels
    __m128i curAlphaGroup =
        *(__m128i *)(&alpha[i * pixel_size]);  // Load 4 Alpha pixels

    // Convert to 16-bit
    __m128i curBgGroup16Lo = _mm_unpacklo_epi8(curBgGroup, zero);
    __m128i curBgGroup16Hi = _mm_unpackhi_epi8(curBgGroup, zero);

    __m128i curKeyGroup16Lo = _mm_unpacklo_epi8(curKeyGroup, zero);
    __m128i curKeyGroup16Hi = _mm_unpackhi_epi8(curKeyGroup, zero);

    __m128i curAlphaGroup16Lo = _mm_unpacklo_epi8(curAlphaGroup, zero);
    __m128i curAlphaGroup16Hi = _mm_unpackhi_epi8(curAlphaGroup, zero);

    // Backgroud = Alpha * Background
    curBgGroup16Lo = _mm_mullo_epi16(curBgGroup16Lo, curAlphaGroup16Lo);
    curBgGroup16Hi = _mm_mullo_epi16(curBgGroup16Hi, curAlphaGroup16Hi);

    // Key = (255 * Alpha) * Key
    curKeyGroup16Lo = _mm_mullo_epi16(
        curKeyGroup16Lo, _mm_sub_epi16(twoFiftyFive, curAlphaGroup16Lo));
    curKeyGroup16Hi = _mm_mullo_epi16(
        curKeyGroup16Hi, _mm_sub_epi16(twoFiftyFive, curAlphaGroup16Hi));

    // Background = Background + Key
    curBgGroup16Lo = _mm_adds_epi16(curBgGroup16Lo, curKeyGroup16Lo);
    curBgGroup16Hi = _mm_adds_epi16(curBgGroup16Hi, curKeyGroup16Hi);

    // Shift right by 8 (divide by 256)
    curBgGroup16Lo = _mm_srli_epi16(curBgGroup16Lo, 8);
    curBgGroup16Hi = _mm_srli_epi16(curBgGroup16Hi, 8);

    // Pack the data back
    __m128i result = _mm_packus_epi16(curBgGroup16Lo, curBgGroup16Hi);

    _mm_store_si128((__m128i *)(&bg[i * pixel_group]), result);
  }
}

void apply_key_avx(uint8_t *bg, uint8_t *key, uint8_t *alpha, int width,
                   int height) {
  const auto pixel_group = 8;
  const auto pixel_size = 4;
  int num_pixels = width * height;

  for (uint32_t i = 0; i < num_pixels; i += pixel_group) {
    // std::cout << "LOOP" << std::endl;
    // std::cout << i << " " << pixel_group << " - "
    //           << " Offset = " << std::dec << i * pixel_group << std::endl;
    const __m256i zero = _mm256_setzero_si256();
    const __m256i twoFiftyFive =
        _mm256_set_epi32(0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF,
                         0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF);
    __m256i curBgGroup =
        *(__m256i *)(&bg[i * pixel_size]);  // Load 4 Background pixels
    __m256i curKeyGroup =
        *(__m256i *)(&key[i * pixel_size]);  // Load 4 Key pixels
    __m256i curAlphaGroup =
        *(__m256i *)(&alpha[i * pixel_size]);  // Load 4 Alpha pixels

    // Convert to 16-bit
    __m256i curBgGroup16Lo = _mm256_unpacklo_epi8(curBgGroup, zero);
    __m256i curBgGroup16Hi = _mm256_unpackhi_epi8(curBgGroup, zero);

    __m256i curKeyGroup16Lo = _mm256_unpacklo_epi8(curKeyGroup, zero);
    __m256i curKeyGroup16Hi = _mm256_unpackhi_epi8(curKeyGroup, zero);

    __m256i curAlphaGroup16Lo = _mm256_unpacklo_epi8(curAlphaGroup, zero);
    __m256i curAlphaGroup16Hi = _mm256_unpackhi_epi8(curAlphaGroup, zero);

    // Backgroud = Alpha * Background
    curBgGroup16Lo = _mm256_mullo_epi16(curBgGroup16Lo, curAlphaGroup16Lo);
    curBgGroup16Hi = _mm256_mullo_epi16(curBgGroup16Hi, curAlphaGroup16Hi);

    // Key = (255 * Alpha) * Key
    curKeyGroup16Lo = _mm256_mullo_epi16(
        curKeyGroup16Lo, _mm256_sub_epi16(twoFiftyFive, curAlphaGroup16Lo));
    curKeyGroup16Hi = _mm256_mullo_epi16(
        curKeyGroup16Hi, _mm256_sub_epi16(twoFiftyFive, curAlphaGroup16Hi));

    // Background = Background + Key
    curBgGroup16Lo = _mm256_adds_epi16(curBgGroup16Lo, curKeyGroup16Lo);
    curBgGroup16Hi = _mm256_adds_epi16(curBgGroup16Hi, curKeyGroup16Hi);

    // Shift right by 8 (divide by 256)
    curBgGroup16Lo = _mm256_srli_epi16(curBgGroup16Lo, 8);
    curBgGroup16Hi = _mm256_srli_epi16(curBgGroup16Hi, 8);

    // Pack the data back
    __m256i result = _mm256_packus_epi16(curBgGroup16Lo, curBgGroup16Hi);

    _mm256_store_si256((__m256i *)(&bg[i * pixel_group]), result);
  }
}

void apply_key_avx_2(uint8_t *bg, uint8_t *key, uint8_t *alpha, int width,
                     int height) {
  const auto pixel_group = 4;
  const auto pixel_size = 4;

  int num_pixels = width * height;

  for (uint32_t i = 0; i < num_pixels; i += pixel_group) {
    // std::cout << i << " " << pixel_group << " - "
    //           << " Offset = " << std::dec << i * pixel_group << std::endl;
    const __m256i zero = _mm256_setzero_si256();
    const __m256i twoFiftyFive =
        _mm256_set_epi32(0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF,
                         0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF);
    __m128i curBgGroup =
        *(__m128i *)(&bg[i * pixel_size]);  // Load 4 Background pixels
    __m128i curKeyGroup =
        *(__m128i *)(&key[i * pixel_size]);  // Load 4 Key pixels
    __m128i curAlphaGroup =
        *(__m128i *)(&alpha[i * pixel_size]);  // Load 4 Alpha pixels

    // Convert to 16-bit
    __m256i curBgGroup16 = _mm256_cvtepu8_epi16(curBgGroup);
    __m256i curKeyGroup16 = _mm256_cvtepu8_epi16(curKeyGroup);
    __m256i curAlphaGroup16 = _mm256_cvtepu8_epi16(curAlphaGroup);

    // Backgroud = Alpha * Background
    curBgGroup16 = _mm256_mullo_epi16(curBgGroup16, curAlphaGroup16);

    // Key = (255 * Alpha) * Key
    curKeyGroup16 = _mm256_mullo_epi16(
        curKeyGroup16, _mm256_sub_epi16(twoFiftyFive, curAlphaGroup16));

    // Background = Background + Key
    curBgGroup16 = _mm256_adds_epi16(curBgGroup16, curKeyGroup16);

    // Shift right by 8 (divide by 256)
    curBgGroup16 = _mm256_srli_epi16(curBgGroup16, 8);

    // Swap (hi/low 128-bit)
    // curBgGroup16 = _mm256_castps_si256(
    //     _mm256_permute2f128_ps(_mm256_castsi256_ps(curBgGroup16),
    //                            _mm256_castsi256_ps(curBgGroup16), 1));

    // Pack the data back
    __m256i result = _mm256_packus_epi16(
        curBgGroup16, _mm256_castps_si256(_mm256_permute2f128_ps(
                          _mm256_castsi256_ps(curBgGroup16),
                          _mm256_castsi256_ps(curBgGroup16), 1)));

    __m128i result8 = _mm256_castsi256_si128(result);

    _mm_store_si128((__m128i *)(&bg[i * pixel_group]), result8);
  }
}

#endif