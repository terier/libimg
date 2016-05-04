#ifndef IMAGE_IO_H_INCL
#define IMAGE_IO_H_INCL

#include "image2.h"

namespace image {

class ImageIO {

public:

	virtual void write(const imagev3* img, const char* filename) = 0;
	virtual imagev3* read(const char* filename) = 0;
	
};

}

#endif