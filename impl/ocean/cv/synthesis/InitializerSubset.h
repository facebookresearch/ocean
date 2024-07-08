/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_SUBSET_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_SUBSET_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Initializer.h"
#include "ocean/cv/synthesis/Layer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all initializer objects that can separate the initialization process into individual subsets (of the mapping area).
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT InitializerSubset : virtual public Initializer
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 */
		inline InitializerSubset(Layer& layer);

		/**
		 * Invokes the initialization process.
		 * @see Initializer::invoke().
		 */
		bool invoke(Worker* worker = nullptr) const override;

	protected:

		/**
		 * Initializes a subset of the entire mapping area.
		 * This function has to be implemented in derived classes that support a subset initialization process.<br>
		 * @param firstColumn First column of the mapping area to be initialized
		 * @param numberColumns Number of columns of the mapping area to be handled
		 * @param firstRow First row of the mapping area to be initialized
		 * @param numberRows Number of rows of the mapping area to be handled
		 */
		virtual void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const = 0;
};

inline InitializerSubset::InitializerSubset(Layer& layer) :
	Initializer(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_SUBSET_H
