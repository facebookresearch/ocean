// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_RANDOM_MAPPING_AREA_CONSTRAINED_I_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_RANDOM_MAPPING_AREA_CONSTRAINED_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerAreaConstrained.h"
#include "ocean/cv/synthesis/InitializerI.h"
#include "ocean/cv/synthesis/InitializerRandomized.h"
#include "ocean/cv/synthesis/InitializerSubset.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This initializer creates a random initial mapping and further respects a filter that defines undesired source elements.
 * The initializer randomly selects target positions as long as they lie outside the synthesis mask.<br>
 * No appearance constraints will be used to create the initial mapping.<br>
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT InitializerRandomMappingAreaConstrainedI1 :
	virtual public InitializerAreaConstrained,
	virtual public InitializerI,
	virtual public InitializerRandomized,
	virtual public InitializerSubset,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param randomGenerator Random generator object used to created random numbers during the initialization process
		 * @param filter The filter mask that divides the target region into desired and undesired target content
		 */
		inline InitializerRandomMappingAreaConstrainedI1(LayerI1& layer, RandomGenerator& randomGenerator, const Frame& filter);

	private:

		/**
		 * Initializes a subset of the entire mapping area.
		 * @see InitializerSubset::initializeSubset().
		 * @see initializeSubsetChannels().
		 */
		void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;
};

inline InitializerRandomMappingAreaConstrainedI1::InitializerRandomMappingAreaConstrainedI1(LayerI1& layer, RandomGenerator& randomGenerator, const Frame& filter) :
	Initializer(layer),
	InitializerAreaConstrained(layer, filter),
	InitializerI(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_RANDOM_MAPPING_AREA_CONSTRAINED_I_1_H
