/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_I_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerI.h"
#include "ocean/cv/synthesis/InitializerRandomized.h"
#include "ocean/cv/synthesis/InitializerSubset.h"
#include "ocean/cv/synthesis/Initializer1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This initializer creates an initial mapping by the adaption of an already existing mapping of a coarser synthesis layer.<br>
 * The initializer supports mapping with integer accuracy.<br>
 * The coarser mapping is upsampled and adjusted to the synthesis mask.
 * @tparam tFactor The template parameter defines the dimension increase factor between the synthesis layer and the given coarser layer. A factor of 2 means that the width and height of the synthesis layer is two times larger than the width and height of the given coarser layer, with range [2, infinity)
 * @see MappingI, LayerI1, InitializerCoarserMappingAdaptionF1, InitializerAreaConstrainedCoarserMappingAdaptionI1.
 * @ingroup cvsynthesis
 */
template <unsigned int tFactor>
class InitializerCoarserMappingAdaptionI1 :
	virtual public InitializerI,
	virtual public InitializerRandomized,
	virtual public InitializerSubset,
	virtual public Initializer1
{
	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param randomGenerator Random number generator
		 * @param coarserLayer The coarser synthesis layer from that the mapping will be adapted
		 */
		inline InitializerCoarserMappingAdaptionI1(LayerI1& layer, RandomGenerator& randomGenerator, const LayerI1& coarserLayer);

	private:

		/**
		 * Initializes a subset of the entire mapping area.
		 * @see InitializerSubset::initializeSubset().
		 */
		void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

	private:

		/// Coarser synthesis layer from that the mapping will be adapted.
		const LayerI1& coarserLayerI_;
};

template <unsigned int tFactor>
inline InitializerCoarserMappingAdaptionI1<tFactor>::InitializerCoarserMappingAdaptionI1(LayerI1& layer, RandomGenerator& randomGenerator, const LayerI1& coarserLayer) :
	Initializer(layer),
	InitializerI(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer),
	coarserLayerI_(coarserLayer)
{
	// nothing to do here
}

template <unsigned int tFactor>
void InitializerCoarserMappingAdaptionI1<tFactor>::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tFactor >= 2u, "Invalid factor!");

	const unsigned int width = layerI_.width();
	const unsigned int height = layerI_.height();

	const unsigned int coarserWidth = coarserLayerI_.width();
	const unsigned int coarserHeight = coarserLayerI_.height();

	ocean_assert(width / tFactor == coarserWidth);
	ocean_assert(height / tFactor == coarserHeight);

	MappingI& mapping = layerI_.mapping();
	const MappingI& coarserMapping = coarserLayerI_.mapping();

	RandomGenerator randomGenerator(randomGenerator_);

	const uint8_t* const mask = layerI_.mask().template constdata<uint8_t>();
	const uint8_t* const coarserMask = coarserLayerI_.mask().template constdata<uint8_t>();

	const unsigned int maskStrideElements = layerI_.mask().strideElements();
	const unsigned int coarserMaskStrideElements = coarserLayerI_.mask().strideElements();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* maskRow = mask + y * maskStrideElements;
		PixelPosition* positionRow = mapping.row(y);

		const unsigned int yCoarser = min(y / tFactor, coarserHeight - 1u);

		const uint8_t* coarserMaskRow = coarserMask + yCoarser * coarserMaskStrideElements;
		const PixelPosition* coarserPositionRow = coarserMapping.row(yCoarser);

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (maskRow[x] != 0xFFu)
			{
				const unsigned int xCoarser = min(x / tFactor, coarserWidth - 1u);

				// if the corresponding coarser layer pixel is a mask pixel
				if (coarserMaskRow[xCoarser] != 0xFFu)
				{
					const PixelPosition& coarserPosition = coarserPositionRow[xCoarser];
					ocean_assert(coarserPosition.x() < coarserLayerI_.width());
					ocean_assert(coarserPosition.y() < coarserLayerI_.height());

					const unsigned int candidateX = (unsigned int)(int(x) + (int(coarserPosition.x()) - int(xCoarser)) * int(tFactor));
					const unsigned int candidateY = (unsigned int)(int(y) + (int(coarserPosition.y()) - int(yCoarser)) * int(tFactor));

					ocean_assert(candidateX < width);
					ocean_assert(candidateY < height);

					if (mask[candidateY * maskStrideElements + candidateX] == 0xFFu)
					{
						positionRow[x] = CV::PixelPosition(candidateX, candidateY);
						continue;
					}
				}

				unsigned int candidateX, candidateY;
				do
				{
					candidateX = RandomI::random(randomGenerator, width - 1u);
					candidateY = RandomI::random(randomGenerator, height - 1u);
				}
				while (mask[candidateY * maskStrideElements + candidateX] != 0xFFu);

				positionRow[x] = CV::PixelPosition(candidateX, candidateY);
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_I_1_H
