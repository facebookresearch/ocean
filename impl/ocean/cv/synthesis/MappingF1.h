/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_MAPPING_F_1_H
#define META_OCEAN_CV_SYNTHESIS_MAPPING_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/MappingF.h"

#include "ocean/base/DataType.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"

#include "ocean/cv/synthesis/MappingI.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * Cost function:
 *
 * pixelCost = structureFactor * structureCost + appearanceCost
 *
 * structureCost = structureCost' / normalizationStructureCost
 * appearanceCost = appearanceCost' / normalizationAppearanceCost
 *
 * structureCost
 * pixelCost = structureFactor * structureCost' / normalizationStructureCost + appearanceCost' / normalizationAppearanceCost
 *
 * pixelCost' = structureFactor * structureCost * normalizationAppearanceCost + appearanceCost * normalizationStructureCost
 *
 * 24bit:
 * normalizationAppearanceCost = 3 * 255^2 * numberSamples
 * normalizationStructureCost = width^2 + height^2
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT MappingF1 : public MappingF
{
	public:

		/**
		 * Creates an empty mapping object.
		 */
		MappingF1();

		/**
		 * Copies a mapping from a given mapping object.
		 * @param pixelMapping Pixel mapping to be copied
		 */
		MappingF1(const MappingF1& pixelMapping);

		/**
		 * Move constructor.
		 * @param mapping The mapping to be moved
		 */
		MappingF1(MappingF1&& mapping) noexcept;

		/**
		 * Creates a new mapping object width defined dimension.
		 * Beware: An initial mapping is not provided.<br>
		 * @param width The width of the mapping object in pixel, with range [1, infinity)
		 * @param height The height of the mapping object in pixel, with range [1, infinity)
		 */
		MappingF1(const unsigned int width, const unsigned int height);

		/**
		 * Calculates the smallest/cheapest spatial cost for a given point in a four-neighborhood and normalizes the result according to the frame dimension.
		 * Instead of summing ob the cost for all neighboring pixels, this function determines the minimal cost of all neighboring pixels.
		 * @param xTarget Horizontal target position to determine the spatial cost for, with range [0, width) and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the spatial cost for, with range [0, height) and must lie inside the target mask
		 * @param xSource Corresponding horizontal source mapping position for the given position
		 * @param ySource Corresponding vertical source mapping position for the given position
		 * @param targetMask Mask separating target and source pixels for the given target position, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param maxCost The maximal cost the spatial cost can have, with range [0, infinity)
		 * @return The resulting spatial cost
		 * @tparam tChannels The number of data channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline Scalar spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const Scalar maxCost) const;

		/**
		 * Calculates the appearance cost for a given point in a given frame.
		 * @param xTarget Horizontal target position to determine the appearance cost for, with range [2, width - 3] and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the appearance cost for, with range [2, height - 3] and must lie inside the target mask
		 * @param xSource Horizontal source position to determine the appearance cost for, with range [2, width - 3) and must lie outside the target mask
		 * @param ySource Vertical source position to determine the appearance cost for, with range [2, height - 3) and must lie outside the target mask
		 * @param frame The target and source frame to determine the appearance cost on, with same dimension as this mapping object
		 * @param mask The mask separating target and source pixels for the given positions, width same dimension as this mapping object and with 0xFF for source pixels, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param borderFactor Constant factor to weight the appearance cost of synthesis border pixels (border between target and source pixels) individually, with range [1, infinity)
		 * @tparam tChannels The number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		unsigned int appearanceCost5x5(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int borderFactor) const;

		/**
		 * Calculates the appearance cost for a given point between two given frames.
		 * @param xTarget Horizontal target position to determine the appearance cost for, with range [2, width - 3] and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the appearance cost for, with range [2, height - 3] and must lie inside the target mask
		 * @param xSource Horizontal source position to determine the appearance cost for, with range [2, width - 3) and must lie outside the target mask
		 * @param ySource Vertical source position to determine the appearance cost for, with range [2, height - 3) and must lie outside the target mask
		 * @param frame The target and source frame to determine the appearance cost on, with same dimension as this mapping object
		 * @param mask The mask separating target and source pixels for the given positions, width same dimension as this mapping object and with 0xFF for source pixels, must be valid
		 * @param reference The reference frame to determine the appearance cost on (evaluated at the target position), with same frame type as 'frame', must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param referencePaddingElements The number of padding elements at the end of each reference frame row, in elements, with range [0, infinity)
		 * @param borderFactor Constant factor to weight the appearance cost of synthesis border pixels (border between target and source pixels) individually, with range [1, infinity)
		 * @tparam tChannels The number of channels of the frame and reference frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline unsigned int appearanceReferenceCost5x5(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* frame, const uint8_t* mask, const uint8_t* reference, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int referencePaddingElements, const unsigned int borderFactor) const;

		/**
		 * Applies the current mapping for one given frame.<br>
		 * @see Mapping::applyMapping().
		 */
		void applyMapping(Frame& frame, const Frame& mask, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker = nullptr) const override;

		/**
		 * Applies the current mapping for one given frame.<br>
		 * Only mask pixels will be updated in the frame while the specification of a bounding box in which the mapping will be applied is used to improve the performance of the execution.
		 * @param frame The frame holding source and target area, must be valid
		 * @param mask The 8 bit mask defining source and target area with 0xFF defining a non-mask pixel, with same frame dimension and pixel origin as the provided frame, must be valid
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
		 * Assigns a pixel mapping with float accuracy.
		 * @param right Mapping to be assigned
		 * @return Reference to this object
		 */
		inline MappingF1& operator=(const MappingF1& right);

		/**
		 * Move operator.
		 * @param right Mapping to be moved
		 * @return Reference to this object
		 */
		inline MappingF1& operator=(MappingF1&& right) noexcept;

		/**
		 * Assigns a pixel mapping with float accuracy.
		 * @param right Mapping to be assigned
		 * @return Reference to this object
		 */
		MappingF1& operator=(const MappingI& right);

	protected:

		/**
		 * Returns the sum of square differences for a 5x5 block determined between two individual pixel and one sub-pixel position between three individual images.
		 * The result is the sum of two ssd calculations: ssd(image0, image2) * factor02 + ssd(image1, image2) * factor12.<br>
		 * @param image0 The first image to be used, must be valid
		 * @param image1 The second image to be used, must be valid
		 * @param image2 The third image to be used, must be valid
		 * @param width0 The width of the first frame, in pixels, with range [tPatchSize, infinity)
		 * @param width1 The width of the second frame, in pixels) with range [tPatchSize, infinity)
		 * @param width2 The width of the third frame, in pixels, with range [tPatchSize + 1, infinity)
		 * @param image0PaddingElements The number of padding elements at the end of each first image row, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image row, in elements, with range [0, infinity)
		 * @param image2PaddingElements The number of padding elements at the end of each third image row, in elements, with range [0, infinity)
		 * @param factor02 The multiplication factor for the ssd(image0, image2) result
		 * @param factor12 The multiplication factor for the ssd(image1, image2) result
		 * @param centerX0 Horizontal center position of the 5x5 block in the first frame, with range [tPatchSize/2, width0 - tPatchSize/2)
		 * @param centerY0 Vertical center position of the 5x5 block in the first frame, with range [tPatchSize/2, height0 - tPatchSize/2)
		 * @param centerX1 Horizontal center position of the 5x5 block in the first frame, with range [tPatchSize/2, width1 - tPatchSize/2)
		 * @param centerY1 Vertical center position of the 5x5 block in the first frame, with range [tPatchSize/2, height1 - tPatchSize/2)
		 * @param centerX2 Horizontal sub-pixel center position of the 5x5 block in the second frame, with range [tPatchSize/2, width2 - tPatchSize/2 - 1)
		 * @param centerY2 Vertical sub-pixel center position of the 5x5 block in the second frame, with range [tPatchSize/2, height2 - tPatchSize/2 -1)
		 * @return The resulting SSD result
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline unsigned int sumSquareDifferencesThreePatch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* image2, const unsigned int width0, const unsigned int width1, const unsigned int width2, unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int image2PaddingElements, const unsigned int factor02, const unsigned int factor12, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const Scalar centerX2, const Scalar centerY2);

		/**
		 * Calculates the sum of square differences between two 5x5 frame regions in two frames with explicit weighted mask pixels.<br>
		 * Compared to the standard ssd calculation this extended version weights the square difference of mask pixels (value not equal to 0xFF) with a given factor.
		 * @param frame0 Pointer to the top left position in the 5x5 region in the first frame
		 * @param frame1 Pointer to the top left position in the 5x5 region in the second frame
		 * @param mask0 Pointer to the top left position in the 5x5 region in the mask frame, with 0xFF defining a non-mask pixel
		 * @param width0 The width of the first frame (and the mask) in pixel, with range [5, infinity)
		 * @param height0 The height of the first frame (and the mask) in pixel, with range [5, infinity)
		 * @param width1 The width of the second frame in pixel, with range [6, infinity)
		 * @param height1 The height of the second frame in pixel, with range [6, infinity)
		 * @param xPosition0 Horizontal pixel center position of the 5x5 block in the first frame, with range [2, width - 3]
		 * @param yPosition0 Vertical pixel center position of the 5x5 block in the first frame, with range [2, height - 3]
		 * @param xPosition1 Horizontal sub-pixel center position of the 5x5 block in the second frame, with range [2, width - 3)
		 * @param yPosition1 Vertical sub-pixel center position of the 5x5 block in the second frame, with range [2, height - 3)
		 * @param frame0PaddingElements The number of padding elements at the end of each first frame row, in elements, with range [0, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each second frame row, in elements, with range [0, infinity)
		 * @param mask0PaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param borderFactor Multiplication factor for squared differences of border pixels, with range [1, infinity)
		 * @return Resulting sum of squared differences
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline unsigned int ssd5x5Mask(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int xPosition0, const unsigned int yPosition0, const Scalar xPosition1, const Scalar yPosition1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int borderFactor);

		/**
		 * Calculates the smallest/cheapest spatial cost for a given point in a four-neighborhood.
		 * Instead of summing ob the cost for all neighboring pixels, this function determines the minimal cost of all neighboring pixels.
		 * @param xTarget Horizontal target position to determine the spatial cost for, with range [0, width) and must lie inside the target mask
		 * @param yTarget Vertical target position to determine the spatial cost for, with range [0, height) and must lie inside the target mask
		 * @param xSource Corresponding horizontal source mapping position for the given position
		 * @param ySource Corresponding vertical source mapping position for the given position
		 * @param targetMask Mask separating target and source pixels for the given target position, width same dimension as this mapping object and with 0xFF for source pixels
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param maxCost The maximal cost the spatial cost can have, with range [0, infinity)
		 * @return Resulting spatial cost for the given points
		 */
		inline Scalar spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const Scalar maxCost) const;

		/**
		 * Applies the current mapping in a subset of one given frame.<br>
		 * Only mask pixels will be updated in the frame while the specification of a bounding box in which the mapping will be applied is used to improve the performance of the execution.
		 * @param frame The frame holding source and target area, with frame dimension identical to width() x height()
		 * @param mask The 8 bit mask defining source and target area with 0xFF defining a non-mask pixel, with same frame dimension and pixel origin as the provided frame
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param xStart Horizontal start position of the update area in pixel, with range [0, width())
		 * @param xWidth Width of the update area in pixel, with range [1, width() - xStart]
		 * @param firstRow The first row to be handled, with range [0, height())
		 * @param numberRows The number of rows to be handled, with range [1, height() - yStart]
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void applyOneFrameMapping8BitPerChannelSubset(uint8_t* const frame, const uint8_t* const mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int firstRow, const unsigned int numberRows) const;
};

template <unsigned int tChannels>
inline Scalar MappingF1::spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const Scalar maxCost) const
{
	return std::min(spatialCost4Neighborhood(xTarget, yTarget, xSource, ySource, targetMask, targetMaskPaddingElements, maxCost) * Scalar(appearanceCostNormalization<tChannels>()), maxCost);
}

template <unsigned int tChannels>
unsigned int MappingF1::appearanceCost5x5(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int borderFactor) const
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(mask != nullptr && mask != nullptr);

	ocean_assert(width_ >= 6u && height_ >= 6u);

	ocean_assert(xTarget >= 2u && xTarget <= width_ - 3u);
	ocean_assert(yTarget >= 2u && yTarget <= height_ - 3u);

	ocean_assert(xSource >= Scalar(2) && xSource < Scalar(width_ - 3u));
	ocean_assert(ySource >= Scalar(2) && ySource < Scalar(height_ - 3u));

	const unsigned int maskStrideElements = width_ + maskPaddingElements;

	// the given target coordinate must lie inside the completion mask
	ocean_assert(mask[yTarget * maskStrideElements + xTarget] != 0xFFu);

#ifdef OCEAN_DEBUG
	const int xSourceRounded(Numeric::round32(xSource));
	const int ySourceRounded(Numeric::round32(ySource));

	// the given out-coordinate must lie outside the completion mask
	ocean_assert(mask[ySourceRounded * maskStrideElements + xSourceRounded] == 0xFFu);
#endif

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int patchSize_2 = patchSize / 2u;

	constexpr unsigned int patchArea = patchSize * patchSize;

	if (xSource >= Scalar(patchSize_2) && ySource >= Scalar(patchSize_2) && xSource < Scalar(width_ - patchSize_2 - 1u) && ySource < Scalar(height_ - patchSize_2 - 1u))
	{
		if (mask[yTarget * maskStrideElements + xTarget] == 0x00u || mask[yTarget * maskStrideElements + xTarget] >= 3u)
		{
			ocean_assert(xTarget >= patchSize_2 && yTarget >= patchSize_2 && xTarget <= width_ - patchSize_2 - 1u && yTarget <= height_ - patchSize_2 - 1u);

			return Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, patchSize>(frame, frame, width_, width_, xTarget, yTarget, xSource, ySource, framePaddingElements, framePaddingElements) * spatialCostNormalization<tChannels>() / patchArea;
		}

		if (xTarget >= patchSize_2 && yTarget >= patchSize_2 && xTarget <= width_ - patchSize_2 - 1u && yTarget <= height_ - patchSize_2 - 1u)
		{
			return ssd5x5Mask<tChannels>(frame, frame, mask, width_, height_, width_, height_, xTarget, yTarget, xSource, ySource, framePaddingElements, framePaddingElements, maskPaddingElements, borderFactor) * spatialCostNormalization<tChannels>() / patchArea;
		}
	}

	ocean_assert(false && "This should never happen!");
	return 0xFFFFFF00u; // explicitly below 0xFFFFFFFF = (unsigned int)(-1)
}

template <unsigned int tChannels>
inline unsigned int MappingF1::appearanceReferenceCost5x5(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* frame, const uint8_t* mask, const uint8_t* reference, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int referencePaddingElements, const unsigned int borderFactor) const
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(mask != nullptr && mask != nullptr && reference != nullptr);

	ocean_assert(width_ >= 6u && height_ >= 6u);

	ocean_assert(xTarget >= 2u && xTarget <= width_ - 3u);
	ocean_assert(yTarget >= 2u && yTarget <= height_ - 3u);

	ocean_assert(xSource >= Scalar(2) && xSource < Scalar(width_ - 3u));
	ocean_assert(ySource >= Scalar(2) && ySource < Scalar(height_ - 3u));

	const unsigned int maskStrideElements = width_ + maskPaddingElements;

	// the given in-coordinate must lie inside the completion mask
	ocean_assert(mask[yTarget * maskStrideElements + xTarget] != 0xFF);

#ifdef OCEAN_DEBUG
	const int xSourceRounded(Numeric::round32(xSource));
	const int ySourceRounded(Numeric::round32(ySource));

	// the given out-coordinate must lie outside the completion mask
	ocean_assert(mask[ySourceRounded * maskStrideElements + xSourceRounded] == 0xFFu);
#endif

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int patchSize_2 = patchSize / 2u;

	constexpr unsigned int patchArea = patchSize * patchSize;

	if (xSource >= Scalar(patchSize_2) && ySource >= Scalar(patchSize_2) && xSource < Scalar(width_ - patchSize_2 - 1u) && ySource < Scalar(height_ - patchSize_2 - 1u))
	{
		if (mask[yTarget * maskStrideElements + xTarget] == 0x00u || mask[yTarget * maskStrideElements + xTarget] >= 3u)
		{
			ocean_assert(xTarget >= patchSize_2 && yTarget >= patchSize_2 && xTarget <= width_ - patchSize_2 - 1u && yTarget <= height_ - patchSize_2 - 1u);

			return sumSquareDifferencesThreePatch8BitPerChannel<tChannels, patchSize>(frame, reference, frame, width_, width_, width_, framePaddingElements, referencePaddingElements, framePaddingElements, 1u, 5u, xTarget, yTarget, xTarget, yTarget, xSource, ySource) * spatialCostNormalization<tChannels>() / patchArea;
		}

		if (xTarget >= patchSize_2 && yTarget >= patchSize_2 && xTarget <= width_ - patchSize_2 - 1u && yTarget <= height_ - patchSize_2 - 1u)
		{
			return (5u * ssd5x5Mask<tChannels>(reference, frame, mask, width_, height_, width_, height_, xTarget, yTarget, xSource, ySource, referencePaddingElements, framePaddingElements, maskPaddingElements, borderFactor)
						+ ssd5x5Mask<tChannels>(frame, frame, mask, width_, height_, width_, height_, xTarget, yTarget, xSource, ySource, framePaddingElements, framePaddingElements, maskPaddingElements, borderFactor)) * spatialCostNormalization<tChannels>() / patchArea;
		}
	}

	ocean_assert(false && "This should never happen!");
	return 0xFFFFFF00u; // explicitly below 0xFFFFFFFF = (unsigned int)(-1)
}

inline Scalar MappingF1::spatialCost4Neighborhood(const unsigned int xTarget, const unsigned int yTarget, const Scalar xSource, const Scalar ySource, const uint8_t* targetMask, const unsigned int targetMaskPaddingElements, const Scalar maxCost) const
{
	ocean_assert(targetMask != nullptr);
	ocean_assert(maxCost > Scalar(0));

	const unsigned int targetMaskStrideElements = width_ + targetMaskPaddingElements;

	// target position must fit to the layer dimensions
	ocean_assert(xTarget < width_ && yTarget < height_);

	// the given in-coordinate must lie inside the completion mask
	ocean_assert(targetMask[yTarget * targetMaskStrideElements + xTarget] != 0xFF);

	Scalar cost = maxCost;

	const uint8_t* targetMaskPointer = targetMask + (int(yTarget) - 1) * int(targetMaskStrideElements) + xTarget;
	const Vector2* mappingPointer = mappingF_ + (int(yTarget) - 1) * int(width_) + xTarget;

	// top pixel (north)
	if (yTarget > 0 && *targetMaskPointer != 0xFF)
	{
		ocean_assert(mappingPointer->x() >= 0 && mappingPointer->x() < Scalar(width_));
		ocean_assert(mappingPointer->y() >= 0 && mappingPointer->y() < Scalar(height_));
		ocean_assert(*mappingPointer == mappingF_[(yTarget - 1u) * width_ + xTarget]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget - 1u) * targetMaskStrideElements + xTarget]);

		// topPosition - position = -1 (ideal)
		// => topPosition - position + 1 => min
		const Scalar localCost = sqr(mappingPointer->x() - xSource) + sqr(mappingPointer->y() - ySource + 1);

		if (localCost == 0)
		{
			return 0;
		}

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
		ocean_assert(mappingPointer->x() >= 0 && mappingPointer->x() < Scalar(width_));
		ocean_assert(mappingPointer->y() >= 0 && mappingPointer->y() < Scalar(height_));
		ocean_assert(*mappingPointer == mappingF_[yTarget * width_ + xTarget - 1u]);
		ocean_assert(*targetMaskPointer == targetMask[yTarget * targetMaskStrideElements + xTarget - 1u]);

		// leftPosition - position = -1 (ideal)
		// => leftPosition - position + 1 => min
		const Scalar localCost = sqr(mappingPointer->x() - xSource + 1) + sqr(mappingPointer->y() - ySource);

		if (localCost == 0)
		{
			return 0;
		}

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
		ocean_assert(mappingPointer->x() >= 0 && mappingPointer->x() < Scalar(width_));
		ocean_assert(mappingPointer->y() >= 0 && mappingPointer->y() < Scalar(height_));
		ocean_assert(*mappingPointer == mappingF_[yTarget * width_ + xTarget + 1u]);
		ocean_assert(*targetMaskPointer == targetMask[yTarget * targetMaskStrideElements + xTarget + 1u]);

		// rightPosition - position = 1 (ideal)
		// => rightPosition - position - 1 => min
		const Scalar localCost = sqr(mappingPointer->x() - xSource - 1) + sqr(mappingPointer->y() - ySource);

		if (localCost == 0)
		{
			return 0;
		}

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
		ocean_assert(mappingPointer->x() >= 0 && mappingPointer->x() < Scalar(width_));
		ocean_assert(mappingPointer->y() >= 0 && mappingPointer->y() < Scalar(height_));
		ocean_assert(*mappingPointer == mappingF_[(yTarget + 1u) * width_ + xTarget]);
		ocean_assert(*targetMaskPointer == targetMask[(yTarget + 1u) * targetMaskStrideElements + xTarget]);

		// bottomPosition - position = 1 (ideal)
		// => bottomPosition - position - 1 => min
		const Scalar localCost = sqr(mappingPointer->x() - xSource) + sqr(mappingPointer->y() - ySource - 1);

		if (localCost == 0)
		{
			return 0;
		}

		if (localCost < cost)
		{
			cost = localCost;
		}
	}

	return cost;
}

inline MappingF1& MappingF1::operator=(const MappingF1& right)
{
	MappingF::operator=(right);
	return *this;
}

inline MappingF1& MappingF1::operator=(MappingF1&& right) noexcept
{
	if (this != &right)
	{
		MappingF::operator=(std::move(right));
	}

	return *this;
}

template <unsigned int tChannels>
inline unsigned int MappingF1::ssd5x5Mask(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int xPosition0, const unsigned int yPosition0, const Scalar xPosition1, const Scalar yPosition1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int borderFactor)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame0 != nullptr && frame1 != nullptr && mask0 != nullptr);
	ocean_assert_and_suppress_unused(width0 >= 5u && height0 >= 5u, height0);
	ocean_assert_and_suppress_unused(width1 >= 6u && height1 >= 6u, height1);

	ocean_assert(xPosition0 >= 2u && xPosition0 <= width0 - 2u);
	ocean_assert(yPosition0 >= 2u && yPosition0 <= height0 - 2u);

	ocean_assert(xPosition1 >= Scalar(2) && xPosition1 < Scalar(width1 - 3u));
	ocean_assert(yPosition1 >= Scalar(2) && yPosition1 < Scalar(height1 - 3u));

	ocean_assert(borderFactor >= 1u);

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int patchArea = patchSize * patchSize;

	constexpr unsigned int framePatch0PaddingElements = 0u;
	constexpr unsigned int maskPatch0PaddingElements = 0u;

	uint8_t framePatch0[patchArea * tChannels];
	FrameConverter::patchFrame<uint8_t>(frame0, framePatch0, width0, tChannels, xPosition0, yPosition0, patchSize, frame0PaddingElements, framePatch0PaddingElements);

	uint8_t maskPatch0[patchArea];
	FrameConverter::patchFrame<uint8_t>(mask0, maskPatch0, width0, 1u, xPosition0, yPosition0, patchSize, mask0PaddingElements, maskPatch0PaddingElements);

	uint8_t framePatch1[patchArea * tChannels];
	Advanced::AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, patchSize, CV::PC_TOP_LEFT>(frame1, width1, frame1PaddingElements, framePatch1, Vector2(xPosition1, yPosition1));

	unsigned int ssd = 0u;

	for (unsigned int n = 0u; n < patchArea; ++n)
	{
		unsigned int ssdLocal = 0u;

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			const int value = int(framePatch0[n * tChannels + c]) - int(framePatch1[n * tChannels + c]);
			ssdLocal += value * value;
		}

		if (maskPatch0[n] == 0xFFu)
		{
			ssd += ssdLocal * borderFactor;
		}
		else
		{
			ssd += ssdLocal;
		}
	}

	return ssd;
}

template <unsigned int tChannels>
inline void MappingF1::applyOneFrameMapping8BitPerChannel(uint8_t* const frame, const uint8_t* const mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker) const
{
	ocean_assert(frame != nullptr && mask != nullptr);

	ocean_assert(xStart + xWidth <= width_);
	ocean_assert(yStart + yHeight <= height_);

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &MappingF1::applyOneFrameMapping8BitPerChannelSubset<tChannels>, frame, mask, framePaddingElements, maskPaddingElements, xStart, xWidth, 0u, 0u), yStart, yHeight, 6u, 7u, 20u);
	}
	else
	{
		applyOneFrameMapping8BitPerChannelSubset<tChannels>(frame, mask, framePaddingElements, maskPaddingElements, xStart, xWidth, yStart, yHeight);
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <>
inline unsigned int MappingF1::sumSquareDifferencesThreePatch8BitPerChannel<3u, 5u>(const uint8_t* image0, const uint8_t* image1, const uint8_t* image2, const unsigned int width0, const unsigned int width1, const unsigned int width2, unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int image2PaddingElements, const unsigned int factor02, const unsigned int factor12, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const Scalar centerX2, const Scalar centerY2)
{
	constexpr unsigned int tChannels = 3u;
	constexpr unsigned int tPatchSize = 5u;
	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(image0 != nullptr && image1 != nullptr && image2 != nullptr);

	ocean_assert(centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);
	ocean_assert(centerY0 >= tPatchSize_2);

	ocean_assert(centerX1 >= tPatchSize_2 && centerX1 < width1 - tPatchSize_2);
	ocean_assert(centerY1 >= tPatchSize_2);

	ocean_assert(centerX2 >= Scalar(tPatchSize_2) && centerX2 < Scalar(width2 - tPatchSize_2 - 1u));
	ocean_assert(centerY2 >= Scalar(tPatchSize_2));

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;
	const unsigned int image2StrideElements = width2 * tChannels + image2PaddingElements;

	const uint8_t* imageTopLeft0 = image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels;
	const uint8_t* imageTopLeft1 = image1 + (centerY1 - tPatchSize_2) * image1StrideElements + (centerX1 - tPatchSize_2) * tChannels;
	const uint8_t* imageTopLeft2 = image2 + ((unsigned int)(centerY2) - tPatchSize_2) * image2StrideElements + ((unsigned int)(centerX2) - tPatchSize_2) * tChannels;

	const Scalar scalarFx2 = centerX2 - Scalar((unsigned int)(centerX2));
	const Scalar scalarFy2 = centerY2 - Scalar((unsigned int)(centerY2));

	ocean_assert(scalarFx2 >= 0 && scalarFx2 <= 1);
	ocean_assert(scalarFy2 >= 0 && scalarFy2 <= 1);

	const unsigned int fx2 = (unsigned int)(Scalar(128) * scalarFx2 + Scalar(0.5));
	const unsigned int fy2 = (unsigned int)(Scalar(128) * scalarFy2 + Scalar(0.5));


	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	SSE::prefetchT0(imageTopLeft2);
	SSE::prefetchT0(imageTopLeft2 + image2StrideElements);

	const unsigned int fx2_ = 128u - fx2;
	const unsigned int fy2_ = 128u - fy2;

	const __m128i f2x_y_ = _mm_set1_epi16(short(fx2_ * fy2_));
	const __m128i f2xy_ = _mm_set1_epi16(short(fx2 * fy2_));
	const __m128i f2x_y = _mm_set1_epi16(short(fx2_ * fy2));
	const __m128i f2xy = _mm_set1_epi16(short(fx2 * fy2));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);
	SSE::prefetchT0(imageTopLeft2 + 2u * image2StrideElements);

	// row 0
	__m128i image0_row0 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)imageTopLeft0), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft2);
	__m128i image2_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + image2StrideElements));
	__m128i interpolation2 = SSE::interpolation3Channel24Bit8Elements(image2_row0Front, image2_row1Front, f2x_y_, f2xy_, f2x_y, f2xy);

	__m128i image1_row0 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)imageTopLeft1), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 8u));
	__m128i image2_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + image2StrideElements + 8u));
	interpolation2 = _mm_or_si128(interpolation2, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image2_row0Back, image2_row1Back, f2x_y_, f2xy_, f2x_y, f2xy), 9));

	__m128i result02 = SSE::sumSquareDifference8Bit16Elements(image0_row0, interpolation2);
	__m128i result12 = SSE::sumSquareDifference8Bit16Elements(image1_row0, interpolation2);


	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);
	SSE::prefetchT0(imageTopLeft2 + 3u * image2StrideElements);


	// row 1
	__m128i image0_row1 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 1u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 2u * image2StrideElements));
	interpolation2 = SSE::interpolation3Channel24Bit8Elements(image2_row1Front, image2_row2Front, f2x_y_, f2xy_, f2x_y, f2xy);

	__m128i image1_row1 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 1u * image1StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 2u * image2StrideElements + 8u));
	interpolation2 = _mm_or_si128(interpolation2, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image2_row1Back, image2_row2Back, f2x_y_, f2xy_, f2x_y, f2xy), 9));

	result02 = _mm_add_epi32(result02, SSE::sumSquareDifference8Bit16Elements(image0_row1, interpolation2));
	result12 = _mm_add_epi32(result12, SSE::sumSquareDifference8Bit16Elements(image1_row1, interpolation2));


	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);
	SSE::prefetchT0(imageTopLeft2 + 4u * image2StrideElements);


	// row 2
	__m128i image0_row2 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 3u * image2StrideElements));
	interpolation2 = SSE::interpolation3Channel24Bit8Elements(image2_row2Front, image2_row3Front, f2x_y_, f2xy_, f2x_y, f2xy);

	__m128i image1_row2 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 3u * image2StrideElements + 8u));
	interpolation2 = _mm_or_si128(interpolation2, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image2_row2Back, image2_row3Back, f2x_y_, f2xy_, f2x_y, f2xy), 9));

	result02 = _mm_add_epi32(result02, SSE::sumSquareDifference8Bit16Elements(image0_row2, interpolation2));
	result12 = _mm_add_epi32(result12, SSE::sumSquareDifference8Bit16Elements(image1_row2, interpolation2));


	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);
	SSE::prefetchT0(imageTopLeft2 + 5u * image2StrideElements);

	// row 3
	__m128i image0_row3 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 4u * image2StrideElements));
	interpolation2 = SSE::interpolation3Channel24Bit8Elements(image2_row3Front, image2_row4Front, f2x_y_, f2xy_, f2x_y, f2xy);

	__m128i image1_row3 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 4u * image2StrideElements + 8u));
	interpolation2 = _mm_or_si128(interpolation2, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image2_row3Back, image2_row4Back, f2x_y_, f2xy_, f2x_y, f2xy), 9));

	result02 = _mm_add_epi32(result02, SSE::sumSquareDifference8Bit16Elements(image0_row3, interpolation2));
	result12 = _mm_add_epi32(result12, SSE::sumSquareDifference8Bit16Elements(image1_row3, interpolation2));



	// row 4
	__m128i image0_row4 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft2 + 5u * image2StrideElements));
	interpolation2 = SSE::interpolation3Channel24Bit8Elements(image2_row4Front, image2_row5Front, f2x_y_, f2xy_, f2x_y, f2xy);

	__m128i image1_row4 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image2_row5Back = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft2 + 5u * image2StrideElements + 8u - 6u)), 6); // here we start 6 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation2 = _mm_or_si128(interpolation2, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image2_row4Back, image2_row5Back, f2x_y_, f2xy_, f2x_y, f2xy), 9));

	result02 = _mm_add_epi32(result02, SSE::sumSquareDifference8Bit16Elements(image0_row4, interpolation2));
	result12 = _mm_add_epi32(result12, SSE::sumSquareDifference8Bit16Elements(image1_row4, interpolation2));


	return (SSE::sum_u32_4(result02)) * factor02 + (SSE::sum_u32_4(result12)) * factor12;
}

#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <unsigned int tChannels, unsigned int tPatchSize>
inline unsigned int MappingF1::sumSquareDifferencesThreePatch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* image2, const unsigned int width0, const unsigned int width1, const unsigned int width2, unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int image2PaddingElements, const unsigned int factor02, const unsigned int factor12, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const Scalar centerX2, const Scalar centerY2)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid channel number!");

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(image0 != nullptr && image1 != nullptr && image2 != nullptr);

	ocean_assert_and_suppress_unused(centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2, tPatchSize_2);
	ocean_assert(centerY0 >= tPatchSize_2);

	ocean_assert(centerX1 >= tPatchSize_2 && centerX1 < width1 - tPatchSize_2);
	ocean_assert(centerY1 >= tPatchSize_2);

	ocean_assert(centerX2 >= Scalar(tPatchSize_2) && centerX2 < Scalar(width2 - tPatchSize_2 - 1u));
	ocean_assert(centerY2 >= Scalar(tPatchSize_2));

	uint8_t interpolated2[tPatchSize * tPatchSize * tChannels];

	CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image2, width2, image2PaddingElements, interpolated2, Vector2(centerX2, centerY2));

	return SumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(image0, width0, centerX0, centerY0, image0PaddingElements, interpolated2) * factor02
				+ SumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(image1, width1, centerX1, centerY1, image1PaddingElements, interpolated2) * factor12;
}

template <unsigned int tChannels>
void MappingF1::applyOneFrameMapping8BitPerChannelSubset(uint8_t* const frame, const uint8_t* const mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int xStart, const unsigned int xWidth, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	typedef typename DataType<uint8_t, tChannels>::Type DataType;

	ocean_assert(xStart + xWidth <= width_);
	ocean_assert(firstRow + numberRows <= height_);

	const unsigned int frameStrideElements = width_ * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width_ + maskPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		DataType* framePixel = ((DataType*)(frame + y * frameStrideElements)) + xStart;
		const uint8_t* maskPixel = mask + y * maskStrideElements + xStart;

		const Vector2* mappingPixel = mappingF_ + y * width_ + xStart; // mapping does not contain padding (mappingStride == width_)

		for (unsigned int n = 0u; n < xWidth; ++n)
		{
			if (*maskPixel != 0xFF)
			{
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(frame, width_, height_, framePaddingElements, *mappingPixel, (uint8_t*)(framePixel));
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

#endif // META_OCEAN_CV_SYNTHESIS_MAPPING_F_1_H
