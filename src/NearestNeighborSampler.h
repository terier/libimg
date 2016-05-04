#ifndef NEAREST_NEIGHBOR_SAMPLER_H_INCL
#define NEAREST_NEIGHBOR_SAMPLER_H_INCL

#include "Sampler.h"

namespace image {

template<typename T>
class NearestNeighborSampler : public Sampler<T> {

public:

	NearestNeighborSampler(const image2<T>* image, EdgeOperation edgeop = EDGE_ZERO)
		: Sampler<T>(image, edgeop)
	{
	}

	virtual T sample(const la::v2& p) {
		return Sampler<T>::get((int) p.x, (int) p.y);
	}

};

}

#endif