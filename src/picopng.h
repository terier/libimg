#ifndef PICOPNG_H_INCL
#define PICOPNG_H_INCL

#include <vector>
#include <iostream>

extern int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32);

#endif