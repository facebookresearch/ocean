/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_HOMOGRAPHY_MAPPING_ADAPTION_F_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_HOMOGRAPHY_MAPPING_ADAPTION_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerF.h"
#include "ocean/cv/synthesis/InitializerRandomized.h"
#include "ocean/cv/synthesis/InitializerSubset.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerF1.h"

#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an initializer that initializes the float mapping by the application of a previous mapping with corresponding homography.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT InitializerHomographyMappingAdaptionF1 :
	virtual public InitializerF,
	virtual public InitializerRandomized,
	virtual public InitializerSubset,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param sourceLayer Source synthesis layer with same dimension as the initializer layer and float accuracy
		 * @param randomGenerator Random number generator
		 * @param homography Homography transformation that is applied
		 */
		inline InitializerHomographyMappingAdaptionF1(LayerF1& layer, const LayerF1& sourceLayer, RandomGenerator& randomGenerator, const SquareMatrix3& homography);

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param sourceMapping Source mapping with same dimension as the initializer layer and float accuracy
		 * @param randomGenerator Random number generator
		 * @param homography Homography transformation that is applied
		 */
		inline InitializerHomographyMappingAdaptionF1(LayerF1& layer, const MappingF1& sourceMapping, RandomGenerator& randomGenerator, const SquareMatrix3& homography);

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param sourceLayer Source synthesis layer with same dimension as the initializer layer and integer accuracy
		 * @param randomGenerator Random number generator
		 * @param homography Homography transformation that is applied
		 */
		inline InitializerHomographyMappingAdaptionF1(LayerF1& layer, const LayerI1& sourceLayer, RandomGenerator& randomGenerator, const SquareMatrix3& homography);

	private:

		/**
		 * Initializes a subset of the entire mapping area.
		 * @see InitializerSubset::initializeSubset().
		 * @see initializeSubsetChannels().
		 */
		void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

		/**
		 * Initializes a subset of the entire mapping area.
		 * The source layer has float accuracy.
		 * @param firstColumn First column of the mapping area to be initialized
		 * @param numberColumns Number of columns of the mapping area to be handled
		 * @param firstRow First row of the mapping area to be initialized
		 * @param numberRows Number of rows of the mapping area to be handled
		 */
		void initializeSubsetF1(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const;

		/**
		 * Initializes a subset of the entire mapping area.
		 * The source layer has integer accuracy.
		 * @param firstColumn First column of the mapping area to be initialized
		 * @param numberColumns Number of columns of the mapping area to be handled
		 * @param firstRow First row of the mapping area to be initialized
		 * @param numberRows Number of rows of the mapping area to be handled
		 */
		void initializeSubsetI1(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const;

	private:

		/// Source mapping to be adapted with float accuracy.
		const MappingF1* sourceMappingF1_;

		/// Source mapping to be adapted with integer accuracy.
		const MappingI1* sourceMappingI1_;

		/// Homography transformation.
		const SquareMatrix3 homography_;
};

inline InitializerHomographyMappingAdaptionF1::InitializerHomographyMappingAdaptionF1(LayerF1& layer, const LayerF1& sourceLayer, RandomGenerator& randomGenerator, const SquareMatrix3& homography) :
	Initializer(layer),
	InitializerF(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer),
	sourceMappingF1_(&sourceLayer.mappingF1()),
	sourceMappingI1_(nullptr),
	homography_(homography)
{
	// nothing to do here
}

inline InitializerHomographyMappingAdaptionF1::InitializerHomographyMappingAdaptionF1(LayerF1& layer, const MappingF1& sourceMapping, RandomGenerator& randomGenerator, const SquareMatrix3& homography) :
	Initializer(layer),
	InitializerF(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer),
	sourceMappingF1_(&sourceMapping),
	sourceMappingI1_(nullptr),
	homography_(homography)
{
	// nothing to do here
}

inline InitializerHomographyMappingAdaptionF1::InitializerHomographyMappingAdaptionF1(LayerF1& layer, const LayerI1& sourceLayer, RandomGenerator& randomGenerator, const SquareMatrix3& homography) :
	Initializer(layer),
	InitializerF(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer),
	sourceMappingF1_(nullptr),
	sourceMappingI1_(&sourceLayer.mappingI1()),
	homography_(homography)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_HOMOGRAPHY_MAPPING_ADAPTION_F_1_H
