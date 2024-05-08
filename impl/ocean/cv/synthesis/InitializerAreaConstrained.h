/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_AREA_CONSTRAINED_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_AREA_CONSTRAINED_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a base class for all initializers basing on area constraints.
 * @ingroup cvsynthesis
 */
class InitializerAreaConstrained : virtual public Initializer
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param filter The filter mask that divides the target region into desired and undesired target content
		 */
		inline InitializerAreaConstrained(Layer& layer, const Frame& filter);

	protected:

		/// Synthesis filter dividing the target region into desired and undesired content.
		const Frame& filter_;
};

inline InitializerAreaConstrained::InitializerAreaConstrained(Layer& layer, const Frame& filter) :
	Initializer(layer),
	filter_(filter)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_AREA_CONSTRAINED_H
