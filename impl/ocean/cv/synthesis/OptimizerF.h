/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_F_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_F_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerF.h"
#include "ocean/cv/synthesis/Optimizer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all optimizers that use a mapping with float accuracy.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT OptimizerF : virtual public Optimizer
{
	public:

		/**
		 * Calculates the search radii for the mapping optimization.
		 * @param radii Number of radii to create, with range [1, infinity)
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @return Resulting search radii
		 */
		static inline std::vector<Scalar> calculateSearchRadii(const unsigned int radii, const unsigned int width, const unsigned int height);

	protected:

		/**
		 * Creates a new optimizer object.
		 * @param layer The layer that has to be optimized
		 */
		inline OptimizerF(LayerF& layer);

	private:

		/// Specialized layer reference.
		LayerF& layerF_;
};

inline OptimizerF::OptimizerF(LayerF& layer) :
	Operator(),
	Optimizer(layer),
	layerF_(layer)
{
	OCEAN_SUPPRESS_UNUSED_WARNING(layerF_);
}

inline std::vector<Scalar> OptimizerF::calculateSearchRadii(const unsigned int radii, const unsigned int width, const unsigned int height)
{
	ocean_assert(radii != 0u);
	ocean_assert(width != 0u && height != 0u);

	std::vector<Scalar> result(radii);

	const Scalar maxWidthHeight = Scalar(max(width, height));

	for (unsigned int n = 0; n < radii; ++n)
	{
		result[n] = max(Scalar(2), Scalar(Scalar(maxWidthHeight) - Scalar(maxWidthHeight - 1u) * Scalar(n) / Scalar(radii)) * Scalar(0.5));
	}

	return result;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_F_H
