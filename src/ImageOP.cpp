#include "ImageOP.h"
#include "NearestNeighborSampler.h"
#include "LerpSampler.h"

namespace image {

void convolve(const imagev3* input, imagev3* output, const imagev3* kernel) {
	const int kw = kernel->getWidth();
	const int kh = kernel->getHeight();
	const int iw = input->getWidth();
	const int ih = input->getHeight();
	NearestNeighborSampler<la::v3> smp(input, EDGE_CLAMP);
	// loop image
	#pragma omp parallel for
	for (int j = 0; j < ih; j++) {
		for (int i = 0; i < iw; i++) {
			// loop kernel
			la::v3 sum;
			for (int y = 0; y < kh; y++) {
				for (int x = 0; x < kw; x++) {
					sum += smp.get(i - kw / 2 + x, j - kh / 2 + y) * kernel->get(x, y);
				}
			}
			output->set(i, j, sum);
		}
	}
}

void copy(const imagev3* input, imagev3* output) {
	output->set(*input);
}

void threshold(const imagef* input, imageb* output, float thr) {
	map(input, output, [&] (const float c) {
		return c > thr;
	});
}

void saturate(const imagev3* input, imagev3* output) {
	map(input, output, imageop::sat);
}

void negative(const imagev3* input, imagev3* output) {
	map(input, output, imageop::neg);
}

void bnot(const imageb* input, imageb* output) {
	map(input, output, [] (const bool c) {
		return !c;
	});
}

void band(const imageb* input1, const imageb* input2, imageb* output) {
	binop(input1, input2, output, [] (const bool a, const bool b) {
		return a && b;
	});
}

void bor(const imageb* input1, const imageb* input2, imageb* output) {
	binop(input1, input2, output, [] (const bool a, const bool b) {
		return a || b;
	});
}

void bxor(const imageb* input1, const imageb* input2, imageb* output) {
	binop(input1, input2, output, [] (const bool a, const bool b) {
		return a != b;
	});
}

void beq(const imageb* input1, const imageb* input2, imageb* output) {
	binop(input1, input2, output, [] (const bool a, const bool b) {
		return a == b;
	});
}

void bsub(const imageb* input1, const imageb* input2, imageb* output) {
	binop(input1, input2, output, [] (const bool a, const bool b) {
		return a && !b;
	});	
}

void dilate(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery) {
	const int kw = kernel->getWidth();
	const int kh = kernel->getHeight();
	const int iw = input->getWidth();
	const int ih = input->getHeight();
	NearestNeighborSampler<bool> smp(input, EDGE_ZERO);
	// loop image
	#pragma omp parallel for
	for (int j = 0; j < ih; j++) {
		for (int i = 0; i < iw; i++) {
			// loop kernel
			bool sum = false; // reduction init
			for (int y = 0; y < kh; y++) {
				for (int x = 0; x < kw; x++) {
					sum = sum || (smp.get(i - centerx + x, j - centery + y) && kernel->get(x, y));
				}
			}
			output->set(i, j, sum);
		}
	}
}

void erode(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery) {
	const int kw = kernel->getWidth();
	const int kh = kernel->getHeight();
	const int iw = input->getWidth();
	const int ih = input->getHeight();
	NearestNeighborSampler<bool> smp(input, EDGE_ONE);
	// loop image
	#pragma omp parallel for
	for (int j = 0; j < ih; j++) {
		for (int i = 0; i < iw; i++) {
			// loop kernel
			bool sum = true; // reduction init
			for (int y = 0; y < kh; y++) {
				for (int x = 0; x < kw; x++) {
					sum = sum && (smp.get(i - centerx + x, j - centery + y) || !kernel->get(x, y));
				}
			}
			output->set(i, j, sum);
		}
	}
}

void open(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery) {
	imageb* temp = new imageb(input->getWidth(), input->getHeight());
	erode(input, temp, kernel, centerx, centery);
	dilate(temp, output, kernel, centerx, centery);
	delete temp;
}

void close(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery) {
	imageb* temp = new imageb(input->getWidth(), input->getHeight());
	dilate(input, temp, kernel, centerx, centery);
	erode(temp, output, kernel, centerx, centery);
	delete temp;
}

void hitmiss(const imageb* input, imageb* output, const imageb* kernel1, const imageb* kernel2, int centerx, int centery) {
	const int kw = kernel1->getWidth();
	const int kh = kernel1->getHeight();
	const int iw = input->getWidth();
	const int ih = input->getHeight();
	NearestNeighborSampler<bool> smpzero(input, EDGE_ZERO);
	NearestNeighborSampler<bool> smpone(input, EDGE_ONE);
	// loop image
	#pragma omp parallel for
	for (int j = 0; j < ih; j++) {
		for (int i = 0; i < iw; i++) {
			// loop kernel
			bool sum = true; // reduction init
			for (int y = 0; y < kh; y++) {
				for (int x = 0; x < kw; x++) {
					bool valuezero = smpzero.get(i - centerx + x, j - centery + y);
					bool valueone = smpone.get(i - centerx + x, j - centery + y);
					bool foregroundCheck = !kernel1->get(x, y) || valuezero;
					bool backgroundCheck = !kernel2->get(x, y) || !valueone;
					sum = sum && (foregroundCheck && backgroundCheck);
				}
			}
			output->set(i, j, sum);
		}
	}
}

int distanceTransform(const imageb* input, imagei* output, const imageb* kernel, int centerx, int centery) {
	int distance = 0;
	bool imageChanged = false;
	output->fill(0);
	imageb* beforeErode = new imageb(input->getWidth(), input->getHeight());
	imageb* afterErode = new imageb(input->getWidth(), input->getHeight());
	beforeErode->set(*input);
	do {
		distance++;
		erode(beforeErode, afterErode, kernel, centerx, centery);
		imageChanged = false;
		#pragma omp parallel for
		for (int j = 0; j < input->getHeight(); j++) {
			for (int i = 0; i < input->getWidth(); i++) {
				if (beforeErode->get(i, j) && !afterErode->get(i, j)) {
					imageChanged = true;
					output->set(i, j, distance);
				}
			}
		}
		// swap pointers for buffer reuse
		imageb* T = beforeErode;
		beforeErode = afterErode;
		afterErode = T;
	} while (imageChanged);
	delete beforeErode;
	delete afterErode;
	return distance;
}

void boundaries(const imageb* input, imageb* output, const imageb* kernel, int centerx, int centery) {
	imageb* temp = new imageb(input->getWidth(), input->getHeight());
	erode(input, temp, kernel, centerx, centery);
	bsub(input, temp, output);
	delete temp;
}

float globalThreshold(const imagef* input, imageb* output, float eps) {
	float T = 0.5f;
	float Tprev;
	do {
		// segment with T
		map(input, output, [&] (const float c) {
			return c > T;
		});
		// calculate m1, m2
		float m1 = 0.f;
		float m2 = 1.f;
		int n1 = 0;
		int n2 = 0;
		for (int j = 0; j < input->getHeight(); j++) {
			for (int i = 0; i < input->getWidth(); i++) {
				if (!output->get(i, j)) {
					n1++;
					m1 = ((n1 - 1) / (float)n1) * m1 + input->get(i, j) / (float)n1;
				} else {
					n2++;
					m2 = ((n2 - 1) / (float)n2) * m2 + input->get(i, j) / (float)n2;
				}
			}
		}
		// prepare next iteration
		Tprev = T;
		T = (m1 + m2) / 2;
	} while (fabs(T - Tprev) > eps);
	return T;
}

void sobel(const imagev3* input, imagev3* output) {
	imagev3 kx(3, 3);
	imagev3 buffer(input->getWidth(), input->getHeight());
	kx.set(0, 0, -la::v3::E)
	  .set(0, 2, -la::v3::E)
	  .set(2, 0, la::v3::E)
	  .set(2, 2, la::v3::E)
	  .set(0, 1, -la::v3::E * 2)
	  .set(2, 1, la::v3::E * 2);
	convolve(input, &buffer, &kx);
	imagev3 ky(3, 3);
	ky.set(0, 0, -la::v3::E)
	  .set(2, 0, -la::v3::E)
	  .set(0, 2, la::v3::E)
	  .set(2, 2, la::v3::E)
	  .set(1, 0, -la::v3::E * 2)
	  .set(1, 2, la::v3::E * 2);
	convolve(input, output, &ky);
	binop(&buffer, output, output, imageop::com);
}

}