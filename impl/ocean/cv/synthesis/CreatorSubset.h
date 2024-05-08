/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_SUBSET_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_SUBSET_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Creator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a creator that can be distributed to subsets of the synthesis layer.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT CreatorSubset : virtual public Creator
{
	public:

		/**
		 * Invokes the creator.
		 * @see Creator::invoke().
		 */
		bool invoke(Worker* worker = nullptr) const override;

	protected:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 */
		inline CreatorSubset(const Layer& layer);

		/**
		 * Creates a subset of the information.
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 */
		virtual void createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const = 0;
};

inline CreatorSubset::CreatorSubset(const Layer& layer) :
	Creator(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_SUBSET_H
