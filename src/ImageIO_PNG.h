#ifndef IMAGE_IO_PNG_H_INCL
#define IMAGE_IO_PNG_H_INCL

#include "ImageIO.h"
#include "utils.h"

#include <fstream>
#include <vector>
#include <picopng.h>

namespace image {

class ImageIO_PNG : public ImageIO {

public:

	ImageIO_PNG() {
	}

	virtual void write(const imagev3*, const char*) {
		fprintf(stderr, "ImageIO_PNG: write not implemented");
	}

	virtual imagev3* read(const char* filename) {
		std::vector<unsigned char> buffer, image;
		loadFile(buffer, filename);
		unsigned long w, h;
		int error = decodePNG(image, w, h, &buffer[0], buffer.size(), true);
		if (!error) {
			imagev3* img = new imagev3(w, h);
			for (unsigned long j = 0; j < h; j++) {
				for (unsigned long i = 0; i < w; i++) {
					float r = image[4 * (i + w * j) + 0] / 255.f;
					float g = image[4 * (i + w * j) + 1] / 255.f;
					float b = image[4 * (i + w * j) + 2] / 255.f;
					img->set(i, j, la::v3(r, g, b));
				}
			}
			return img;
		} else {
			return NULL;
		}
	}

private:

	void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) {
		std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

		std::streamsize size = 0;
		if (file.seekg(0, std::ios::end).good()) size = file.tellg();
		if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

		if (size > 0) {
			buffer.resize((size_t)size);
			file.read((char*)(&buffer[0]), size);
		} else {
			buffer.clear();
		}
	}

};

}

#endif