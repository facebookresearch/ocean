// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_I_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_I_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Creator.h"
#include "ocean/cv/synthesis/LayerI.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all creators that support mappings with integer accuracy.
 * @ingroup cvsynthesis
 */
class CreatorI : virtual public Creator
{
	protected:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 */
		inline CreatorI(const LayerI& layer);

	protected:

		/// Specialized layer reference.
		const LayerI& layerI_;
};

inline CreatorI::CreatorI(const LayerI& layer) :
	Creator(layer),
	layerI_(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_I_H
