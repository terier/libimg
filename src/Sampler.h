#ifndef SAMPLER_H_INCL
#define SAMPLER_H_INCL

#include "image2.h"
#include <la.h>

namespace image {

enum EdgeOperation {
	EDGE_ZERO,
	EDGE_ONE,
	EDGE_CLAMP,
	EDGE_REPEAT
};

template<typename T>
class Sampler {

protected:

	const image2<T>* image;
	EdgeOperation edgeOp;

public:

	Sampler(const image2<T>* img, EdgeOperation eo = EDGE_ZERO)
		: image(img), edgeOp(eo)
	{
	}

	virtual ~Sampler() {
	}

	virtual T sample(const la::v2& p) = 0;

	T get(int x, int y) {
		if (x < 0 || y < 0 || x >= image->getWidth() || y >= image->getHeight()) {
			switch (edgeOp) {
				case EDGE_ZERO:
					return T(0);
					break;
				case EDGE_ONE:
					return T(1);
				case EDGE_CLAMP:
					return image->get(Utils::clamp(x, 0, image->getWidth() - 1), Utils::clamp(y, 0, image->getHeight() - 1));
					break;
				case EDGE_REPEAT:
					return image->get(Utils::mod(x, image->getWidth()), Utils::mod(y, image->getHeight()));
					break;
				default:
					return T(0);
					break;
			}
		} else {
			return image->get(x, y);
		}
	}

};

}

#endif