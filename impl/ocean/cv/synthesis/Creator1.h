// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_1_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Creator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all creators that support mappings for one frame.
 * @ingroup cvsynthesis
 */
class Creator1 : virtual public Creator
{
	protected:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 */
		inline Creator1(const Layer& layer);
};

inline Creator1::Creator1(const Layer& layer) :
	Creator(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_1_H
