/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all initializers that are provided for a single frame only.
 * The resulting mapping starts and ends in the same frame.
 * @ingroup cvsynthesis
 */
class Initializer1 : virtual public Initializer
{
	protected:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 */
		inline Initializer1(Layer& layer);
};

inline Initializer1::Initializer1(Layer& layer) :
	Initializer(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_1_H
