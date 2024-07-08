/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_I_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_I_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerI.h"
#include "ocean/cv/synthesis/Optimizer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all optimizers that use a mapping with integer accuracy.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT OptimizerI : virtual public Optimizer
{
	public:

		/**
		 * Calculate the search radii for the mapping optimization.
		 * @param radii Number of radii to create, with range [1, infinity)
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @return Resulting search radii
		 */
		static inline std::vector<int> calculateSearchRadii(const unsigned int radii, const unsigned int width, const unsigned int height);

	protected:

		/**
		 * Creates a new optimizer object.
		 * @param layer Synthesis layer that will be optimized
		 */
		inline OptimizerI(LayerI& layer);

	private:

		/// Specialized layer reference.
		LayerI& layerI_;
};

inline OptimizerI::OptimizerI(LayerI& layer) :
	Operator(),
	Optimizer(layer),
	layerI_(layer)
{
	OCEAN_SUPPRESS_UNUSED_WARNING(layerI_);
}

inline std::vector<int> OptimizerI::calculateSearchRadii(const unsigned int radii, const unsigned int width, const unsigned int height)
{
	ocean_assert(radii != 0u);
	ocean_assert(width != 0u && height != 0u);

	std::vector<int> result(radii);

	const unsigned int maxWidthHeight = max(width, height);

	for (unsigned int n = 0u; n < radii; ++n)
	{
		result[n] = int(max(2u, (maxWidthHeight - (maxWidthHeight - 1u) * n / radii + 1u) / 2u));
	}

	return result;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_I_H
