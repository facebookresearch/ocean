/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_CONVERT_MAPPING_F_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_CONVERT_MAPPING_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerF.h"
#include "ocean/cv/synthesis/InitializerSubset.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerF1.h"
#include "ocean/cv/synthesis/LayerI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an initializer that converts the mapping of a layer with integer mapping to a layer with float mapping.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT InitializerConvertMappingF1 :
	virtual public InitializerF,
	virtual public InitializerSubset,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param sourceLayer Layer with existing mapping that will be converted (must have the same dimension as the layer that receives the mapping)
		 */
		inline InitializerConvertMappingF1(LayerF1& layer, const LayerI1& sourceLayer);

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param sourceMapping Existing mapping that will be converted (must have the same dimension as the layer that receives the mapping)
		 */
		inline InitializerConvertMappingF1(LayerF1& layer, const MappingI1& sourceMapping);

	private:

		/**
		 * Initializes a subset of the entire mapping area.
		 * @see InitializerSubset::initializeSubset().
		 */
		void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

	private:

		/// Source mapping that will be converted.
		const MappingI1& sourceMapping_;
};

inline InitializerConvertMappingF1::InitializerConvertMappingF1(LayerF1& layer, const LayerI1& sourceLayer) :
	Initializer(layer),
	InitializerF(layer),
	InitializerSubset(layer),
	Initializer1(layer),
	sourceMapping_(sourceLayer.mapping())
{
	// nothing to do here
}

inline InitializerConvertMappingF1::InitializerConvertMappingF1(LayerF1& layer, const MappingI1& mapping) :
	Initializer(layer),
	InitializerF(layer),
	InitializerSubset(layer),
	Initializer1(layer),
	sourceMapping_(mapping)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_CONVERT_MAPPING_F_1_H
