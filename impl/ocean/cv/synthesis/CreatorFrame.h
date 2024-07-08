/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_FRAME_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_FRAME_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Creator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all creator objects that create a resulting frame as output.
 * @ingroup cvsynthesis
 */
class CreatorFrame : virtual public Creator
{
	protected:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 * @param target The target frame that will receive the creator output
		 */
		inline CreatorFrame(const Layer& layer, Frame& target);

	protected:

		/// Target frame of the creator.
		Frame& target_;
};

inline CreatorFrame::CreatorFrame(const Layer& layer, Frame& target) :
	Creator(layer),
	target_(target)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_FRAME_H
