#ifndef IMAGE_OP_H_INCL
#define IMAGE_OP_H_INCL

#include <image2.h>
#include <la.h>

namespace image {

namespace imageop {
	auto avg = [] (const la::v3& c) { return la::v3((c.x + c.y + c.z) / 3); };
	auto lum = [] (const la::v3& c) { return la::v3(0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z); };
	auto max = [] (const la::v3& c) { return la::v3(fmax(fmax(c.x, c.y), c.z)); };
	auto min = [] (const la::v3& c) { return la::v3(fmin(fmin(c.x, c.y), c.z)); };
	auto sat = [] (const la::v3& c) { return Utils::clamp(c, la::v3::O, la::v3::E); };
	auto neg = [] (const la::v3& c) { return la::v3::E - c; };
	auto lsq = [] (const la::v3& c) { return la::v3(c.lensq()); };
	auto len = [] (const la::v3& c) { return la::v3(c.len()); };
	auto rgb = [] (const la::v3& c) { return la::v3(c.x, c.y, c.z); };
	auto rbg = [] (const la::v3& c) { return la::v3(c.x, c.z, c.y); };
	auto grb = [] (const la::v3& c) { return la::v3(c.y, c.x, c.z); };
	auto gbr = [] (const la::v3& c) { return la::v3(c.y, c.z, c.x); };
	auto brg = [] (const la::v3& c) { return la::v3(c.z, c.x, c.y); };
	auto bgr = [] (const la::v3& c) { return la::v3(c.z, c.y, c.x); };
	auto ftr = [] (const la::v3& c) { return la::v3(c.x, 0, 0); };
	auto ftg = [] (const la::v3& c) { return la::v3(0, c.y, 0); };
	auto ftb = [] (const la::v3& c) { return la::v3(0, 0, c.z); };
	auto add = [] (const la::v3& a, const la::v3& b) { return a + b; };
	auto sub = [] (const la::v3& a, const la::v3& b) { return a - b; };
	auto mul = [] (const la::v3& a, const la::v3& b) { return a * b; };
	auto div = [] (const la::v3& a, const la::v3& b) { return a / b; };
	auto com = [] (const la::v3& a, const la::v3& b) { return la::v3(a.x, b.x, 0); };
}

template<typename IN, typename OUT>
void convert(const image2<IN>* input, image2<OUT>* output) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(i, j, OUT(input->get(i, j)));
		}
	}
}

template<typename IN, typename OUT, typename F>
void map(const image2<IN>* input, image2<OUT>* output, F func) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(i, j, func(input->get(i, j)));
		}
	}
}

template<typename IN, typename OUT, typename F>
void filter(const image2<IN>* input, image2<OUT>* output, F func) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(i, j, func(input->get(i, j)) ? la::v3::E : la::v3::O);
		}
	}
}

template<typename IN, typename OUT, typename F>
void binop(const image2<IN>* input1, const image2<IN>* input2, image2<OUT>* output, F func) {
	#pragma omp parallel for
	for (int j = 0; j < input1->getHeight(); j++) {
		for (int i = 0; i < input1->getWidth(); i++) {
			output->set(i, j, func(input1->get(i, j), input2->get(i, j)));
		}
	}
}

template<typename T>
void rotatecw(const image2<T>* input, image2<T>* output) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(output->getWidth() - j - 1, i, input->get(i, j));
		}
	}	
}

template<typename T>
void rotateccw(const image2<T>* input, image2<T>* output) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(j, output->getHeight() - i - 1, input->get(i, j));
		}
	}	
}

template<typename T>
void transpose(const image2<T>* input, image2<T>* output) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(j, i, input->get(i, j));
		}
	}	
}

template<typename T>
void mirrorx(const image2<T>* input, image2<T>* output) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(output->getWidth() - i - 1, j, input->get(i, j));
		}
	}	
}

template<typename T>
void mirrory(const image2<T>* input, image2<T>* output) {
	#pragma omp parallel for
	for (int j = 0; j < input->getHeight(); j++) {
		for (int i = 0; i < input->getWidth(); i++) {
			output->set(i, output->getHeight() - j - 1, input->get(i, j));
		}
	}	
}

void convolve(const imagev3* input, imagev3* output, const imagev3* kernel);

void copy(const imagev3* input, imagev3* output);
void threshold(const imagef* input, imageb* output, float threshold);
void saturate(const imagev3* input, imagev3* output);
void negative(const imagev3* input, imagev3* output);

void bnot(const imageb* input, const imageb* output);
void band(const imageb* input1, const imageb* input2, imageb* output);
void bor(const imageb* input1, const imageb* input2, imageb* output);
void bxor(const imageb* input1, const imageb* input2, imageb* output);
void beq(const imageb* input1, const imageb* input2, imageb* output);
void bsub(const imageb* input1, const imageb* input2, imageb* output);

void dilate(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery);
void erode(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery);
void open(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery);
void close(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery);
void hitmiss(const imageb* input, imageb* output, const imageb* kernel1, const imageb* kernel2, int centerx, int centery);

int distanceTransform(const imageb* input, imagei* output, const imageb* kernel, int centerx, int centery);
void boundaries(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery);

float globalThreshold(const imagef* input, imageb* output, float eps);

void sobel(const imagev3* input, imagev3* output);

}

#endif