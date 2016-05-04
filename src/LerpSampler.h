#ifndef LERP_SAMPLER_H_INCL
#define LERP_SAMPLER_H_INCL

#include "Sampler.h"

namespace image {

class LerpSampler : public Sampler<la::v3> {

public:

	LerpSampler(const imagev3* image, EdgeOperation edgeop = EDGE_ZERO)
		: Sampler(image, edgeop)
	{
	}

	virtual la::v3 sample(const la::v2& p) {
		int x0 = (int) p.x;
		int y0 = (int) p.y;
		int x1 = x0 + 1;
		int y1 = y0 + 1;
		float ax = p.x - (float) x0;
		float ay = p.y - (float) y0;
		la::v3 c00 = get(x0, y0);
		la::v3 c01 = get(x0, y1);
		la::v3 c10 = get(x1, y0);
		la::v3 c11 = get(x1, y1);
		return c00.lerp(c01, ay).lerp(c10.lerp(c11, ay), ax);
	}

};

}

#endif