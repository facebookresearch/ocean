/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_RANDOMIZED_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_RANDOMIZED_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all initializers that mainly initialize the synthesis mapping by a heuristic (randomized) approach.
 * @ingroup cvsynthesis
 */
class InitializerRandomized : virtual public Initializer
{
	protected:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param randomGenerator Random generator object used to created random numbers during the initialization process
		 */
		inline InitializerRandomized(Layer& layer, RandomGenerator& randomGenerator);

	protected:

		/// Random number generator.
		RandomGenerator& randomGenerator_;
};

inline InitializerRandomized::InitializerRandomized(Layer& layer, RandomGenerator& randomGenerator) :
	Initializer(layer),
	randomGenerator_(randomGenerator)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_RANDOMIZED_H
