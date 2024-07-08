/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Operator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all synthesis optimizers.
 * An optimizer tries to improve a given synthesis mapping to provide a more optimal synthesis result.<br>
 * The optimizer should not be applied before the mapping has been initialized.
 * @ingroup cvsynthesis
 */
class Optimizer : virtual public Operator
{
	public:

		/**
		 * Invokes the operator.
		 * @param radii Number of improvement radii during one optimization iteration for each mapping position
		 * @param iterations Number of optimization iterations that will be applied
		 * @param maxSpatialCost Maximal spatial cost
		 * @param worker Optional worker object to distribute the computation
		 * @param applyInitialMapping True, to apply the initial mapping before the optimization starts
		 * @return True, if succeeded
		 */
		virtual bool invoke(const unsigned int radii, const unsigned int iterations = 5u, const unsigned int maxSpatialCost = 0xFFFFFFFFu, Worker* worker = nullptr, const bool applyInitialMapping = true) const = 0;

	protected:

		/**
		 * Creates a new operator object.
		 * @param layer The layer for that this operator is applied
		 */
		inline Optimizer(Layer& layer);

	protected:

		/// Optimizer layer.
		Layer& layer_;
};

inline Optimizer::Optimizer(Layer& layer) :
	layer_(layer)
{

}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_H
