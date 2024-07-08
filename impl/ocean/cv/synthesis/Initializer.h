/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Operator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all synthesis initializers.
 * An initializer provides the initial mapping for a synthesis layer.<br>
 * The initializer holds only references to e.g. the synthesis layers or any other input or output information.<br>
 * Each initialization object is a simple container that gathers all objects that are necessary for the initialization directly when it is created.<br>
 * Once the initializer has been created it should be invoked.<br>
 * A initializer should not be invoked twice.
 * @see Initializer::invoke().
 * @ingroup cvsynthesis
 */
class Initializer : virtual public Operator
{
	public:

		/**
		 * Invokes the initializer.
		 * @param worker Optional worker object to distribute the computation.
		 * @return True, if succeeded
		 */
		virtual bool invoke(Worker* worker = nullptr) const = 0;

	protected:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 */
		inline Initializer(Layer& layer);

	protected:

		/// Synthesis layer that has to be initialized.
		Layer& layer_;
};

inline Initializer::Initializer(Layer& layer) :
	layer_(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_H
