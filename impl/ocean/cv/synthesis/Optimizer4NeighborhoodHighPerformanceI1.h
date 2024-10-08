/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_4_NEIGHBORHOOD_HIGH_PERFORMANCE_I_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_4_NEIGHBORHOOD_HIGH_PERFORMANCE_I_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerI1.h"
#include "ocean/cv/synthesis/OptimizerI.h"
#include "ocean/cv/synthesis/OptimizerSubset.h"
#include "ocean/cv/synthesis/Optimizer1.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a high performance mapping optimizer for integer mappings that use one single frame.
 * @tparam tWeightFactor Spatial weight impact, with range [0, infinity)
 * @tparam tBorderFactor Weight factor of border pixels, with range [1, infinity)
 * @tparam tUpdateFrame True, to update the frame pixel whenever a new mapping has been found
 * @ingroup cvsynthesis
 */
template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
class Optimizer4NeighborhoodHighPerformanceI1 :
	virtual public OptimizerI,
	virtual public OptimizerSubset,
	virtual public Optimizer1
{
	public:

		/**
		 * Creates a new optimizer object.
		 * @param layer The layer to be optimized
		 * @param randomGenerator Random number generator
		 */
		inline Optimizer4NeighborhoodHighPerformanceI1(LayerI1& layer, RandomGenerator& randomGenerator);

	private:

		/**
		 * Optimizes a subset of the synthesis frame.
		 * @see Optimizer1::optimizeSubset().
		 * @see optimizerSubsetChannels().
		 */
		void optimizeSubset(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const override;

		/**
		 * Specialization of the default subset optimization function.
		 * The template parameters specified the number of channels the synthesis frame has.<br>
		 * @param radii Number of improvement radii during one optimization iteration for each mapping position
		 * @param maxSpatialCost Maximal spatial cost
		 * @param boundingBoxTop First row of the entire synthesis area
		 * @param boundingBoxHeight Number of rows of the entire synthesis area
		 * @param downIsMain True, if the downwards direction is the main optimization direction (for all subsets with even thread indices)
		 * @param firstColumn First column to be handled in the subset
		 * @param numberColumns Number of columns to be handled in the subset
		 * @param rowOffset Offset within the entire synthesis area (boundingBoxHeight), the subset may be moved by this offset
		 * @param firstRow First row to be handled in the subset
		 * @param numberRows Number of rows to be handled in the subset
		 * @param threadIndex Index of the thread that executes the subset optimization function
		 * @tparam tChannels Number of data channels of the frame
		 * @see optimizerSubset().
		 */
		template <unsigned int tChannels>
		void optimizeSubsetChannels(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const;

	protected:

		/// Specialized layer reference.
		LayerI1& layerI1_;
};

template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
inline Optimizer4NeighborhoodHighPerformanceI1<tWeightFactor, tBorderFactor, tUpdateFrame>::Optimizer4NeighborhoodHighPerformanceI1(LayerI1& layer, RandomGenerator& randomGenerator) :
	Optimizer(layer),
	OptimizerI(layer),
	OptimizerSubset(layer, randomGenerator),
	Optimizer1(layer),
	layerI1_(layer)
{
	// nothing to do here
}

template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
void Optimizer4NeighborhoodHighPerformanceI1<tWeightFactor, tBorderFactor, tUpdateFrame>::optimizeSubset(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const
{
	ocean_assert(layerI1_.frame().numberPlanes() == 1u);

	switch (layerI1_.frame().channels())
	{
		case 1u:
			optimizeSubsetChannels<1u>(radii, maxSpatialCost, boundingBoxTop, boundingBoxHeight, downIsMain, firstColumn, numberColumns, rowOffset, firstRow, numberRows, threadIndex);
			break;

		case 2u:
			optimizeSubsetChannels<2u>(radii, maxSpatialCost, boundingBoxTop, boundingBoxHeight, downIsMain, firstColumn, numberColumns, rowOffset, firstRow, numberRows, threadIndex);
			break;

		case 3u:
			optimizeSubsetChannels<3u>(radii, maxSpatialCost, boundingBoxTop, boundingBoxHeight, downIsMain, firstColumn, numberColumns, rowOffset, firstRow, numberRows, threadIndex);
			break;

		case 4u:
			optimizeSubsetChannels<4u>(radii, maxSpatialCost, boundingBoxTop, boundingBoxHeight, downIsMain, firstColumn, numberColumns, rowOffset, firstRow, numberRows, threadIndex);
			break;

		default:
			ocean_assert(false && "Invalid frame type.");
	}
}

template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
template <unsigned int tChannels>
void Optimizer4NeighborhoodHighPerformanceI1<tWeightFactor, tBorderFactor, tUpdateFrame>::optimizeSubsetChannels(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const
{
	const unsigned int layerWidth = layerI1_.width();
	const unsigned int layerHeight = layerI1_.height();
	ocean_assert(layerWidth != 0 && layerHeight != 0);

	const std::vector<int> searchRadii(calculateSearchRadii(radii, layerWidth, layerHeight));

	Frame& layerFrame = layerI1_.frame();
	const Frame& layerMask = layerI1_.mask();
	MappingI1& layerMapping = layerI1_.mapping();

	ocean_assert(FrameType::formatIsGeneric(layerFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));
	ocean_assert(layerFrame.pixelOrigin() == layerMask.pixelOrigin());

	ocean_assert(firstColumn + numberColumns <= layerFrame.width());
	ocean_assert(firstRow + numberRows <= layerFrame.height());

	RandomGenerator generator(randomGenerator_);

	uint8_t* const layerFrameData = layerFrame.data<uint8_t>();
	const uint8_t* const layerMaskData = layerMask.constdata<uint8_t>();

	const unsigned int layerFramePaddingElements = layerFrame.paddingElements();
	const unsigned int layerMaskPaddingElements = layerMask.paddingElements();
	const unsigned int layerMaskStrideElements = layerMask.strideElements();

#ifdef OCEAN_DEBUG
	const PixelBoundingBox& debugLayerBoundingBox = layerI1_.boundingBox();
	ocean_assert(!debugLayerBoundingBox || firstRow >= debugLayerBoundingBox.top());
	ocean_assert(!debugLayerBoundingBox || firstRow + numberRows <= debugLayerBoundingBox.bottomEnd());
#endif // OCEAN_DEBUG

	const bool down = (downIsMain && (threadIndex % 2u) == 0u) || (!downIsMain && (threadIndex % 2u) == 1u);

	const unsigned int xStart = firstColumn;
	const unsigned int yStart = firstRow;
	const unsigned int xEnd = firstColumn + numberColumns;
	const unsigned int yEnd = firstRow + numberRows;

	ocean_assert(xEnd - xStart <= layerWidth);
	ocean_assert(yEnd - yStart <= layerHeight);

	if (down)
	{
		// find better positions for each mask pixel (top left to bottom right)
		for (unsigned int yy = yStart; yy < yEnd; ++yy)
		{
			const unsigned int y = modulo(int(yy + rowOffset - boundingBoxTop), int(boundingBoxHeight)) + boundingBoxTop;

			const uint8_t* maskRow = layerMask.constrow<uint8_t>(y) + xStart;
			PixelPosition* positionRow = layerMapping.row(y) + xStart;

			for (unsigned int x = xStart; x < xEnd; ++x)
			{
				bool foundBetter = false;

				ocean_assert(maskRow == layerMask.constpixel<uint8_t>(x, y));
				if (*maskRow != 0xFF)
				{
					unsigned int newPositionX = positionRow->x();
					unsigned int newPositionY = positionRow->y();

					const uint64_t oldSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, newPositionX, newPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost);
					const uint64_t oldColorCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, newPositionX, newPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);
					uint64_t newCost = uint64_t(tWeightFactor) * oldSpatialCost + oldColorCost;

					unsigned int testPositionX, testPositionY;

					// first propagation from left to right
					ocean_assert(x == 0u || (maskRow - 1) == layerMask.constpixel<uint8_t>(x - 1u, y));
					if (x > 0u && *(maskRow - 1) != 0xFFu)
					{
						ocean_assert(layerMapping.position(x - 1, y));
						ocean_assert(*(positionRow - 1) == layerMapping.position(x - 1, y));

						// take the position to the left (of the current position)
						testPositionX = (positionRow - 1)->x() + 1;
						testPositionY = (positionRow - 1)->y();

						if (testPositionX < layerWidth && layerMaskData[testPositionY * layerMaskStrideElements + testPositionX] == 0xFF)
						{
							// the structure cost is 0 due to the neighbor condition
							ocean_assert_accuracy(layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost) == 0u);

							const uint64_t testCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

							if (testCost < newCost)
							{
								newPositionX = testPositionX;
								newPositionY = testPositionY;
								newCost = testCost;
								foundBetter = true;
							}
						}

						// second propagation from top to bottom
						ocean_assert(y == 0u || (maskRow - layerMaskStrideElements) == layerMask.constpixel<uint8_t>(x, y - 1u));
						if (y > 0u && *(maskRow - layerMaskStrideElements) != 0xFFu
							// test only if the mapping of the left position does not match (shifted) to the mapping of the top position
							&& (positionRow - 1)->northEast() != *(positionRow - layerWidth))
						{
							ocean_assert(layerMapping.position(x, y - 1));
							ocean_assert(*(positionRow - layerWidth) == layerMapping.position(x, y - 1));

							// take the next position to the top (of the current position)
							testPositionX = (positionRow - layerWidth)->x();
							testPositionY = (positionRow - layerWidth)->y() + 1;

							if (testPositionY < layerHeight && layerMaskData[testPositionY * layerMaskStrideElements + testPositionX] == 0xFF)
							{
								// the structure cost is 0 due to the neighbor condition
								ocean_assert_accuracy(layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost) == 0u);

								const uint64_t testCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

								if (testCost < newCost)
								{
									newPositionX = testPositionX;
									newPositionY = testPositionY;
									newCost = testCost;
									foundBetter = true;
								}
							}
						}
					}
					else
					{
						// second propagation from top to bottom
						ocean_assert(y == 0u || (maskRow - layerMaskStrideElements) == layerMask.constpixel<uint8_t>(x, y - 1u));
						if (y > 0u && *(maskRow - layerMaskStrideElements) != 0xFFu)
						{
							ocean_assert(layerMapping.position(x, y - 1));
							ocean_assert(*(positionRow - layerWidth) == layerMapping.position(x, y - 1));

							// take the next position to the top (of the current position)
							testPositionX = (positionRow - layerWidth)->x();
							testPositionY = (positionRow - layerWidth)->y() + 1;

							if (testPositionY < layerHeight && layerMaskData[testPositionY * layerMaskStrideElements + testPositionX] == 0xFF)
							{
								// the structure cost is 0 due to the neighbor condition
								ocean_assert_accuracy(layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost) == 0u);

								const uint64_t testCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

								if (testCost < newCost)
								{
									newPositionX = testPositionX;
									newPositionY = testPositionY;
									newCost = testCost;
									foundBetter = true;
								}
							}
						}
					}

					// find a better position of the current mask pixel
					for (unsigned int n = 0; n < radii; ++n)
					{
						ocean_assert(newPositionX != (unsigned int)(-1) && newPositionY != (unsigned int)(-1));

						testPositionX = newPositionX + RandomI::random(generator, -searchRadii[n], searchRadii[n]);
						testPositionY = newPositionY + RandomI::random(generator, -searchRadii[n], searchRadii[n]);

						// the test position must lie inside the mask
						if ((testPositionX == newPositionX && testPositionY == newPositionY)
								|| testPositionX >= layerWidth || testPositionY >= layerHeight
								|| layerMaskData[testPositionY * layerMaskStrideElements + testPositionX] != 0xFF)
							continue;

						const uint64_t testSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost);
						const uint64_t testColorCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

						const uint64_t testCost = uint64_t(tWeightFactor) * testSpatialCost + testColorCost;

						if (testCost < newCost)
						{
							newPositionX = testPositionX;
							newPositionY = testPositionY;
							newCost = testCost;
							foundBetter = true;
						}
					}

					if (tUpdateFrame && foundBetter)
					{
						ocean_assert(layerMaskData[y * layerWidth + x] != 0xFF);
						ocean_assert(layerMaskData[newPositionY * layerMaskStrideElements + newPositionX] == 0xFF);

						positionRow->setPosition(newPositionX, newPositionY);

						CV::CVUtilities::copyPixel<tChannels>(layerFrameData, layerFrameData, x, y, newPositionX, newPositionY, layerWidth, layerWidth, layerFramePaddingElements, layerFramePaddingElements);
					}
				}

				++maskRow;
				++positionRow;
			}
		}
	}
	else // up
	{
		// find better positions for each mask pixel (bottom right to top left)
		for (unsigned int yy = yEnd - 1u; yy != yStart - 1u; --yy)
		{
			const unsigned int y = modulo(int(yy + rowOffset - boundingBoxTop), int(boundingBoxHeight)) + boundingBoxTop;

			const uint8_t* maskRow = layerMask.constrow<uint8_t>(y) + xEnd - 1u;
			PixelPosition* positionRow = layerMapping.row(y) + xEnd - 1u;

			for (unsigned int x = xEnd - 1u; x != xStart - 1u; --x)
			{
				bool foundBetter = false;

				ocean_assert(maskRow == layerMask.constpixel<uint8_t>(x, y));
				if (*maskRow != 0xFF)
				{
					unsigned int newPositionX = positionRow->x();
					unsigned int newPositionY = positionRow->y();

					const uint64_t oldSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, newPositionX, newPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost);
					const uint64_t oldColorCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, newPositionX, newPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);
					uint64_t newCost = uint64_t(tWeightFactor) * oldSpatialCost + oldColorCost;

					unsigned int testPositionX, testPositionY;

					// first propagation from right to left
					ocean_assert(x == layerWidth - 1u || (maskRow + 1) == layerMask.constpixel<uint8_t>(x + 1u, y));
					if (x < layerWidth - 1u && *(maskRow + 1) != 0xFFu)
					{
						ocean_assert(layerMapping.position(x + 1, y));
						ocean_assert(*(positionRow + 1) == layerMapping.position(x + 1, y));

						// take the position to the right (of the current position)
						testPositionX = (positionRow + 1)->x() - 1;
						testPositionY = (positionRow + 1)->y();

						if (testPositionX != (unsigned int)(-1) && layerMaskData[testPositionY * layerWidth + testPositionX] == 0xFF)
						{
							// the structure cost is 0 due to the neighbor condition
							ocean_assert_accuracy(layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost) == 0u);

							const uint64_t testCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

							if (testCost < newCost)
							{
								newPositionX = testPositionX;
								newPositionY = testPositionY;
								newCost = testCost;
								foundBetter = true;
							}
						}

						// second propagation from bottom to top
						ocean_assert(y == layerHeight - 1u || (maskRow + layerMaskStrideElements) == layerMask.constpixel<uint8_t>(x, y + 1u));
						if (y < layerHeight - 1u && *(maskRow + layerMaskStrideElements) != 0xFFu
							// test only if the mapping of the right position does not match (shifted) to the mapping of the bottom position
							&& (positionRow + 1)->southWest() != *(positionRow + layerWidth))
						{
							ocean_assert(layerMapping.position(x, y + 1));
							ocean_assert(*(positionRow + layerWidth) == layerMapping.position(x, y + 1));

							// take the next position towards the bottom (of the current position)
							testPositionX = (positionRow + layerWidth)->x();
							testPositionY = (positionRow + layerWidth)->y() - 1;

							if (testPositionY != (unsigned int)(-1) && layerMaskData[testPositionY * layerWidth + testPositionX] == 0xFF)
							{
								// the structure cost is 0 due to the neighbor condition
								ocean_assert_accuracy(layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost) == 0u);

								const uint64_t testCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

								if (testCost < newCost)
								{
									newPositionX = testPositionX;
									newPositionY = testPositionY;
									newCost = testCost;
									foundBetter = true;
								}
							}
						}
					}
					else
					{
						// second propagation from bottom to top
						ocean_assert(y == layerHeight - 1u || (maskRow + layerMaskStrideElements) == layerMask.constpixel<uint8_t>(x, y + 1u));
						if (y < layerHeight - 1u && *(maskRow + layerMaskStrideElements) != 0xFFu)
						{
							ocean_assert(layerMapping.position(x, y + 1));
							ocean_assert(*(positionRow + layerWidth) == layerMapping.position(x, y + 1));

							// take the next position towards the bottom (of the current position)
							testPositionX = (positionRow + layerWidth)->x();
							testPositionY = (positionRow + layerWidth)->y() - 1;

							if (testPositionY != (unsigned int)(-1) && layerMaskData[testPositionY * layerWidth + testPositionX] == 0xFF)
							{
								// the structure cost is 0 due to the neighbor condition
								ocean_assert_accuracy(layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost) == 0u);

								const uint64_t testCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);

								if (testCost < newCost)
								{
									newPositionX = testPositionX;
									newPositionY = testPositionY;
									newCost = testCost;
									foundBetter = true;
								}
							}
						}
					}

					// find a better position of the current mask pixel
					for (unsigned int n = 0; n < radii; ++n)
					{
						ocean_assert(newPositionX != (unsigned int)(-1) && newPositionY != (unsigned int)(-1));

						testPositionX = newPositionX + RandomI::random(generator, -searchRadii[n], searchRadii[n]);
						testPositionY = newPositionY + RandomI::random(generator, -searchRadii[n], searchRadii[n]);

						if ((testPositionX == newPositionX && testPositionY == newPositionY)
								|| testPositionX >= layerWidth || testPositionY >= layerHeight
								|| layerMaskData[testPositionY * layerWidth + testPositionX] != 0xFF)
							continue;

						const uint64_t testSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, maxSpatialCost);
						const uint64_t testColorCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements);
						const uint64_t testCost = uint64_t(tWeightFactor) * testSpatialCost + testColorCost;

						if (testCost < newCost)
						{
							newPositionX = testPositionX;
							newPositionY = testPositionY;
							newCost = testCost;
							foundBetter = true;
						}
					}

					if (tUpdateFrame && foundBetter)
					{
						ocean_assert(layerMaskData[y * layerWidth + x] != 0xFF);
						ocean_assert(layerMaskData[newPositionY * layerWidth + newPositionX] == 0xFF);

						positionRow->setPosition(newPositionX, newPositionY);

						CV::CVUtilities::copyPixel<tChannels>(layerFrameData, layerFrameData, x, y, newPositionX, newPositionY, layerWidth, layerWidth, layerFramePaddingElements, layerFramePaddingElements);
					}
				}

				--maskRow;
				--positionRow;
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_4_NEIGHBORHOOD_HIGH_PERFORMANCE_I_H
