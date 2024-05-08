/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_F_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_F_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"
#include "ocean/cv/synthesis/LayerF.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all initializer objects that are applied for mappings with float accuracy.
 * @see MappingF.
 * @ingroup cvsynthesis
 */
class InitializerF : virtual public Initializer
{
	protected:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 */
		inline InitializerF(LayerF& layer);

	protected:

		/// Specialized layer reference.
		LayerF& layerF_;
};

inline InitializerF::InitializerF(LayerF& layer) :
	Initializer(layer),
	layerF_(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_F_H
