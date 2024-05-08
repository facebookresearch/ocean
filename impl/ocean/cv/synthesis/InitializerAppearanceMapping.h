/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_APPEARANCE_MAPPING_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_APPEARANCE_MAPPING_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the abstract base class for all appearance initializers.
 * @ingroup cvsynthesis
 */
class InitializerAppearanceMapping : virtual public Initializer
{
	protected:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer that has to be initialized
		 */
		inline InitializerAppearanceMapping(Layer& layer);
};

inline InitializerAppearanceMapping::InitializerAppearanceMapping(Layer& layer) :
	Initializer(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_APPEARANCE_MAPPING_H
