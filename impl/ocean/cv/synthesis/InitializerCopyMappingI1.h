/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_COPY_MAPPING_I_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_COPY_MAPPING_I_1_H

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
 * This initializer simply copies an existing mapping.<br>
 * The existing mapping must have the same dimension as the synthesis layer
 * @see InitializerMoveMappingI1.
 * @ingroup cvsynthesis
 */
class InitializerCopyMappingI1 :
	virtual public InitializerI,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param mapping Existing mapping that is copied (must have the same dimension as the layer that receives the mapping)
		 */
		inline InitializerCopyMappingI1(LayerI1& layer, const MappingI1& mapping);

		/**
		 * Invokes the initialization.
		 * @see Initializer::invoke().
		 */
		bool invoke(Worker* worker) const override;

	private:

		/// Existing mapping that will be copied.
		const MappingI1& mapping_;
};

inline InitializerCopyMappingI1::InitializerCopyMappingI1(LayerI1& layer, const MappingI1& mapping) :
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

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_COPY_MAPPING_I_1_H
