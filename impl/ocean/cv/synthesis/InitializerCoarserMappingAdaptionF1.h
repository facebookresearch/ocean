// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
 * @tparam tFactor Defines the dimension increase factor between the synthesis layer and the given coarser layer. A factor of 2 means that the width and height of the synthesis layer is two times larger than the width and height of the given coarser layer.
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
		const LayerF1& coarserLayer_;
};

template <unsigned int tFactor>
inline InitializerCoarserMappingAdaptionF1<tFactor>::InitializerCoarserMappingAdaptionF1(LayerF1& layer, RandomGenerator& randomGenerator, const LayerF1& coarserLayer) :
	Initializer(layer),
	InitializerF(layer),
	InitializerRandomized(layer, randomGenerator),
	InitializerSubset(layer),
	Initializer1(layer),
	coarserLayer_(coarserLayer)
{
	// nothing to do here
}

template <unsigned int tFactor>
void InitializerCoarserMappingAdaptionF1<tFactor>::initializeSubset(const unsigned int /*firstColumn*/, const unsigned int /*numberColumns*/, const unsigned int /*firstRow*/, const unsigned int /*numberRows*/) const
{
	static_assert(oceanFalse<tFactor>(), "Not implemented for this increase factor!");
}

// **TODO** validate
template <>
inline void InitializerCoarserMappingAdaptionF1<2u>::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	const unsigned int layerWidth = layerF_.width();
	const unsigned int layerHeight = layerF_.height();

	ocean_assert(layerWidth / 2u == coarserLayer_.width());
	ocean_assert(layerHeight / 2u == coarserLayer_.height());

	MappingF& layerMapping = layerF_.mapping();
	const MappingF1& coarserLayerMapping = coarserLayer_.mapping();

	const uint8_t* const layerMaskData = layerF_.legacyMask().constdata<uint8_t>();
	const uint8_t* const coarserLayerMaskData = coarserLayer_.legacyMask().constdata<uint8_t>();

	const unsigned int coarserLayerWidth = coarserLayer_.width();
	const unsigned int coarserLayerHeight = coarserLayer_.height();

	RandomGenerator randomGenerator(randomGenerator_);

	const unsigned int xStart = firstColumn;
	const unsigned int yStart = firstRow;
	const unsigned int xEnd = firstColumn + numberColumns;
	const unsigned int yEnd = firstRow + numberRows;

	for (unsigned int y = yStart; y < yEnd; ++y)
	{
		const unsigned int yLow = min(y >> 1u, coarserLayerHeight - 1u);
		ocean_assert(yLow < coarserLayerHeight);

		const uint8_t* layerPointer = layerMaskData + y * layerWidth + xStart - 1;
		const uint8_t* const coarserLayerPointerRow = coarserLayerMaskData + yLow * coarserLayerWidth;

		Vector2* pixelPosition = layerMapping() + y * layerWidth + xStart;
		const Vector2* const coarserPixelPositionRow = coarserLayerMapping() + yLow * coarserLayerWidth;

		for (unsigned int x = xStart; x < xEnd; ++x)
		{
			ocean_assert(*(layerPointer + 1) == layerMaskData[y * layerWidth + x]);

			// if the high layer pixel is a mask pixel
			if (*++layerPointer != 0xFF)
			{
				const unsigned int xLow = min(x >> 1u, coarserLayerWidth - 1);
				ocean_assert(xLow < coarserLayerWidth);

				// if the corresponding low layer pixel is a mask pixel
				ocean_assert_and_suppress_unused(coarserLayerPointerRow[xLow] != 0xFF, coarserLayerPointerRow);

				const Vector2& positionLow = coarserPixelPositionRow[xLow];
				ocean_assert(positionLow.x() < Scalar(coarserLayerWidth));
				ocean_assert(positionLow.y() < Scalar(coarserLayerHeight));

				const Scalar highAbsoluteX = Scalar(x) + (positionLow.x() - Scalar(xLow)) * 2;
				const Scalar highAbsoluteY = Scalar(y) + (positionLow.y() - Scalar(yLow)) * 2;

				ocean_assert(highAbsoluteX >= 0 && highAbsoluteX < Scalar(int(layerWidth)));
				ocean_assert(highAbsoluteY >= 0 && highAbsoluteY < Scalar(int(layerHeight)));

				if (layerMaskData[Numeric::round32(highAbsoluteY) * layerWidth + Numeric::round32(highAbsoluteX)] == 0xFF)
				{
					ocean_assert(layerMaskData[Numeric::round32(highAbsoluteY) * layerWidth + Numeric::round32(highAbsoluteX)] == 0xFF);

					ocean_assert(pixelPosition == &layerMapping.position(x, y));
					*pixelPosition = Vector2(highAbsoluteX, highAbsoluteY);
				}
				else
				{
					while (true)
					{
						const Scalar localHighAbsoluteX = Random::scalar(randomGenerator, Scalar(2u), Scalar(layerWidth - 4u));
						const Scalar localHighAbsoluteY = Random::scalar(randomGenerator, Scalar(2u), Scalar(layerHeight - 4u));

						if (layerMaskData[Numeric::round32(localHighAbsoluteY) * layerWidth + Numeric::round32(localHighAbsoluteX)] == 0xFF)
						{
							ocean_assert(pixelPosition == &layerMapping.position(x, y));
							*pixelPosition = Vector2(localHighAbsoluteX, localHighAbsoluteY);
							break;
						}
					}
				}
			}

			++pixelPosition;
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_COARSER_MAPPING_ADAPTION_F_1_H
