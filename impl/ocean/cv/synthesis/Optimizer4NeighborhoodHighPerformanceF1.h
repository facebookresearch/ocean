/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPTIMIZER_4_NEIGHBORHOOD_HIGH_PERFORMANCE_F_1_H
#define META_OCEAN_CV_SYNTHESIS_OPTIMIZER_4_NEIGHBORHOOD_HIGH_PERFORMANCE_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerF1.h"
#include "ocean/cv/synthesis/OptimizerF.h"
#include "ocean/cv/synthesis/OptimizerSubset.h"
#include "ocean/cv/synthesis/Optimizer1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a high performance mapping optimizer for float mappings that use one single frame.
 * @tparam tWeightFactor Spatial weight impact, with range [0, infinity)
 * @tparam tBorderFactor Weight factor of border pixels, with range [1, infinity)
 * @tparam tUpdateFrame True, to update the frame pixel whenever a new mapping has been found
 * @ingroup cvsynthesis
 */
template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
class Optimizer4NeighborhoodHighPerformanceF1 :
	virtual public OptimizerF,
	virtual public OptimizerSubset,
	virtual public Optimizer1
{
	public:

		/**
		 * Creates a new optimizer object.
		 * @param layer Synthesis layer that will be optimized
		 * @param randomGenerator Random number generator
		 */
		inline Optimizer4NeighborhoodHighPerformanceF1(LayerF1& layer, RandomGenerator& randomGenerator);

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
		LayerF1& layerF1_;
};

template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
inline Optimizer4NeighborhoodHighPerformanceF1<tWeightFactor, tBorderFactor, tUpdateFrame>::Optimizer4NeighborhoodHighPerformanceF1(LayerF1& layer, RandomGenerator& randomGenerator) :
	Optimizer(layer),
	OptimizerF(layer),
	OptimizerSubset(layer, randomGenerator),
	Optimizer1(layer),
	layerF1_(layer)
{
	//  nothing to do here
}

template <unsigned int tWeightFactor, unsigned int tBorderFactor, bool tUpdateFrame>
void Optimizer4NeighborhoodHighPerformanceF1<tWeightFactor, tBorderFactor, tUpdateFrame>::optimizeSubset(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const
{
	ocean_assert(layerF1_.frame().numberPlanes() == 1u);

	switch (layerF1_.frame().channels())
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
void Optimizer4NeighborhoodHighPerformanceF1<tWeightFactor, tBorderFactor, tUpdateFrame>::optimizeSubsetChannels(const unsigned int radii, const unsigned int maxSpatialCost, const unsigned int boundingBoxTop, const unsigned int boundingBoxHeight, const bool downIsMain, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int rowOffset, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadIndex) const
{
	const unsigned int layerWidth = layerF1_.width();
	const unsigned int layerHeight = layerF1_.height();

	ocean_assert(layerWidth != 0u && layerHeight != 0u);

	const std::vector<Scalar> searchRadii(calculateSearchRadii(radii, layerWidth, layerHeight));

	Frame& layerFrame = layerF1_.frame();
	const Frame& layerMask = layerF1_.mask();
	MappingF1& layerMapping = layerF1_.mapping();

	ocean_assert(FrameType::formatIsGeneric(layerFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));
	ocean_assert(layerFrame.pixelOrigin() == layerMask.pixelOrigin());

	ocean_assert(firstColumn + numberColumns <= layerFrame.width());
	ocean_assert(firstRow + numberRows <= layerFrame.height());

	RandomGenerator generator(randomGenerator_);

	uint8_t* const layerFrameData = layerFrame.data<uint8_t>();
	const uint8_t* const layerMaskData = layerMask.constdata<uint8_t>();

	const unsigned int layerFramePaddingElements = layerFrame.paddingElements();
	const unsigned int layerFrameStrideElements = layerFrame.strideElements();

	const unsigned int layerMaskPaddingElements = layerMask.paddingElements();
	const unsigned int layerMaskStrideElements = layerMask.strideElements();

#ifdef OCEAN_DEBUG
	const PixelBoundingBox& debugLayerBoundingBox = layerF1_.boundingBox();
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
			Vector2* positionRow = layerMapping.row(y) + xStart;

			for (unsigned int x = xStart; x < xEnd; ++x)
			{
				bool foundBetter = false;

				ocean_assert(maskRow == layerMask.constpixel<uint8_t>(x, y));
				if (*maskRow != 0xFF)
				{
					Scalar newPositionX = positionRow->x();
					Scalar newPositionY = positionRow->y();

					const Scalar oldSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, newPositionX, newPositionY, layerMaskData, layerMaskPaddingElements, Scalar(maxSpatialCost));
					const unsigned int oldColorCost = layerMapping.appearanceCost5x5<tChannels>(x, y, newPositionX, newPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);
					Scalar newCost = Scalar(tWeightFactor) * oldSpatialCost + Scalar(oldColorCost);

					Scalar testPositionX, testPositionY;

					// first propagation from left to right
					ocean_assert(maskRow - 1 == layerMask.constpixel<uint8_t>(x - 1u, y));
					if (x > 0 && *(maskRow - 1) != 0xFF)
					{
						ocean_assert(layerMapping.position(x - 1, y).x() > 0);
						ocean_assert(*(positionRow - 1) == layerMapping.position(x - 1, y));

						// take the position to the left (of the current position)
						testPositionX = (positionRow - 1)->x() + 1;
						testPositionY = (positionRow - 1)->y();

						if (testPositionX < Scalar(layerWidth - 3u) && layerMaskData[Numeric::round32(testPositionY) * layerMaskStrideElements + Numeric::round32(testPositionX)] == 0xFF)
						{
							// the structure cost is 0 due to the neighbor condition
							const unsigned int testCost = layerMapping.appearanceCost5x5<tChannels>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);

							if (Scalar(testCost) < newCost)
							{
								newPositionX = testPositionX;
								newPositionY = testPositionY;
								newCost = Scalar(testCost);
								foundBetter = true;
							}
						}
					}

					// second propagation from top to bottom
					ocean_assert(maskRow - layerMaskStrideElements == layerMask.constpixel<uint8_t>(x, y - 1u));
					if (y > 0 && *(maskRow - layerMaskStrideElements) != 0xFF)
					{
						ocean_assert(layerMapping.position(x, y - 1).x() > 0);
						ocean_assert(*(positionRow - layerWidth) == layerMapping.position(x, y - 1));

						// take the next position to the top (of the current position)
						testPositionX = (positionRow - layerWidth)->x();
						testPositionY = (positionRow - layerWidth)->y() + 1;

						if (testPositionY < Scalar(layerHeight - 3u) && layerMaskData[Numeric::round32(testPositionY) * layerMaskStrideElements + Numeric::round32(testPositionX)] == 0xFF)
						{
							// the structure cost is 0 due to the neighbor condition
							const unsigned int testCost = layerMapping.appearanceCost5x5<tChannels>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);

							if (Scalar(testCost) < newCost)
							{
								newPositionX = testPositionX;
								newPositionY = testPositionY;
								newCost = Scalar(testCost);
								foundBetter = true;
							}
						}
					}

					// find a better position of the current mask pixel
					for (unsigned int n = 0; n < radii; ++n)
					{
						ocean_assert(newPositionX != -1 && newPositionY != -1);

						testPositionX = newPositionX + Random::scalar(generator, -searchRadii[n], searchRadii[n]);
						testPositionY = newPositionY + Random::scalar(generator, -searchRadii[n], searchRadii[n]);

						// the test position must lie inside the
						if ((testPositionX == newPositionX && testPositionY == newPositionY) || testPositionX < Scalar(2) || testPositionX >= Scalar(layerWidth - 3u)
								|| testPositionY < Scalar(2) || testPositionY >= Scalar(layerHeight - 3u)
								|| layerMaskData[Numeric::round32(testPositionY) * layerMaskStrideElements + Numeric::round32(testPositionX)] != 0xFF)
							continue;

						const Scalar testSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, Scalar(maxSpatialCost));
						const unsigned int testColorCost = layerMapping.appearanceCost5x5<tChannels>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);
						const Scalar testCost = Scalar(tWeightFactor) * testSpatialCost + Scalar(testColorCost);

						if (testCost < newCost)
						{
							newPositionX = testPositionX;
							newPositionY = testPositionY;
							newCost = Scalar(testCost);
							foundBetter = true;
						}
					}

					if (tUpdateFrame && foundBetter)
					{
						ocean_assert(layerMask.constpixel<uint8_t>(x, y)[0] != 0xFF);
						ocean_assert(layerMask.constpixel<uint8_t>(Numeric::round32(newPositionX), Numeric::round32(newPositionY))[0] == 0xFF);

						*positionRow = Vector2(newPositionX, newPositionY);

						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(layerFrameData, layerWidth, layerHeight, layerFramePaddingElements, Vector2(newPositionX, newPositionY), layerFrameData + y * layerFrameStrideElements + x * tChannels);
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
			Vector2* positionRow = layerMapping.row(y) + xEnd - 1u;

			for (unsigned int x = xEnd - 1u; x != xStart - 1u ; --x)
			{
				bool foundBetter = false;

				ocean_assert(maskRow == layerMask.constpixel<uint8_t>(x, y));
				if (*maskRow != 0xFF)
				{
					Scalar newPositionX = positionRow->x();
					Scalar newPositionY = positionRow->y();

					const Scalar oldSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, newPositionX, newPositionY, layerMaskData, layerMaskPaddingElements, Scalar(maxSpatialCost));
					const unsigned int oldColorCost = layerMapping.appearanceCost5x5<tChannels>(x, y, newPositionX, newPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);
					Scalar newCost = Scalar(tWeightFactor) * oldSpatialCost + Scalar(oldColorCost);

					Scalar testPositionX, testPositionY;

					// first propagation from right to left
					ocean_assert(maskRow + 1 == layerMask.constpixel<uint8_t>(x + 1u, y));
					if (x < layerWidth - 1 && *(maskRow + 1) != 0xFF)
					{
						ocean_assert(layerMapping.position(x + 1, y).x() > 0);
						ocean_assert(*(positionRow + 1) == layerMapping.position(x + 1, y));

						// take the position to the right (of the current position)
						testPositionX = (positionRow + 1)->x() - 1;
						testPositionY = (positionRow + 1)->y();

						if (testPositionX >= Scalar(2) && layerMaskData[Numeric::round32(testPositionY) * layerMaskStrideElements + Numeric::round32(testPositionX)] == 0xFF)
						{
							// the structure cost is 0 due to the neighbor condition
							const unsigned int testCost = layerMapping.appearanceCost5x5<tChannels>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);

							if (Scalar(testCost) < newCost)
							{
								newPositionX = testPositionX;
								newPositionY = testPositionY;
								newCost = Scalar(testCost);
								foundBetter = true;
							}
						}
					}

					// second propagation from bottom to top
					ocean_assert(maskRow + layerMaskStrideElements == layerMask.constpixel<uint8_t>(x, y + 1u));
					if (y < layerHeight - 1 && *(maskRow + layerMaskStrideElements) != 0xFF)
					{
						ocean_assert(layerMapping.position(x, y + 1).x() > 0);
						ocean_assert(*(positionRow + layerWidth) == layerMapping.position(x, y + 1));

						// take the next position towards the bottom (of the current position)
						testPositionX = (positionRow + layerWidth)->x();
						testPositionY = (positionRow + layerWidth)->y() - 1;

						if (testPositionY >= Scalar(2) && layerMaskData[Numeric::round32(testPositionY) * layerMaskStrideElements + Numeric::round32(testPositionX)] == 0xFF)
						{
							// the structure cost is 0 due to the neighbor condition
							const unsigned int testCost = layerMapping.appearanceCost5x5<tChannels>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);

							if (Scalar(testCost) < newCost)
							{
								newPositionX = testPositionX;
								newPositionY = testPositionY;
								newCost = Scalar(testCost);
								foundBetter = true;
							}
						}
					}

					// find a better position of the current mask pixel
					for (unsigned int n = 0; n < radii; ++n)
					{
						ocean_assert(newPositionX != -1 && newPositionY != -1);

						testPositionX = newPositionX + Random::scalar(generator, -searchRadii[n], searchRadii[n]);
						testPositionY = newPositionY + Random::scalar(generator, -searchRadii[n], searchRadii[n]);

						if ((testPositionX == newPositionX && testPositionY == newPositionY) || testPositionX < Scalar(2) || testPositionX >= Scalar(layerWidth - 3u)
								|| testPositionY < Scalar(2) || testPositionY >= Scalar(layerHeight - 3u)
								|| layerMaskData[Numeric::round32(testPositionY) * layerMaskStrideElements + Numeric::round32(testPositionX)] != 0xFF)
							continue;

						const Scalar testSpatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, testPositionX, testPositionY, layerMaskData, layerMaskPaddingElements, Scalar(maxSpatialCost));
						const unsigned int testColorCost = layerMapping.appearanceCost5x5<tChannels>(x, y, testPositionX, testPositionY, layerFrameData, layerMaskData, layerFramePaddingElements, layerMaskPaddingElements, tBorderFactor);
						const Scalar testCost = Scalar(tWeightFactor) * testSpatialCost + Scalar(testColorCost);

						if (testCost < newCost)
						{
							newPositionX = testPositionX;
							newPositionY = testPositionY;
							newCost = Scalar(testCost);
							foundBetter = true;
						}
					}

					if (tUpdateFrame && foundBetter)
					{
						ocean_assert(layerMask.constpixel<uint8_t>(x, y)[0] != 0xFF);
						ocean_assert(layerMask.constpixel<uint8_t>(Numeric::round32(newPositionX), Numeric::round32(newPositionY))[0] == 0xFF);

						*positionRow = Vector2(newPositionX, newPositionY);

						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(layerFrameData, layerWidth, layerHeight, layerFramePaddingElements, Vector2(newPositionX, newPositionY), layerFrameData + y * layerFrameStrideElements + x * tChannels);
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

#endif // META_OCEAN_CV_SYNTHESIS_OPTIMIZER_4_NEIGHBORHOOD_HIGH_PERFORMANCE_F_1_H
