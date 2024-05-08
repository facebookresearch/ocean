/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_SUBSET_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_SUBSET_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Optimizer.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all optimizers that are able to optimize seperate subsets of the synthesis content.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT OptimizerSubset : virtual public Optimizer
{
	public:

		/**
		 * Creates a new optimizer object.
		 * @param layer Synthesis layer that has to be optimized
		 * @param randomGenerator Random number generator to be used during the optimization
		 */
		inline OptimizerSubset(Layer& layer, RandomGenerator& randomGenerator);

		/**
		 * Invokes the operator.
		 * @see Optimizer::invoke().
		 */
		bool invoke(const unsigned int radii, const unsigned int iterations = 5u, const unsigned int maxSpatialCost = 0xFFFFFFFFu, Worker* worker = nullptr, const bool applyInitialMapping = true) const override;

	protected:

		/**
		 * Optimizes a subset of the synthesis frame.
		 * @param radii Number of improvement radii during one optimization iteration for each mapping position
		 * @param maxSpatialCost Maximal spatial cost
		 * @param boundingBoxTop First row of the entire synthesis area
		 * @param boundingBoxHeight Number of rows of the entire synthesis area
		 * @param downIsMain True, if the downwards direction is the main optimization direction (for all subsets with even thread indices)
		 * @param firstColumn First column to be handled in the subset
		 * @param numberColumns Number of columns to be handled in the subset
		 * @param rowOffset Offset within the entire synthesis area (boundingBoxHeight), the subset may be moved by this offset
		 * @param firstRow First row to be handled in the subset
		 * @param numberRows Number of rows to be handled in the subset
		 * @param threadIndex Index of the thread that executes the subset optimization function
		 */
		virtual void optimizeSubset(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const = 0;

	protected:

		/// Random number generator of this optimizer.
		RandomGenerator& randomGenerator_;
};

inline OptimizerSubset::OptimizerSubset(Layer& layer, RandomGenerator& randomGenerator) :
	Optimizer(layer),
	randomGenerator_(randomGenerator)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_SUBSET_H
