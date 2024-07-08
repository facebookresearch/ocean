/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_I_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_I_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"
#include "ocean/cv/synthesis/LayerI.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all initializer objects that are applied for mappings with integer accuracy.
 * @see MappingI.
 * @ingroup cvsynthesis
 */
class InitializerI : virtual public Initializer
{
	protected:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 */
		inline InitializerI(LayerI& layer);

	protected:

		/// Specialized layer reference.
		LayerI& layerI_;
};

inline InitializerI::InitializerI(LayerI& layer) :
	Initializer(layer),
	layerI_(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_I_H
