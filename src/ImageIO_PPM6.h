#ifndef IMAGE_IO_PPM6_H_INCL
#define IMAGE_IO_PPM6_H_INCL

#include "ImageIO.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

namespace image {

class ImageIO_PPM6 : public ImageIO {

public:

	ImageIO_PPM6() {
	}

	virtual void write(const imagev3* img, const char* filename) {
		FILE* file = fopen(filename, "wb");
		if (!file) {
			return;
		}
		fprintf(file, "P6 %d %d 255\n", img->getWidth(), img->getHeight());

		for (int j = 0; j < img->getHeight(); ++j) {
			for (int i = 0; i < img->getWidth(); ++i) {
				la::v3 color = img->get(i, j);
				unsigned char c[3];
				c[0] = (unsigned char) (Utils::clamp(color.x, 0.f, 1.f) * 255);
				c[1] = (unsigned char) (Utils::clamp(color.y, 0.f, 1.f) * 255);
				c[2] = (unsigned char) (Utils::clamp(color.z, 0.f, 1.f) * 255);
				fwrite(c, 1, 3, file);
			}
		}

		fclose(file);
	}

	virtual imagev3* read(const char* filename) {
		FILE* file = fopen(filename, "rb");
		if (!file) {
			return NULL;
		}
		char magic[3];
		fgets(magic, 3, file);
		if (strcmp(magic, "P6")) {
			return NULL;
		}
		skipComment(file);
		int width = 0;
		int height = 0;
		fscanf(file, "%d%d", &width, &height);
		skipComment(file);
		float max;
		fscanf(file, "%f", &max);
		skipComment(file);

		imagev3* image = new imagev3(width, height);
		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				unsigned char c[3];
				la::v3 color;
				fread(c, 1, 3, file);
				color.set(c[0], c[1], c[2]);
				color /= max;
				image->set(i, j, color);
			}
		}

		fclose(file);
		return image;
	}

private:

	void skipLine(FILE* file) {
		int c;
		while ((c = fgetc(file)) != '\n') {
		}
	}

	void skipWhitespace(FILE* file) {
		int c;
		while (isspace(c = fgetc(file))) {
		}
		ungetc(c, file);
	}

	void skipComment(FILE* file) {
		int c;
		skipWhitespace(file);
		if ((c = fgetc(file)) == '#') {
			skipLine(file);
		} else {
			ungetc(c, file);
		}
	}
	
};

}

#endif