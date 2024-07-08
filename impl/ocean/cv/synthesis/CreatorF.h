/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_F_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_F_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Creator.h"
#include "ocean/cv/synthesis/LayerF.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all creators that support mappings with float accuracy.
 * @ingroup cvsynthesis
 */
class CreatorF : virtual public Creator
{
	protected:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 */
		inline CreatorF(const LayerF& layer);

	protected:

		/// Specialized layer reference.
		const LayerF& layerF_;
};

inline CreatorF::CreatorF(const LayerF& layer) :
	Creator(layer),
	layerF_(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_F_H
