/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_F_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerF.h"
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
 * The initializer supports mapping with float accuracy.<br>
 * The coarser mapping is upsampled and adjusted to the synthesis mask.<br>
 * @tparam tFactor Defines the dimension increase factor between the synthesis layer and the given coarser layer. A factor of 2 means that the width and height of the synthesis layer is two times larger than the width and height of the given coarser layer, with range [0, infinity)
 * @see MappingF, LayerF1, InitializerCoarserMappingAdaptionI1.
 * @ingroup cvsynthesis
 */
template <unsigned int tFactor>
class InitializerCoarserMappingAdaptionF1 :
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
		 * @param coarserLayer The coarser synthesis layer from the mapping will be adopted
		 */
		inline InitializerCoarserMappingAdaptionF1(LayerF1& layer, RandomGenerator& randomGenerator, const LayerF1& coarserLayer);

	private:

		/**
		 * Initializes a subset of the entire mapping area.
		 * @see InitializerSubset::initializeSubset().
		 * @see initializeSubsetChannels().
		 */
		void initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

	private:

		/// Coarser layer that has to be adopted.
		const LayerF1& coarserLayerF_;
};

template <unsigned int tFactor>
inline InitializerCoarserMappingAdaptionF1<tFactor>::InitializerCoarserMappingAdaptionF1(LayerF1& layer, RandomGenerator& randomGenerator, const LayerF1& coarserLayer) :
	Initializer(layer),
	InitializerF(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer),
	coarserLayerF_(coarserLayer)
{
	// nothing to do here
}

template <unsigned int tFactor>
inline void InitializerCoarserMappingAdaptionF1<tFactor>::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tFactor >= 2u, "Invalid factor!");

	const unsigned int width = layerF_.width();
	const unsigned int height = layerF_.height();

	const unsigned int coarserWidth = coarserLayerF_.width();
	const unsigned int coarserHeight = coarserLayerF_.height();

	ocean_assert(width / tFactor == coarserWidth);
	ocean_assert(height / tFactor == coarserHeight);

	MappingF& mapping = layerF_.mapping();
	const MappingF& coarserMapping = coarserLayerF_.mapping();

	RandomGenerator randomGenerator(randomGenerator_);

	const uint8_t* const mask = layerF_.mask().template constdata<uint8_t>();
	const uint8_t* const coarserMask = coarserLayerF_.mask().template constdata<uint8_t>();

	const unsigned int maskStrideElements = layerF_.mask().strideElements();
	const unsigned int coarserMaskStrideElements = coarserLayerF_.mask().strideElements();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* maskRow = mask + y * maskStrideElements;
		Vector2* positionRow = mapping.row(y);

		const unsigned int yCoarser = min(y / tFactor, coarserHeight - 1u);

		const uint8_t* coarserMaskRow = coarserMask + yCoarser * coarserMaskStrideElements;
		const Vector2* coarserPositionRow = coarserMapping.row(yCoarser);

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (maskRow[x] != 0xFFu)
			{
				const unsigned int xCoarser = min(x / tFactor, coarserWidth - 1u);

				// if the corresponding coarser layer pixel is a mask pixel
				if (coarserMaskRow[xCoarser] != 0xFFu)
				{
					const Vector2& coarserPosition = coarserPositionRow[xCoarser];
					ocean_assert(coarserPosition.x() < Scalar(coarserLayerF_.width()));
					ocean_assert(coarserPosition.y() < Scalar(coarserLayerF_.height()));

					const Scalar candidateX = Scalar(x) + (coarserPosition.x() - Scalar(xCoarser)) * Scalar(tFactor);
					const Scalar candidateY = Scalar(y) + (coarserPosition.y() - Scalar(yCoarser)) * Scalar(tFactor);

					const unsigned int intCandidateX = (unsigned int)(Numeric::round32(candidateX));
					const unsigned int intCandidateY = (unsigned int)(Numeric::round32(candidateY));

					if (mask[intCandidateY * maskStrideElements + intCandidateX] == 0xFFu)
					{
						positionRow[x] = Vector2(candidateX, candidateY);
						continue;
					}
				}

				Vector2 candidate;

				while (true)
				{
					candidate = Random::vector2(randomGenerator, Scalar(2u), Scalar(width - 3u), Scalar(2u), Scalar(height - 3u));

					const unsigned int intCandidateX = (unsigned int)(Numeric::round32(candidate.x()));
					const unsigned int intCandidateY = (unsigned int)(Numeric::round32(candidate.y()));

					if (mask[intCandidateY * maskStrideElements + intCandidateX] == 0xFFu)
					{
						positionRow[x] = candidate;
						break;
					}
				}
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_F_1_H
