#ifndef IMAGE_IO_PPM3_H_INCL
#define IMAGE_IO_PPM3_H_INCL

#include "ImageIO.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

namespace image {

class ImageIO_PPM3 : public ImageIO {

public:

	ImageIO_PPM3() {
	}

	virtual void write(const imagev3* img, const char* filename) {
		FILE* file = fopen(filename, "w");
		if (!file) {
			return;
		}
		fprintf(file, "P3 %d %d 255\n", img->getWidth(), img->getHeight());

		for (int j = 0; j < img->getHeight(); ++j) {
			for (int i = 0; i < img->getWidth(); ++i) {
				la::v3 color = img->get(i, j);
				fprintf(file, "%d %d %d\n",
					(int) (Utils::clamp(color.x, 0.f, 1.f) * 255),
					(int) (Utils::clamp(color.y, 0.f, 1.f) * 255),
					(int) (Utils::clamp(color.z, 0.f, 1.f) * 255));
			}
		}

		fclose(file);
	}

	virtual imagev3* read(const char* filename) {
		FILE* file = fopen(filename, "r");
		if (!file) {
			return NULL;
		}
		char magic[3];
		fgets(magic, 3, file);
		if (strcmp(magic, "P3")) {
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
				la::v3 color;
				fscanf(file, "%f%f%f", &color.x, &color.y, &color.z);
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