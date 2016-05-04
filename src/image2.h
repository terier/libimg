#ifndef IMAGE2_H_INCL
#define IMAGE2_H_INCL

#include <utils.h>
#include <la.h>

namespace image {

template<typename T>
class image2 {

	T* data;
	int width;
	int height;

public:

	image2(int w, int h) : width(w), height(h) {
		data = new T[w * h];
	}

	image2(const image2& img) : width(img.width), height(img.height) {
		data = new T[img.width * img.height];
		set(img);
	}

	~image2() {
		delete [] data;
	}

	const T* ptr() const { return data; }

	int getWidth() const { return width; }
	int getHeight() const { return height; }

	void copyFrom(const image2& img, int x1, int y1, int x2, int y2, int xdest, int ydest) {
		#pragma omp parallel for
		for (int i = x1; i <= x2; ++i) {
			for (int j = y1; j <= y2; ++j) {
				data[xdest + (i - x1) + (ydest + (j - y1)) * width] = img.data[i + j * img.width];
			}
		}
	}

	image2& set(const image2& img) {
		#pragma omp parallel for
		for (int i = 0; i < img.width * img.height; ++i) {
			data[i] = img.data[i];
		}
		return *this;
	}

	T get(int x, int y) const {
		return data[x + width * y];
	}

	image2& set(int x, int y, const T& c) {
		data[x + width * y] = c;
		return *this;
	}

	image2& fill(const T& c) {
		#pragma omp parallel for
		for (int i = 0; i < width * height; ++i) {
			data[i] = c;
		}
		return *this;
	}

};

typedef image2<la::v3> imagev3;
typedef image2<float> imagef;
typedef image2<bool> imageb;
typedef image2<int> imagei;

}

#endif