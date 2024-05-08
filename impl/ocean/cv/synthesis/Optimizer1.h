/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_1_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Optimizer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all synthesis optimizers that use one single frame.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT Optimizer1 : virtual public Optimizer
{
	protected:

		/**
		 * Creates a new optimizer object.
		 * @param layer The layer to be optimized
		 */
		inline Optimizer1(Layer& layer);
};

inline Optimizer1::Optimizer1(Layer& layer) :
	Operator(),
	Optimizer(layer)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_1_H
