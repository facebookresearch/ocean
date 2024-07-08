/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_MOVE_MAPPING_I_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_MOVE_MAPPING_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerI.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerI1.h"
#include "ocean/cv/synthesis/MappingI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This initializer undertakes an existing mapping by application of the move constructor.<br>
 * The existing mapping must have the same dimension as the synthesis layer.<br>
 * @see InitializerCopyMappingI1.
 * @ingroup cvsynthesis
 */
class InitializerMoveMappingI1 :
	virtual public InitializerI,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param mapping Existing mapping that will be undertaken (must have the same dimension as the layer that receives the mapping)
		 */
		inline InitializerMoveMappingI1(LayerI1& layer, MappingI1&& mapping);

		/**
		 * Invokes the initialization.
		 * @see Initializer::invoke().
		 */
		bool invoke(Worker* worker) const override;

	private:

		/// Existing mapping that will be undertaken by this initializer.
		MappingI1& mapping_;
};

InitializerMoveMappingI1::InitializerMoveMappingI1(LayerI1& layer, MappingI1&& mapping) :
	Initializer(layer),
	InitializerI(layer),
	Initializer1(layer),
	mapping_(mapping)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_MOVE_MAPPING_I_1_H
