/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_SHRINKING_EROSION_I_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_SHRINKING_EROSION_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerAppearanceMapping.h"
#include "ocean/cv/synthesis/InitializerI.h"
#include "ocean/cv/synthesis/InitializerRandomized.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an initializer that initializes the mapping by shrinking the inpainting mask by an erosion filter.
 * For the resulting initial inpainting image the final mapping is determined by application of simple appearance mapping initializer.
 * @see InitializerAppearanceMappingI1, LayerI1, MappingI.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT InitializerShrinkingErosionI1 :
	virtual public InitializerI,
	virtual public InitializerRandomized,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param randomGenerator Random number generator
		 * @param appearanceInitializer Appearance initializer to be applied afterwards
		 */
		inline InitializerShrinkingErosionI1(LayerI1& layer, RandomGenerator& randomGenerator, const InitializerAppearanceMapping& appearanceInitializer);

		/**
		 * Invokes the initialization process.
		 * @see Initializer::invoke().
		 */
		bool invoke(Worker* worker = nullptr) const override;

	private:

		/// Additional appearance mapping initializer.
		const InitializerAppearanceMapping& appearanceInitializer_;
};

inline InitializerShrinkingErosionI1::InitializerShrinkingErosionI1(LayerI1& layer, RandomGenerator& randomGenerator, const InitializerAppearanceMapping& appearanceInitializer) :
	Initializer(layer),
	InitializerI(layer),
	InitializerRandomized(layer, randomGenerator),
	Initializer1(layer),
	appearanceInitializer_(appearanceInitializer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_SHRINKING_EROSION_I_1_H
