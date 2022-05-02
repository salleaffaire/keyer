#ifndef __DEBUG_HPP___
#define __DEBUG_HPP___

#include <iostream>
#include <iomanip>
#include <cstring>

#include "aligned.hpp"
#include "keyer.hpp"

#define _WIDTH 16  // In pixels
#define _HEIGHT 1  // In pixels
#define _BPP 4     // Bytes per pixels

#define _IMAGESIZE (_WIDTH * _HEIGHT * _BPP)

const unsigned char bgdata[_IMAGESIZE] =
 // R     G     B     A  |  R     G     B     A  |  R     G     B     A  |  R     G     B     A 
 {0x00, 0x01, 0x02, 0xFF, 0x10, 0x11, 0x12, 0xFF, 0x20, 0x21, 0x22, 0xFF, 0x30, 0x31, 0x32, 0xFF, // 4 Pixels
  0x40, 0x41, 0x42, 0xFF, 0x50, 0x51, 0x52, 0xFF, 0x60, 0x61, 0x62, 0xFF, 0x70, 0x71, 0x72, 0xFF,
  0x80, 0x81, 0x82, 0xFF, 0x90, 0x91, 0x92, 0xFF, 0xa0, 0xa1, 0xa2, 0xFF, 0xb0, 0xb1, 0xb2, 0xFF,
  0xc0, 0xc1, 0xc2, 0xFF, 0xd0, 0xd1, 0xd2, 0xFF, 0xe0, 0xe1, 0xe2, 0xFF, 0xf0, 0xf1, 0xf2, 0xFF
 };

 const unsigned char keydata[_IMAGESIZE] =
 // R     G     B     A  |  R     G     B     A  |  R     G     B     A  |  R     G     B     A 
 {0xaa, 0xaa, 0xaa, 0xFF, 0xaa, 0xaa, 0xaa, 0xFF, 0xaa, 0xaa, 0xaa, 0xFF, 0xaa, 0xaa, 0xaa, 0xFF, // 4 Pixels
  0xbb, 0xbb, 0xbb, 0xFF, 0xbb, 0xbb, 0xbb, 0xFF, 0xbb, 0xbb, 0xbb, 0xFF, 0xbb, 0xbb, 0xbb, 0xFF,
  0x22, 0x22, 0x22, 0xFF, 0x22, 0x22, 0x22, 0xFF, 0x22, 0x22, 0x22, 0xFF, 0x22, 0x22, 0x22, 0xFF,
  0x33, 0x33, 0x33, 0xFF, 0x66, 0x66, 0x66, 0xFF, 0x11, 0x11, 0x11, 0xFF, 0x00, 0x00, 0x00, 0xFF
 };

  const unsigned char alphadata[_IMAGESIZE] =
 // R     G     B     A  |  R     G     B     A  |  R     G     B     A  |  R     G     B     A 
 {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, // 4 Pixels
  0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00,
  0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00,
  0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00,
 };


void output(unsigned char *o) {
    for (int i = 0;i < _IMAGESIZE;i+=_BPP) {
        if ((i % 16 == 0) && (i != 0)) {
            std::cout << std::endl;
        } 
        std::cout << std::hex 
                  << std::setw(2) << std::setfill('0') << (int)o[i+0] << " "
                  << std::setw(2) << std::setfill('0') << (int)o[i+1] << " " 
                  << std::setw(2) << std::setfill('0') << (int)o[i+2] << " " 
                  << std::setw(2) << std::setfill('0') << (int)o[i+3] << " ";
    }
    std::cout << std::dec << std::endl;
} 

int debug() {

    // Aligned buffers
    // ------------------------------------------------------------------------
    Aligned<uint8_t> bg(_IMAGESIZE);
    Aligned<uint8_t> key(_IMAGESIZE);
    Aligned<uint8_t> alpha(_IMAGESIZE);

    std::memcpy(bg.geta(), bgdata, _IMAGESIZE);
    std::memcpy(key.geta(), keydata, _IMAGESIZE);
    std::memcpy(alpha.geta(), alphadata, _IMAGESIZE);

    std::cout << "Background" << std::endl;
    output(bg.geta());
    std::cout << "Key" << std::endl;
    output(key.geta());
    std::cout << "Alpha" << std::endl;
    output(alpha.geta());


    std::cout << "Background" << std::endl;
    output(bg.geta());    
}

#endif