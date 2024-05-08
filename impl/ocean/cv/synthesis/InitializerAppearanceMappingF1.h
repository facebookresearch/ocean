/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_APPEARANCE_MAPPING_F_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_APPEARANCE_MAPPING_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerAppearanceMapping.h"
#include "ocean/cv/synthesis/InitializerF.h"
#include "ocean/cv/synthesis/InitializerRandomized.h"
#include "ocean/cv/synthesis/InitializerSubset.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerF1.h"

#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an initializer that initializes the mapping by appearance constraints for mappings with float accuracy.
 * For each mask pixel the best matching target pixel is assigned as initial mapping.<br>
 * The matching is provided by a randomized test of best matching positions.
 * @tparam tPatchSize Defines the patch size for the matching, must be 1
 * @tparam tIterations Defines the number of random seek iterations for each pixel inside the synthesis mask, with range [1, infinity)
 * @see LayerF1, MappingF.
 * @ingroup cvsynthesis
 */
template <unsigned int tPatchSize, unsigned int tIterations>
class InitializerAppearanceMappingF1 :
	virtual public InitializerAppearanceMapping,
	virtual public InitializerF,
	virtual public InitializerRandomized,
	virtual public InitializerSubset,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param randomGenerator Random number generator
		 */
		inline InitializerAppearanceMappingF1(LayerF1& layer, RandomGenerator& randomGenerator);

	private:

		/**
		 * Initializes a subset of the entire mapping area.
		 * @see InitializerSubset::initializeSubset().
		 * @see initializeSubsetChannels().
		 */
		void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

		/**
		 * This function is the specialization of the default initializeSubset() function.
		 * @param firstColumn First column of the mapping area to be initialized
		 * @param numberColumns Number of columns of the mapping area to be handled
		 * @param firstRow First row of the mapping area to be initialized
		 * @param numberRows Number of rows of the mapping area to be handled
		 * @tparam tChannels Number of channels the frame has
		 * @see initializeSubset().
		 */
		template <unsigned int tChannels>
		void initializeSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const;
};

template <unsigned int tPatchSize, unsigned int tIterations>
inline InitializerAppearanceMappingF1<tPatchSize, tIterations>::InitializerAppearanceMappingF1(LayerF1& layer, RandomGenerator& randomGenerator) :
	Initializer(layer),
	InitializerAppearanceMapping(layer),
	InitializerF(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer)
{
	// nothing to do here
}

template <unsigned int tPatchSize, unsigned int tIterations>
void InitializerAppearanceMappingF1<tPatchSize, tIterations>::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tPatchSize == 1u, "Invalid patch size!");

	ocean_assert(layerF_.frame().numberPlanes() == 1u);
	ocean_assert(layer_.frame().dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (layerF_.frame().channels())
	{
		case 1u:
			initializeSubsetChannels<1u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 2u:
			initializeSubsetChannels<2u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 3u:
			initializeSubsetChannels<3u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 4u:
			initializeSubsetChannels<4u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		default:
			ocean_assert(false && "Invalid frame type.");
	}
}

template <unsigned int tPatchSize, unsigned int tIterations>
template <unsigned int tChannels>
void InitializerAppearanceMappingF1<tPatchSize, tIterations>::initializeSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tPatchSize == 1, "Missing Implementation: Currently tPatchSize must be 1");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	const unsigned int tPatchSize_2 = tPatchSize / 2u;

	const unsigned int width = layerF_.width();
	const unsigned int height = layerF_.height();

	MappingF& layerMapping = layerF_.mapping();

	const Frame& frame = layerF_.frame();
	const Frame& mask = layerF_.mask();

	ocean_assert(frame.isValid() && mask.isValid());

	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.width() == width);
	ocean_assert(frame.height() == height);

	ocean_assert(frame.isFrameTypeCompatible(FrameType(mask, frame.pixelFormat()), false));

	RandomGenerator randomGenerator(randomGenerator_);

	const uint8_t* const frameData = frame.constdata<uint8_t>();
	const uint8_t* const maskData = mask.constdata<uint8_t>();

	const unsigned int framePaddingElements = frame.paddingElements();
	const unsigned int maskStrideElements = mask.strideElements();

#ifdef OCEAN_DEBUG
	const PixelBoundingBox& debugLayerBoundingBox = layerF_.boundingBox();
	ocean_assert(!debugLayerBoundingBox || firstColumn >= debugLayerBoundingBox.left());
	ocean_assert(!debugLayerBoundingBox || firstColumn + numberColumns <= debugLayerBoundingBox.rightEnd());
	ocean_assert(!debugLayerBoundingBox || firstRow >= debugLayerBoundingBox.top());
	ocean_assert(!debugLayerBoundingBox || firstRow + numberRows <= debugLayerBoundingBox.bottomEnd());
#endif // OCEAN_DEBUG

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(firstRow + numberRows <= height);

	const Scalar xLocationMax = Scalar(width - tPatchSize_2 - 1u) - Numeric::weakEps();
	const Scalar yLocationMax = Scalar(height - tPatchSize_2 - 1u) - Numeric::weakEps();

	for (unsigned int y = firstRow; y < std::min(firstRow + numberRows, height - 1u); ++y)
	{
		const uint8_t* maskRow = maskData + y * maskStrideElements + firstColumn;
		Vector2* position = layerMapping.row(y) + firstColumn;

		for (unsigned int x = firstColumn; x < std::min(firstColumn + numberColumns, width - 1u); ++x)
		{
			if (*maskRow++ != 0xFFu)
			{
				Scalar bestX, bestY;

				do
				{
					bestX = Random::scalar(randomGenerator, Scalar(tPatchSize_2), xLocationMax);
					bestY = Random::scalar(randomGenerator, Scalar(tPatchSize_2), yLocationMax);
				}
				while (maskData[Numeric::round32(bestY) * int(maskStrideElements) + Numeric::round32(bestX)] != 0xFF);

				unsigned int bestSSD = Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(frameData, frameData, width, width, Scalar(x), Scalar(y), bestX, bestY, framePaddingElements, framePaddingElements)
														+ (unsigned int)(sqrDistance(Scalar(x), Scalar(bestX)) + sqrDistance(Scalar(y), Scalar(bestY))) / 2u;

				for (unsigned int n = 1u; n < tIterations; ++n)
				{
					const Scalar candidateX = Random::scalar(randomGenerator, Scalar(tPatchSize_2), xLocationMax);
					const Scalar candidateY = Random::scalar(randomGenerator, Scalar(tPatchSize_2), yLocationMax);

					if (maskData[Numeric::round32(candidateY) * int(maskStrideElements) + Numeric::round32(candidateX)] != 0xFF)
					{
						continue;
					}

					const unsigned int candidateSSD = Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(frameData, frameData, width, width, Scalar(x), Scalar(y), candidateX, candidateY, framePaddingElements, framePaddingElements)
																	+ (unsigned int)(sqrDistance(Scalar(x), Scalar(candidateX)) + sqrDistance(Scalar(y), Scalar(candidateY))) / 2u;

					if (candidateSSD < bestSSD)
					{
						bestX = candidateX;
						bestY = candidateY;
						bestSSD = candidateSSD;
					}
				}

				*position = Vector2(bestX, bestY);
			}

			++position;
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_APPEARANCE_MAPPING_F_1_H
