/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Operator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all creators.
 * A creator uses the current state of the synthesis layer to create any kind of information.<br>
 * @ingroup cvsynthesis
 */
class Creator : virtual public Operator
{
	public:

		/**
		 * Invokes the creator and provides the result.
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		virtual bool invoke(Worker* worker = nullptr) const = 0;

	protected:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 */
		inline Creator(const Layer& layer);

	protected:

		/// Creator layer to be used of information creation.
		const Layer& layer_;
};

inline Creator::Creator(const Layer& layer) :
	layer_(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_H
