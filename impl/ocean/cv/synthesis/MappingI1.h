/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_MAPPING_I_1_H
#define META_OCEAN_CV_SYNTHESIS_MAPPING_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/MappingI.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/advanced/SumSquareDifferencesNoCenter.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the pixel mapping between source and target frames.
 *
 * Cost function:<br>
 *
 * pixelCost = spatialFactor * spatialCost + appearanceCost<br>
 *
 * spatialCost = spatialCost' / normalizationSpatialCost<br>
 * appearanceCost = appearanceCost' / normalizationAppearanceCost<br>
 *
 * spatialCost<br>
 * pixelCost = spatialFactor * spatialCost' / normalizationSpatialCost + appearanceCost' / normalizationAppearanceCost<br>
 *
 * pixelCost' = spatialFactor * spatialCost * normalizationAppearanceCost + appearanceCost * normalizationSpatialCost<br>
 *
 * 24bit:<br>
 * normalizationAppearanceCost = 3 * 255^2 * numberSamples<br>
 * normalizationSpatialCost = width^2 + height^2<br>
 *
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT MappingI1 : public MappingI
{
	public:

		/**
		 * Creates an empty mapping object.
		 */
		MappingI1();

		/**
		 * Copies a mapping from a given mapping object.
		 * @param pixelMapping Pixel mapping to be copied
		 */
		MappingI1(const MappingI1& pixelMapping);

		/**
		 * Move constructor.
		 * @param pixelMapping Mapping to be moved
		 */
		MappingI1(MappingI1&& pixelMapping) noexcept;

		/**
		 * Creates a new mapping object with defined dimension.
		 * An initial mapping is not provided.<br>
		 * @param width The width of the mapping object in pixel, with range [1, infinity)
		 * @param height The height of the mapping object in pixel, with range [1, infinity)
		 */
		MappingI1(const unsigned int width, const unsigned int height);

		/**
		 * Calculates the smallest/cheapest spatial cost for a given point in a four-neighborhood and normalizes the result according to the frame dimension.
		 * Instead of summing ob the cost for all neighboring pixels, this function determines the minimal cost of all neighboring pixels.
		 * @param xTarget Horizontal target position to determine the spatial cost for, with range [0, width - 1], and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the spatial cost for, with range [0, height - 1], and must lie inside the target mask
		 * @param xSource Corresponding horizontal source mapping position for the given position
		 * @param ySource Corresponding vertical source mapping position for the given position
		 * @param targetMask Mask separating target and source pixels for the given target position, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param maxCost The maximal cost the spatial cost can have, with range [1, infinity)
		 * @return Resulting spatial cost for the given points
		 * @tparam tChannels The number of data channels of the frame, with range [1, infinity)
		 * @see spatialCost8Neighborhood().
		 */
		template <unsigned int tChannels>
		inline unsigned int spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const;

		/**
		 * Calculates the smallest/cheapest spatial cost for a given point in a eight-neighborhood and normalizes the result according to the frame dimension.
		 * Instead of summing ob the cost for all neighboring pixels, this function determines the minimal cost of all neighboring pixels.
		 * @param xTarget Horizontal target position to determine the spatial cost for, with range [0, width) and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the spatial cost for, with range [0, height) and must lie inside the target mask
		 * @param xSource Corresponding horizontal source mapping position for the given position
		 * @param ySource Corresponding vertical source mapping position for the given position
		 * @param targetMask Mask separating target and source pixels for the given target position, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param maxCost The maximal cost the spatial cost can have, with range [1, infinity)
		 * @return Resulting spatial cost for the given points
		 * @tparam tChannels The number of data channels of the frame, with range [1, infinity)
		 * @see spatialCost8Neighborhood().
		 */
		template <unsigned int tChannels>
		inline unsigned int spatialCost8Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const;

		/**
		 * Calculates the appearance cost for a given point in a given frame.
		 * @param xTarget Horizontal target position to determine the appearance cost for, with range [0, width) and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the appearance cost for, with range [0, height) and must lie inside the target mask
		 * @param xSource Horizontal source position to determine the appearance cost for, with range [0, width) and must lie outside the target mask
		 * @param ySource Vertical source position to determine the appearance cost for, with range [0, height) and must lie outside the target mask
		 * @param frame The target and source frame to determine the appearance cost on, with same dimension as this mapping object
		 * @param mask The mask separating target and source pixels for the given positions, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number f padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @tparam tChannels The number of channels of the frame, with range [1, infinity)
		 * @tparam tBorderFactor Constant factor to weight the appearance cost of synthesis border pixels (border between target and source pixels) individually, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tBorderFactor>
		unsigned int appearanceCost5x5(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements) const;

		/**
		 * Applies the current mapping for one given frame.<br>
		 * @see Mapping::applyMapping().
		 */
		void applyMapping(Frame& frame, const Frame& mask, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker = nullptr) const override;

		/**
		 * Applies the current mapping for one given frame.<br>
		 * Only mask pixels will be updated in the frame while the specification of a bounding box in which the mapping will be applied is used to improve the performance of the execution.
		 * @param frame The frame holding source and target area
		 * @param mask The 8 bit mask defining source and target area with 0xFF defining a non-mask pixel, with same frame dimension and pixel origin as the provided frame
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param xStart Horizontal start position of the update area in pixel, with range [0, width())
		 * @param xWidth Width of the update area in pixel, with range [1, width() - xStart]
		 * @param yStart Vertical start position of the update area in pixel, with range [0, height())
		 * @param yHeight Height of the update area in pixel, with range [1, height() - yStart]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline void applyOneFrameMapping8BitPerChannel(uint8_t* const frame, const uint8_t* const mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker = nullptr) const;

		/**
		 * Assigns another pixel mapping object to this one.
		 * @param pixelMapping Pixel mapping object to be copied
		 * @return Reference to this object
		 */
		inline MappingI1& operator=(const MappingI1& pixelMapping);

		/**
		 * Move operator.
		 * @param pixelMapping Right mapping to moved
		 * @return Reference to this move
		 */
		inline MappingI1& operator=(MappingI1&& pixelMapping) noexcept;

	private:

		/**
		 * Calculates the smallest/cheapest spatial cost for a given point in a four-neighborhood.
		 * Instead of summing ob the cost for all neighboring pixels, this function determines the minimal cost of all neighboring pixels.
		 * @param xTarget Horizontal target position to determine the spatial cost for, with range [0, width) and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the spatial cost for, with range [0, height) and must lie inside the target mask
		 * @param xSource Corresponding horizontal source mapping position for the given position
		 * @param ySource Corresponding vertical source mapping position for the given position
		 * @param targetMask Mask separating target and source pixels for the given target position, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param maxCost The maximal cost the spatial cost can have, with range [1, infinity)
		 * @return Resulting spatial cost for the given points
		 */
		inline unsigned int spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const;

		/**
		 * Calculates the smallest/cheapest spatial cost for a given point in a eight-neighborhood.
		 * Instead of summing ob the cost for all neighboring pixels, this function determines the minimal cost of all neighboring pixels.
		 * @param xTarget Horizontal target position to determine the spatial cost for, with range [0, width) and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the spatial cost for, with range [0, height) and must lie inside the target mask
		 * @param xSource Corresponding horizontal source mapping position for the given position
		 * @param ySource Corresponding vertical source mapping position for the given position
		 * @param targetMask Mask separating target and source pixels for the given target position, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param maxCost The maximal cost the spatial cost can have, with range [1, infinity)
		 * @return Resulting spatial cost for the given points
		 */
		inline unsigned int spatialCost8Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const;

		/**
		 * Applies the current mapping in a subset of one given frame.<br>
		 * Only mask pixels will be updated in the frame while the specification of a bounding box in which the mapping will be applied is used to improve the performance of the execution.
		 * @param frame The frame holding source and target area, with frame dimension identical to width() x height()
		 * @param mask The 8 bit mask defining source and target area with 0xFF defining a non-mask pixel, with same frame dimension and pixel origin as the provided frame
		 * @param frameStrideElements The number of elements between two frame rows, in elements, with range [width * tChannels, infinity)
		 * @param maskStrideElements The number of elements between two frame rows, in elements, with range [width, infinity)
		 * @param xStart Horizontal start position of the update area in pixel, with range [0, width())
		 * @param xWidth Width of the update area in pixel, with range [1, width() - xStart]
		 * @param firstRow The first row to be handled, with range [0, height())
		 * @param numberRows The number of rows to be handled, with range [1, height() - yStart]
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void applyOneFrameMapping8BitPerChannelSubset(uint8_t* const frame, const uint8_t* const mask, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int firstRow, const unsigned int numberRows) const;
};

template <unsigned int tChannels>
inline unsigned int MappingI1::spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const
{
	const unsigned int cost = spatialCost4Neighborhood(xTarget, yTarget, xSource, ySource, targetMask, targetMaskPaddingElements, maxCost);

	ocean_assert(cost <= maxCost);
	if (cost == maxCost)
	{
		return maxCost;
	}

	return cost * appearanceCostNormalization<tChannels>();
}

template <unsigned int tChannels>
inline unsigned int MappingI1::spatialCost8Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const
{
	const unsigned int cost = spatialCost8Neighborhood(xTarget, yTarget, xSource, ySource, targetMask, targetMaskPaddingElements, maxCost);

	ocean_assert(cost <= maxCost);
	if (cost == maxCost)
	{
		return maxCost;
	}

	return cost * appearanceCostNormalization<tChannels>();
}

template <unsigned int tChannels>
inline void MappingI1::applyOneFrameMapping8BitPerChannel(uint8_t* const frame, const uint8_t* const mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker) const
{
	ocean_assert(frame != nullptr && mask != nullptr);

	ocean_assert(xStart + xWidth <= width_);
	ocean_assert(yStart + yHeight <= height_);

	const unsigned int frameStrideElements = width_ * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width_ + maskPaddingElements;

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &MappingI1::applyOneFrameMapping8BitPerChannelSubset<tChannels>, frame, mask, frameStrideElements, maskStrideElements, xStart, xWidth, 0u, 0u), yStart, yHeight, 6u, 7u, 40u);
	}
	else
	{
		applyOneFrameMapping8BitPerChannelSubset<tChannels>(frame, mask, frameStrideElements, maskStrideElements, xStart, xWidth, yStart, yHeight);
	}
}

inline MappingI1& MappingI1::operator=(const MappingI1& pixelMapping)
{
	MappingI::operator=(pixelMapping);
	return *this;
}

inline MappingI1& MappingI1::operator=(MappingI1&& pixelMapping) noexcept
{
	if (this != &pixelMapping)
	{
		MappingI::operator=(pixelMapping);
	}

	return *this;
}

inline unsigned int MappingI1::spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const
{
	ocean_assert(targetMask != nullptr);
	ocean_assert(maxCost > 0u);

	// target position must fit to the layer dimensions
	ocean_assert(xTarget < width_ && yTarget < height_);

	const unsigned int targetMaskStrideElements = width_ + targetMaskPaddingElements;

	// the given in-coordinate must lie inside the completion mask
	ocean_assert(targetMask[yTarget * targetMaskStrideElements + xTarget] != 0xFF);

	unsigned int cost = maxCost;

	// north pixel is start position
	const uint8_t* targetMaskPointer = targetMask + (int(yTarget) - 1) * int(targetMaskStrideElements) + xTarget;
	const PixelPosition* mappingPointer = mappingI_ + (int(yTarget) - 1) * int(width_) + xTarget;

	// top pixel (north)
	if (yTarget > 0u && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget - 1u) * width_ + xTarget]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget - 1u) * targetMaskStrideElements + xTarget]);

		// topPosition - position = -1 (ideal)
		// => topPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource) + sqr(mappingPointer->y() - ySource + 1u);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource)) + sqr(int(mappingPointer->y()) - int(ySource) + 1));

		// stop if we cannot get better
		if (localCost == 0u)
		{
			return 0u;
		}

		// take the minimum cost only
		if (localCost < cost)
		{
			cost = localCost;
		}
	}

	// forward pointers to next pixel position
	ocean_assert(width_ >= 1u);
	targetMaskPointer += targetMaskStrideElements - 1u;
	mappingPointer += width_ - 1u;

	// left pixel (west)
	if (xTarget > 0 && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[yTarget * width_ + xTarget - 1u]);
		ocean_assert(*targetMaskPointer == targetMask[yTarget * targetMaskStrideElements + xTarget - 1u]);

		// leftPosition - position = -1 (ideal)
		// => leftPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource + 1u) + sqr(mappingPointer->y() - ySource);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource) + 1) + sqr(int(mappingPointer->y()) - int(ySource)));

		// stop if we cannot get better
		if (localCost == 0u)
		{
			return 0u;
		}

		// take the minimum cost only
		if (localCost < cost)
		{
			cost = localCost;
		}
	}

	// forward pointers to next pixel position
	targetMaskPointer += 2;
	mappingPointer += 2;

	// right pixel (east)
	if (xTarget + 1 < width_ && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[yTarget * width_ + xTarget + 1u]);
		ocean_assert(*targetMaskPointer == targetMask[yTarget * targetMaskStrideElements + xTarget + 1u]);

		// rightPosition - position = 1 (ideal)
		// => rightPosition - position - 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource - 1u) + sqr(mappingPointer->y() - ySource);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource) - 1) + sqr(int(mappingPointer->y()) - int(ySource)));

		// stop if we cannot get better
		if (localCost == 0u)
		{
			return 0u;
		}

		// take the minimum cost only
		if (localCost < cost)
		{
			cost = localCost;
		}
	}

	// forward points one pixel to the right
	targetMaskPointer += targetMaskStrideElements - 1u;
	mappingPointer += width_ - 1u;

	// bottom pixel (south)
	if (yTarget + 1 < height_ && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget + 1u) * width_ + xTarget]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget + 1u) * targetMaskStrideElements + xTarget]);

		// bottomPosition - position = 1 (ideal)
		// => bottomPosition - position - 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource) + sqr(mappingPointer->y() - ySource - 1u);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource)) + sqr(int(mappingPointer->y()) - int(ySource) - 1));

		// stop if we cannot get better
		if (localCost == 0u)
		{
			return 0u;
		}

		// take the minimum cost only
		if (localCost < cost)
		{
			cost = localCost;
		}
	}

	return cost;
}

inline unsigned int MappingI1::spatialCost8Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const unsigned int maxCost) const
{
	ocean_assert(targetMask);

	// target position must fit to the layer dimensions
	ocean_assert(xTarget < width_ && yTarget < height_);

	const unsigned int targetMaskStrideElements = width_ + targetMaskPaddingElements;

	// the given in-coordinate must lie inside the completion mask
	ocean_assert(targetMask[yTarget * targetMaskStrideElements + xTarget] != 0xFF);

	unsigned int cost = maxCost;

	const uint8_t* targetMaskPointer = targetMask + (int(yTarget) - 1) * int(targetMaskStrideElements) + xTarget - 1;
	const PixelPosition* mappingPointer = mappingI_ + (int(yTarget) - 1) * int(width_) + xTarget - 1;

	// top left pixel (north west)
	if (yTarget > 0 && xTarget > 0 && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget - 1u) * width_ + xTarget - 1u]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget - 1u) * targetMaskStrideElements + xTarget - 1u]);

		// topPosition - position = -1 (ideal)
		// => topPosition - position + 1 => min
		// leftPosition - position = -1 (ideal)
		// => leftPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource + 1u) + sqr(mappingPointer->y() - ySource + 1u);

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward points one pixel to the right
	++targetMaskPointer;
	++mappingPointer;

	// top pixel (north)
	if (yTarget > 0 && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget - 1u) * width_ + xTarget]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget - 1u) * targetMaskStrideElements + xTarget]);

		// topPosition - position = -1 (ideal)
		// => topPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource) + sqr(mappingPointer->y() - ySource + 1u);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource)) + sqr(int(mappingPointer->y()) - int(ySource) + 1));

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward points one pixel to the right
	++targetMaskPointer;
	++mappingPointer;

	// top right pixel (north east)
	if (yTarget > 0 && xTarget + 1u < width_ && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget - 1u) * width_ + xTarget + 1u]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget - 1u) * targetMaskStrideElements + xTarget + 1u]);

		// topPosition - position = -1 (ideal)
		// => topPosition - position + 1 => min
		// rightPosition - position = 1 (ideal)
		// => rightPosition - position - 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource - 1u) + sqr(mappingPointer->y() - ySource + 1u);

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward pointers to next pixel position
	ocean_assert(width_ >= 2u);
	targetMaskPointer += targetMaskStrideElements - 2u;
	mappingPointer += width_ - 2u;

	// left pixel (west)
	if (xTarget > 0 && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[yTarget * width_ + xTarget - 1u]);
		ocean_assert(*targetMaskPointer == targetMask[yTarget * targetMaskStrideElements + xTarget - 1u]);

		// leftPosition - position = -1 (ideal)
		// => leftPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource + 1u) + sqr(mappingPointer->y() - ySource);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource) + 1) + sqr(int(mappingPointer->y()) - int(ySource)));

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward pointers to next pixel position
	targetMaskPointer += 2;
	mappingPointer += 2;

	// right pixel (east)
	if (xTarget + 1 < width_ && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[yTarget * width_ + xTarget + 1u]);
		ocean_assert(*targetMaskPointer == targetMask[yTarget * targetMaskStrideElements + xTarget + 1u]);

		// rightPosition - position = 1 (ideal)
		// => rightPosition - position - 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource - 1u) + sqr(mappingPointer->y() - ySource);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource) - 1) + sqr(int(mappingPointer->y()) - int(ySource)));

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward pointers to next pixel position
	ocean_assert(width_ >= 2u);
	targetMaskPointer += targetMaskStrideElements - 2u;
	mappingPointer += width_ - 2u;

	// bottom left pixel (south west)
	if (xTarget > 0 && yTarget + 1 < height_ && * targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget + 1u) * width_ + xTarget - 1u]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget + 1u) * targetMaskStrideElements + xTarget - 1u]);

		// bottomPosition - position = 1 (ideal)
		// => bottomPosition - position - 1 => min
		// leftPosition - position = -1 (ideal)
		// => leftPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource + 1u) + sqr(mappingPointer->y() - ySource - 1u);

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward points one pixel to the right
	++targetMaskPointer;
	++mappingPointer;

	// bottom pixel (south)
	if (yTarget + 1 < height_ && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget + 1u) * width_ + xTarget]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget + 1u) * targetMaskStrideElements + xTarget]);

		// bottomPosition - position = 1 (ideal)
		// => bottomPosition - position - 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource) + sqr(mappingPointer->y() - ySource - 1u);
		ocean_assert(localCost == sqr(int(mappingPointer->x()) - int(xSource)) + sqr(int(mappingPointer->y()) - int(ySource) - 1));

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	// forward points one pixel to the right
	++targetMaskPointer;
	++mappingPointer;

	// bottom right pixel (south east)
	if (xTarget + 1 < width_ && yTarget + 1 < height_ && *targetMaskPointer != 0xFF)
	{
		ocean_assert(*mappingPointer);
		ocean_assert(*mappingPointer == mappingI_[(yTarget + 1u) * width_ + xTarget + 1u]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget + 1u) * targetMaskStrideElements + xTarget + 1u]);

		// bottomPosition - position = 1 (ideal)
		// => bottomPosition - position - 1 => min
		// leftPosition - position = -1 (ideal)
		// => leftPosition - position + 1 => min
		const unsigned int localCost = sqr(mappingPointer->x() - xSource - 1u) + sqr(mappingPointer->y() - ySource - 1u);

		// stop if we cannot get better
		if (localCost == 0u)
			return 0u;

		// take the minimum cost only
		if (localCost < cost)
			cost = localCost;
	}

	return cost;
}

template <unsigned int tChannels, unsigned int tBorderFactor>
unsigned int MappingI1::appearanceCost5x5(const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements) const
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tBorderFactor >= 1u, "Invalid border factor!");

	ocean_assert(mask != nullptr && frame != nullptr);

	ocean_assert(xTarget < width_ && yTarget < height_);
	ocean_assert(xSource < width_ && ySource < height_);

	const unsigned int maskStrideElements = width_ + maskPaddingElements;

	// the given in-coordinate must lie inside the completion mask
	ocean_assert(mask[yTarget * maskStrideElements + xTarget] != 0xFF);
	// the given out-coordinate must lie outside the completion mask
	ocean_assert(mask[ySource * maskStrideElements + xSource] == 0xFF);

	ocean_assert(width_ >= 3u);
	ocean_assert(height_ >= 3u);

	if (xSource >= 2 && ySource >= 2 && xSource + 2 < width_ && ySource + 2 < height_)
	{
		if (mask[yTarget * maskStrideElements + xTarget] == 0x00 || mask[yTarget * maskStrideElements + xTarget] >= 3)
		{
			ocean_assert(xTarget >= 2 && yTarget >= 2 && xTarget + 2 < width_ && yTarget + 2 < height_);
			return Advanced::SumSquareDifferencesNoCenter::patch8BitPerChannel<tChannels, 5u>(frame, frame, width_, width_, xTarget, yTarget, xSource, ySource, framePaddingElements, framePaddingElements) * spatialCostNormalization<tChannels>() / 25u;
		}

		if (xTarget >= 2 && yTarget >= 2 && xTarget + 2 < width_ && yTarget + 2 < height_)
		{
			const unsigned int frameStrideElements = width_ * tChannels + framePaddingElements;

			const uint8_t* const frameTargetTopLeft = frame + (yTarget - 2) * frameStrideElements + (xTarget - 2) * tChannels;
			const uint8_t* const frameSourceTopLeft = frame + (ySource - 2) * frameStrideElements + (xSource - 2) * tChannels;
			const uint8_t* const maskTopLeft = mask + (yTarget - 2) * maskStrideElements + xTarget - 2;

			return ssd5x5MaskNoCenter<tChannels, tBorderFactor>(frameTargetTopLeft, frameSourceTopLeft, maskTopLeft, width_, width_, framePaddingElements, framePaddingElements, maskPaddingElements) * spatialCostNormalization<tChannels>() / 25u;
		}
	}

	unsigned int cost = 0;

	int iterOutX = int(xSource) - 2;
	for (int iterInX = int(xTarget) - 2; iterInX <= int(xTarget) + 2; ++iterInX)
	{
		const unsigned int validIterInX = mirrorValue(iterInX, width_);
		const unsigned int validIterOutX = mirrorValue(iterOutX, width_);

		int iterOutY = int(ySource) - 2;
		for (int iterInY = int(yTarget) - 2; iterInY <= int(yTarget) + 2; ++iterInY)
		{
			if (iterInX != int(xTarget) || iterInY != int(yTarget))
			{
				const unsigned int validIterInY = mirrorValue(iterInY, height_);
				const unsigned int validIterOutY = mirrorValue(iterOutY, height_);

				ocean_assert(mask[validIterInY * maskStrideElements + validIterInX] + 1 > 0);

				if (mask[validIterInY * maskStrideElements + validIterInX] != 0xFF)
				{
					cost += CV::SumSquareDifferences::patch8BitPerChannelTemplate<tChannels, 1u>(frame, frame, width_, width_, validIterInX, validIterInY, validIterOutX, validIterOutY, framePaddingElements, framePaddingElements);
				}
				else
				{
					cost += CV::SumSquareDifferences::patch8BitPerChannelTemplate<tChannels, 1u>(frame, frame, width_, width_, validIterInX, validIterInY, validIterOutX, validIterOutY, framePaddingElements, framePaddingElements) * tBorderFactor;
				}
			}

			++iterOutY;
		}

		++iterOutX;
	}

	return cost * spatialCostNormalization<tChannels>() / 25u;
}

template <unsigned int tChannels>
void MappingI1::applyOneFrameMapping8BitPerChannelSubset(uint8_t* const frame, const uint8_t* const mask, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	typedef typename DataType<uint8_t, tChannels>::Type DataType;

	ocean_assert(xStart + xWidth <= width_);
	ocean_assert(firstRow + numberRows <= height_);

	ocean_assert(frameStrideElements >= width_ * tChannels);
	ocean_assert(maskStrideElements >= width_);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		DataType* framePixel = ((DataType*)(frame + y * frameStrideElements)) + xStart;
		const uint8_t* maskPixel = mask + y * maskStrideElements + xStart;

		const CV::PixelPosition* mappingPixel = mappingI_ + y * width_ + xStart; // mapping does not contain padding (mappingStride == width_)

		for (unsigned int n = 0u; n < xWidth; ++n)
		{
			if (*maskPixel != 0xFF)
			{
				ocean_assert(mappingPixel->isValid());

				*framePixel = ((const DataType*)(frame + mappingPixel->y() * frameStrideElements))[mappingPixel->x()];
			}

			++framePixel;
			++maskPixel;

			++mappingPixel;
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_MAPPING_I_1_H
