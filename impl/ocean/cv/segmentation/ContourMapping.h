/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_CONTOUR_MAPPING_H
#define META_OCEAN_CV_SEGMENTATION_CONTOUR_MAPPING_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"
#include "ocean/cv/SumSquareDifferences.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class determines mappings between pixels lying on a contour.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT ContourMapping
{
	public:

		/**
		 * Determines the mapping between pixels lying on a contour (defined within a frame) based on visual and geometrical constraints.
		 * The resulting mapping pairs (bidirectionally) pixels on the contour that seem to have the same characteristics regarding visual or geometrical conditions.<br>
		 * Further, the resulting mapping will contain mapping between contour pixels so that the line between those pixels do not leave the mask area.
		 * @param frame The frame in which the contour is defined, must be valid
		 * @param mask The exclusive mask matching with the provided (surrounding) contour, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param width The width of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @return The resulting mapping for each pixel of the provided contour, for each (source) contour pixel the index of the corresponding (target) contour pixel will be provided, an index of -1 identifies pixels without corresponding pixel
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 * @see mappingInterpolation8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static Indices32 contour2mapping8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPositions& denseContour);

		/**
		 * Replaces the visual content of a frame within a mask area by using the mapping information of the contour surrounding the mask.
		 * The new visual content is determined by interpolating the color values of the contour pixels between corresponding pixels and each mask pixel lying on the line, between two matching pixels, located in the mask, will be adjusted/set.<br>
		 * Further, each mask pixel will be set to a non-mask pixel in the case the corresponding pixel in the frame has been adjusted/set.
		 * @param frame The frame for which the visual information (within the mask area) will be adjusted, must be valid
		 * @param mask The exclusive mask matching with the provided (surrounding) contour, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param width The width of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @param boundingBox The bounding box
		 * @param mapping The pixel-wise mapping, matching to the contour, used to interpolate the visual content within the mask area
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 * @see contour2mapping8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void mappingInterpolation8BitPerChannel(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPositions& denseContour, const PixelBoundingBox& boundingBox, const Indices32& mapping);

	private:

		/**
		 * Determines an initial mapping between all pixel of a contour based on simple geometrical and visual rules.
		 * @param frame The frame in which the contour is defined, must be valid
		 * @param mask The exclusive mask matching with the provided (surrounding) contour, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param outline An additional mask highlighting all pixels outlining the mask, so highlighting all pixels converted by the contour
		 * @param width The width of the provided frame (and masks) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param outlinePaddingElements The number of padding elements at the end of each outline row, in elements, with range [0, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @param appearanceThreshold The maximal threshold for the appearance measure, with range [0, infinity)
		 * @return The resulting mapping for each pixel of the provided contour, for each (source) contour pixel the index of the corresponding (target) contour pixel will be provided, an index of -1 identifies pixels without corresponding pixel
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static Indices32 mappingInitialization8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int outlinePaddingElements, const PixelPositions& denseContour, const unsigned int appearanceThreshold);

		/**
		 * Iteratively improves the mapping quality between contour pixels e.g., by propagating  an initial mapping between all pixel of a contour based on simple geometrical and visual rules.
		 * @param frame The frame in which the contour is defined, must be valid
		 * @param mask The exclusive mask matching with the provided (surrounding) contour, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param outline An additional mask highlighting all pixels outlining the mask, so highlighting all pixels converted by the contour
		 * @param width The width of the provided frame (and masks) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param outlinePaddingElements The number of padding elements at the end of each outline row, in elements, with range [0, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @param mapping An already given initial mapping that will be improved, with ranges [0, width)x[0, height)
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void improveMapping8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int outlinePaddingElements, const PixelPositions& denseContour, Indices32& mapping);

		/**
		 * Determines the total cost of a pixel mapping during the forward propagation iteration including appearance and geometric costs.
		 * @param frame The frame in which the contour is defined, must be valid
		 * @param mask The exclusive mask matching with the provided (surrounding) contour, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param outline An additional mask highlighting all pixels outlining the mask, so highlighting all pixels converted by the contour
		 * @param width The width of the provided frame (and masks) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param frameStrideElements The number of elements between two frame rows, in elements, with range [width * tChannels, infinity)
		 * @param maskStrideElements The number of elements between two mask rows, in elements, with range [width * tChannels, infinity)
		 * @param outlineStrideElements The number of elements between two outline rows, in elements, with range [width * tChannels, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @param mapping An already given initial mapping that will be improved, with ranges [0, width)x[0, height)
		 * @param sourceIndex The index of the source contour pixel, with range [0, denseContour.size())
		 * @param targetIndex The index of the target contour pixel, with range [0, denseContour.size())
		 * @param oneWay True, to measure the appearance cost regarding the previous neighbor; False, to measure the appearance cost regarding both (previous and next) neighbors
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static unsigned int totalForwardCost8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const unsigned int outlineStrideElements, const PixelPositions& denseContour, const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex, const bool oneWay);

		/**
		 * Determines the total cost of a pixel mapping during the backward propagation iteration including appearance and geometric costs.
		 * @param frame The frame in which the contour is defined, must be valid
		 * @param mask The exclusive mask matching with the provided (surrounding) contour, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param outline An additional mask highlighting all pixels outlining the mask, so highlighting all pixels converted by the contour
		 * @param width The width of the provided frame (and masks) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param frameStrideElements The number of elements between two frame rows, in elements, with range [width * tChannels, infinity)
		 * @param maskStrideElements The number of elements between two mask rows, in elements, with range [width * tChannels, infinity)
		 * @param outlineStrideElements The number of elements between two outline rows, in elements, with range [width * tChannels, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @param mapping An already given initial mapping that will be improved, with ranges [0, width)x[0, height)
		 * @param sourceIndex The index of the source (starting) contour pixel, with range [0, denseContour.size())
		 * @param targetIndex The index of the target (ending) contour pixel, with range [0, denseContour.size())
		 * @param oneWay True, to measure the appearance cost regarding the previous neighbor; False, to measure the appearance cost regarding both (previous and next) neighbors
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static unsigned int totalBackwardCost8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const unsigned int outlineStrideElements, const PixelPositions& denseContour, const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex, const bool oneWay);

		/**
		 * Returns the appearance cost between two contour points using a [1 2 1] kernel.
		 * @param frame The frame in which the contour is defined, must be valid
		 * @param width The width of the provided frame (and masks) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and mask) in pixel, with range [1, infinity)
		 * @param frameStrideElements The number of elements between two frame rows, in elements, with range [width * tChannels, infinity)
		 * @param denseContour The dense and distinct pixel contour defined in the domain of the frame, with ranges [0, width)x[0, height)
		 * @param sourceIndex The index of the source (starting) contour pixel, with range [0, denseContour.size())
		 * @param targetIndex The index of the target (ending) contour pixel, with range [0, denseContour.size())
		 * @return The appearance cost for the specified mapping, with range [0, infinity)
		 * @tparam tChannels The number of data channel the frame has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static unsigned int directMappingCostAppearance8BitPerChannelKernel3(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const PixelPositions& denseContour, const unsigned int sourceIndex, const unsigned int targetIndex);

		/**
		 * Determines the geometric cost between two contour points while both neighbor pixels are included into the cost determination.
		 * @param mapping The mapping for which the cost will be determined, must be valid
		 * @param sourceIndex The index of the source (starting) contour pixel, with range [0, mapping.size())
		 * @param targetIndex The index of the target (ending) contour pixel, with range [0, mapping.size())
		 * @return The resulting geometry cost, with range [0, infinity)
		 */
		static unsigned int neighborCostDistance(const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex);

		/**
		 * Determines the geometric cost between two contour points while the previous neighbor pixel is included into the cost determination.
		 * @param mapping The mapping for which the cost will be determined, must be valid
		 * @param sourceIndex The index of the source (starting) contour pixel, with range [0, mapping.size())
		 * @param targetIndex The index of the target (ending) contour pixel, with range [0, mapping.size())
		 * @return The resulting geometry cost, with range [0, infinity)
		 */
		static unsigned int neighborCostDistanceMinus(const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex);

		/**
		 * Determines the geometric cost between two contour points while the next neighbor pixel is included into the cost determination.
		 * @param mapping The mapping for which the cost will be determined, must be valid
		 * @param sourceIndex The index of the source (starting) contour pixel, with range [0, mapping.size())
		 * @param targetIndex The index of the target (ending) contour pixel, with range [0, mapping.size())
		 * @return The resulting geometry cost, with range [0, infinity)
		 */
		static unsigned int neighborCostDistancePlus(const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex);

		/**
		 * Determines the (signed) pixel distance along the contour between two location on the contour.
		 * @param highIndex The (higher) index of the first pixel location of the contour, with range [0, size)
		 * @param lowIndex The (lower) index of the second pixel location of the contour, with range [0, size)
		 * @param size The size of the contour in pixel, with range [1, infinity)
		 * @return The positive distance if higher >= lower; the negative distance otherwise, with range [-size/2, size/2]
		 */
		static int distanceInContour(const unsigned int highIndex, const unsigned int lowIndex, const unsigned int size);

		/**
		 * Returns whether a specified line has at least one pixel outside a specified mask.
		 * Start and end pixel point of the given line are not considered and thus may lie outside the mask.
		 * @param mask The 8 bit mask frame with 0xFF defining a non-mask pixel, must be valid
		 * @param outline The 8 bit mask frame with 0x00 defining an outline pixel, with same frame dimension and frame orientation as the mask frame, must be valid
		 * @param width The width of the specified mask frame in pixel, with range [1, infinity)
		 * @param height The height of the specified mask frame in pixel, with range [1, infinity)
		 * @param maskStrideElements The number of elements between two mask rows, in elements, with range [width, infinity)
		 * @param outlineStrideElements The number of elements between two stride rows, in elements, with range [width, infinity)
		 * @param start The start point of the line to test, with range [-1, width]x[-1, height]
		 * @param stop The stop point of the line to test, with range [-1, width]x[-1, height]
		 * @return True, if so
		 */
		static bool isLineOutsideMask(const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int maskStrideElements, const unsigned int outlineStrideElements, const PixelPosition& start, const PixelPosition& stop);

		/**
		 * Returns the square value of a given parameter.
		 * @param value The value to be squared
		 * @return Square value, with range [0, infinity)
		 */
		static inline unsigned int sqr(const int value);
};

template <unsigned int tChannels>
Indices32 ContourMapping::contour2mapping8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPositions& denseContour)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(width != 0u && height != 0u);

	Frame outlineFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	outlineFrame.setValue(0xFF);

	ocean_assert(PixelContour(denseContour).isDense());
	ocean_assert(PixelContour(denseContour).isDistinct());

	for (const PixelPosition& densePixel : denseContour)
	{
		ocean_assert(densePixel.x() < width && densePixel.y() < height);

		outlineFrame.pixel<uint8_t>(densePixel.x(), densePixel.y())[0] = 0x00;
	}

	Indices32 mapping(mappingInitialization8BitPerChannel<tChannels>(frame, mask, outlineFrame.constdata<uint8_t>(), width, height, framePaddingElements, maskPaddingElements, outlineFrame.paddingElements(), denseContour, tChannels * 35u * 35u));

	improveMapping8BitPerChannel<tChannels>(frame, mask, outlineFrame.constdata<uint8_t>(), width, height, framePaddingElements, maskPaddingElements, outlineFrame.paddingElements(), denseContour, mapping);

	return mapping;
}

template <unsigned int tChannels>
void ContourMapping::mappingInterpolation8BitPerChannel(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPositions& denseContour, const PixelBoundingBox& boundingBox, const Indices32& mapping)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(denseContour.size() == mapping.size());
	ocean_assert(boundingBox && boundingBox.right() < width && boundingBox.bottom() < height);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const unsigned int boundingBoxWidth = boundingBox.width();

	Frame holeFrame(FrameType(boundingBox.width(), boundingBox.height(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_32, tChannels + 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	ocean_assert(holeFrame.isContinuous());

	holeFrame.setValue(0x00);

	uint32_t* const hole = holeFrame.data<uint32_t>();

	const unsigned int maxSqrLength = sqr(sqr(width) + sqr(height));

	for (unsigned int n = 0u; n < (unsigned int)mapping.size(); ++n)
	{
		const unsigned int sourceIndex = n;
		const unsigned int targetIndex = mapping[sourceIndex];

		// check whether the contour pixel has no corresponding contour pixel
		if (targetIndex >= mapping.size())
		{
			continue;
		}

		ocean_assert(sourceIndex < denseContour.size());
		ocean_assert(targetIndex < denseContour.size());

		const PixelPosition& source = denseContour[sourceIndex];
		const PixelPosition& target = denseContour[targetIndex];

		ocean_assert(source.x() >= boundingBox.left());
		ocean_assert(source.y() >= boundingBox.top());
		ocean_assert(target.x() >= boundingBox.left());
		ocean_assert(target.y() >= boundingBox.top());

		const int xSource = int(source.x() - boundingBox.left());
		const int ySource = int(source.y() - boundingBox.top());

		const int xTarget = int(target.x() - boundingBox.left());
		const int yTarget = int(target.y() - boundingBox.top());

		const PixelType sourcePixel = *(const PixelType*)(frame + source.y() * frameStrideElements + source.x() * tChannels);
		const PixelType targetPixel = *(const PixelType*)(frame + target.y() * frameStrideElements + target.x() * tChannels);

		int x = xSource;
		int y = ySource;

		CV::Bresenham bresenham(x, y, xTarget, yTarget);
		unsigned int pixels = 1u;

		while (x != xTarget || y != yTarget)
		{
			++pixels;
			bresenham.findNext(x, y);
		}

		x = xSource;
		y = ySource;
		bresenham = CV::Bresenham(x, y, xTarget, yTarget);
		bresenham.findNext(x, y);
		unsigned int t = 1u;

		const unsigned int sqrLength = sqr(source.sqrDistance(target));

		ocean_assert(maxSqrLength >= sqrLength);
		const unsigned int weight = maxSqrLength / sqrLength;

		while (x != xTarget || y != yTarget)
		{
			ocean_assert(x >= 0 && x < int(width));
			ocean_assert(y >= 0 && y < int(height));

			if (mask[(y + boundingBox.top()) * maskStrideElements + x + boundingBox.left()] == 0x00)
			{
				const unsigned int t1 = pixels - t;

				uint32_t* holeLocation = hole + 4u * (y * boundingBoxWidth + x);

				*holeLocation += weight;

				for (unsigned int i = 0u; i < tChannels; ++i)
				{
					const uint32_t value = uint32_t((((uint8_t*)&sourcePixel)[i] * t1 + ((uint8_t*)&targetPixel)[i] * t) / pixels);

					holeLocation[i + 1u] += value * weight;
				}
			}

			++t;

			bresenham.findNext(x, y);
		}
	}

	for (unsigned int y = 0u; y < boundingBox.height(); ++y)
	{
		const uint32_t * holeLine = hole + 4u * (y * boundingBoxWidth);

		uint8_t* frameLine = frame + (y + boundingBox.top()) * frameStrideElements + boundingBox.left() * tChannels;
		uint8_t* maskLine = mask + (y + boundingBox.top()) * maskStrideElements + boundingBox.left();

		for (unsigned int x = 0u; x < boundingBox.width(); ++x)
		{
			if (*holeLine)
			{
				for (unsigned int i = 0u; i < tChannels; ++i)
				{
					frameLine[i] = uint8_t(holeLine[i + 1u] / holeLine[0]);
				}

				maskLine[0] = 0xFF;
			}

			holeLine += 4;
			frameLine += tChannels;
			++maskLine;
		}
	}
}

template <unsigned int tChannels>
Indices32 ContourMapping::mappingInitialization8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int outlinePaddingElements, const PixelPositions& denseContour, const unsigned int appearanceThreshold)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && outline != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int contourSize = (unsigned int)(denseContour.size());
	ocean_assert(contourSize > 3u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int outlineStrideElements = width + outlinePaddingElements;

	// initialization of an invalid mapping
	Indices32 mapping(contourSize, (unsigned int)(-1));

	// find a first mapping by direct appearance cost only
	for (unsigned int n = 0u; n < contourSize; ++n)
	{
		const PixelPosition& source = denseContour[n];

		unsigned int minCost = appearanceThreshold;
		unsigned int minIndex = (unsigned int)(-1);

		for (unsigned int i = 0u; i < contourSize; ++i)
		{
			if (ringDistance(n, i, contourSize) > 5u || denseContour[n].sqrDistance(denseContour[i]) > 25u)
			{
				const PixelPosition& target = denseContour[i];

				if (!isLineOutsideMask(mask, outline, width, height, maskStrideElements, outlineStrideElements, source, target))
				{
					const unsigned int appearanceCost = directMappingCostAppearance8BitPerChannelKernel3<tChannels>(frame, width, height, frameStrideElements, denseContour, n, i);

					if (appearanceCost <= appearanceThreshold && appearanceCost < minCost)
					{
						minCost = appearanceCost;
						minIndex = i;
					}
				}
			}
		}

		mapping[n] = minIndex;
	}

	return mapping;
}

template <unsigned int tChannels>
void ContourMapping::improveMapping8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int outlinePaddingElements, const PixelPositions& denseContour, Indices32& mapping)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && outline != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(denseContour.size() == mapping.size());

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int outlineStrideElements = width + outlinePaddingElements;

	RandomGenerator randomGenerator;

	const unsigned int contourSize = (unsigned int)denseContour.size();

	unsigned int iterations = 0u;
	unsigned int changedMapping = contourSize;
	bool firstIteration = true;

	while (changedMapping > 5u * contourSize / 100u && iterations < 50u)
	{
		changedMapping = 0u;
		++iterations;

		// forward optimization
		for (unsigned int n = 0; n < contourSize + 5u; ++n) // 5 is the extra look
		{
			const unsigned int sourceIndex = (unsigned int)modulo(int(n), int(contourSize));
			const unsigned int targetIndexStart = mapping[sourceIndex];

			// determine the current cost
			unsigned int oldCost = totalForwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, mapping[sourceIndex], firstIteration);

			// check whether a mapping has to be removed due to changed cost conditions
			if (oldCost == (unsigned int)(-1))
			{
				mapping[sourceIndex] = (unsigned int)(-1);
			}

			// propagation step from minus-neighbor
			{
				const unsigned int sourceIndexMinus = (unsigned int)modulo(int(sourceIndex) - 1, int(contourSize));
				const unsigned int targetIndexMinus = mapping[sourceIndexMinus];

				if (targetIndexMinus != (unsigned int)(-1))
				{
					// corresponding neighbor mapping
					{
						const unsigned int targetIndexMinusNeighbor = (unsigned int)modulo(int(targetIndexMinus) - 1, int(contourSize));
						const unsigned int testCost = totalForwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, targetIndexMinusNeighbor, firstIteration);
						if (testCost < oldCost)
						{
							oldCost = testCost;
							mapping[sourceIndex] = targetIndexMinusNeighbor;
						}
					}

					// direct mapping
					{
						const unsigned int testCost = totalForwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, targetIndexMinus, firstIteration);
						if (testCost < oldCost)
						{
							oldCost = testCost;
							mapping[sourceIndex] = targetIndexMinus;
						}
					}
				}
			}

			// randomly find better mappings due to lowers costs
			{
				unsigned int minCost = oldCost;
				unsigned int minTargetIndex = (unsigned int)(-1);

				for (unsigned int i = 0; i < 20u; ++i)
				{
					const unsigned int testTargetIndex = RandomI::random(randomGenerator, contourSize - 1u);
					const unsigned int testCost = totalForwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, testTargetIndex, firstIteration);

					if (testCost < minCost)
					{
						minCost = testCost;
						minTargetIndex = testTargetIndex;
					}
				}

				if (minTargetIndex != (unsigned int)(-1))
				{
					oldCost = minCost;
					mapping[sourceIndex] = minTargetIndex;
				}
			}

			// (backwards) propagation step proposing the sourceIndex as mapping index for the targetIndex or its neighbor
			{
				const unsigned int targetIndex = mapping[sourceIndex];

				// check whether the source index is not the mapping index of the target index
				if (targetIndex != (unsigned int)(-1) && sourceIndex != mapping[targetIndex])
				{
					const unsigned int oldTargetCost = totalForwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, targetIndex, mapping[targetIndex], firstIteration);
					const unsigned int testTargetCost = totalForwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, targetIndex, sourceIndex, firstIteration);

					if (testTargetCost < oldTargetCost)
					{
						mapping[targetIndex] = sourceIndex;
					}
				}
			}

			const unsigned int targetIndexEnd = mapping[sourceIndex];

			if (targetIndexStart != targetIndexEnd)
			{
				++changedMapping;
			}
		}


		// reverse optimization
		for (int n = contourSize; n > -5; --n) // 5 is the extra look
		{
			const unsigned int sourceIndex = (unsigned int)modulo(int(n), int(contourSize));
			const unsigned int targetIndexStart = mapping[sourceIndex];

			// determine the current cost
			unsigned int oldCost = totalBackwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, mapping[sourceIndex], firstIteration);

			// check whether a mapping has to be removed due to changed cost conditions
			if (oldCost == (unsigned int)(-1))
			{
				mapping[sourceIndex] = (unsigned int)(-1);
			}

			// propagation step from plus-neighbor
			{
				const unsigned int sourceIndexPlus = (unsigned int)modulo(int(sourceIndex) + 1, int(contourSize));
				const unsigned int targetIndexPlus = mapping[sourceIndexPlus];

				if (targetIndexPlus != (unsigned int)(-1))
				{
					{
						// corresponding neighbor mapping
						const unsigned int targetIndexPlusNeighbor = (unsigned int)modulo(int(targetIndexPlus) + 1, int(contourSize));
						const unsigned int testCost = totalBackwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, targetIndexPlusNeighbor, firstIteration);
						if (testCost < oldCost)
						{
							oldCost = testCost;
							mapping[sourceIndex] = targetIndexPlusNeighbor;
						}
					}

					// direct mapping
					{
						const unsigned int testCost = totalBackwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, targetIndexPlus, firstIteration);
						if (testCost < oldCost)
						{
							oldCost = testCost;
							mapping[sourceIndex] = targetIndexPlus;
						}
					}
				}
			}

			// randomly find better mappings due to lowers costs
			{
				unsigned int minCost = oldCost;
				unsigned int minTargetIndex = (unsigned int)(-1);

				for (unsigned int i = 0; i < 20u; ++i)
				{
					const unsigned int testTargetIndex = RandomI::random(randomGenerator, contourSize - 1u);
					const unsigned int testCost = totalBackwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, sourceIndex, testTargetIndex, firstIteration);

					if (testCost < minCost)
					{
						minCost = testCost;
						minTargetIndex = testTargetIndex;
					}
				}

				if (minTargetIndex != (unsigned int)(-1))
				{
					oldCost = minCost;
					mapping[sourceIndex] = minTargetIndex;
				}
			}

			// (backwards) propagation step proposing the sourceIndex as mapping index for the targetIndex
			{
				const unsigned int targetIndex = mapping[sourceIndex];

				// check whether the source index is not the mapping index of the target index
				if (targetIndex != (unsigned int)(-1) && sourceIndex != mapping[targetIndex])
				{
					const unsigned int oldTargetCost = totalBackwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, targetIndex, mapping[targetIndex], firstIteration);
					const unsigned int testTargetCost = totalBackwardCost8BitPerChannel<tChannels>(frame, mask, outline, width, height, frameStrideElements, maskStrideElements, outlineStrideElements, denseContour, mapping, targetIndex, sourceIndex, firstIteration);

					if (testTargetCost < oldTargetCost)
					{
						mapping[targetIndex] = sourceIndex;
					}
				}
			}

			const unsigned int targetIndexEnd = mapping[sourceIndex];

			if (targetIndexStart != targetIndexEnd)
			{
				++changedMapping;
			}
		}

		firstIteration = false;
	}
}

template <unsigned int tChannels>
unsigned int ContourMapping::totalForwardCost8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const unsigned int outlineStrideElements, const PixelPositions& denseContour, const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex, const bool oneWay)
{
	ocean_assert(frame != nullptr && mask != nullptr && outline != nullptr);
	ocean_assert(width * tChannels < frameStrideElements);
	ocean_assert(width < maskStrideElements);
	ocean_assert(width < outlineStrideElements);

	ocean_assert(sourceIndex != (unsigned int)(-1));

	if (targetIndex == (unsigned int)(-1))
	{
		return (unsigned int)(-1);
	}

	// test whether the offset inside the contour is large enough
	if (ringDistance(sourceIndex, targetIndex, (unsigned int)denseContour.size()) <= 5u
			&& denseContour[sourceIndex].sqrDistance(denseContour[targetIndex]) <= 25u)
	{
		return (unsigned int)(-1);
	}

	// test whether no line pixel is outside the mask
	if (isLineOutsideMask(mask, outline, width, height, maskStrideElements, outlineStrideElements, denseContour[sourceIndex], denseContour[targetIndex]))
	{
		return (unsigned int)(-1);
	}

	// the application of a use contour could improve the mapping quality
	/*unsigned int usedCounter = 0;
	for (Indices::const_iterator i = mapping.begin(); i != mapping.end(); ++i)
		if (*i == targetIndex)
			++usedCounter;*/

	const unsigned int neighborCost = oneWay ? neighborCostDistanceMinus(mapping, sourceIndex, targetIndex) : neighborCostDistance(mapping, sourceIndex, targetIndex);

	const unsigned int appearanceThreshold = tChannels * 45u * 45u; // **TODO** threshold should be a function parameter

	const unsigned int costAppearance = directMappingCostAppearance8BitPerChannelKernel3<tChannels>(frame, width, height, frameStrideElements, denseContour, sourceIndex, targetIndex);
	if (costAppearance > appearanceThreshold)
	{
		return (unsigned int)(-1);
	}

	return costAppearance + 100u * sqr(neighborCost); // + 100u * sqr(usedCounter);
}

template <unsigned int tChannels>
unsigned int ContourMapping::totalBackwardCost8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const unsigned int outlineStrideElements, const PixelPositions& denseContour, const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex, const bool oneWay)
{
	ocean_assert(sourceIndex != (unsigned int)(-1));

	if (targetIndex == (unsigned int)(-1))
	{
		return (unsigned int)(-1);
	}

	// test whether the offset inside the contour is large enough
	if (ringDistance(sourceIndex, targetIndex, (unsigned int)denseContour.size()) <= 5u
			&& denseContour[sourceIndex].sqrDistance(denseContour[targetIndex]) <= 25u)
	{
		return (unsigned int)(-1);
	}

	// test whether no line pixel is outside the mask
	if (isLineOutsideMask(mask, outline, width, height, maskStrideElements, outlineStrideElements, denseContour[sourceIndex], denseContour[targetIndex]))
	{
		return (unsigned int)(-1);
	}

	// the application of a use contour could improve the mapping quality
	/*unsigned int usedCounter = 0;
	for (Indices::const_iterator i = mapping.begin(); i != mapping.end(); ++i)
		if (*i == targetIndex)
			++usedCounter;*/

	const unsigned int neighborCost = oneWay ? neighborCostDistancePlus(mapping, sourceIndex, targetIndex) : neighborCostDistance(mapping, sourceIndex, targetIndex);

	const unsigned int appearanceThreshold = tChannels * 45u * 45u; // **TODO** threshold should be a function parameter

	const unsigned int costDirectAppearance = directMappingCostAppearance8BitPerChannelKernel3<tChannels>(frame, width, height, frameStrideElements, denseContour, sourceIndex, targetIndex);
	if (costDirectAppearance > appearanceThreshold)
	{
		return (unsigned int)(-1);
	}

	return costDirectAppearance + 100u * sqr(neighborCost); // + 100u * sqr(usedCounter);
}

template <unsigned int tChannels>
unsigned int ContourMapping::directMappingCostAppearance8BitPerChannelKernel3(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const PixelPositions& denseContour, const unsigned int sourceIndex, const unsigned int targetIndex)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert_and_suppress_unused(width * tChannels <= frameStrideElements, width);
	ocean_assert(sourceIndex < denseContour.size());
	ocean_assert(targetIndex < denseContour.size());

	const unsigned int sourceIndexMinus = (unsigned int)modulo(int(sourceIndex) - 1, int(denseContour.size()));
	const unsigned int sourceIndexPlus = (unsigned int)modulo(int(sourceIndex) + 1, int(denseContour.size()));
	ocean_assert(sourceIndexMinus < denseContour.size());
	ocean_assert(sourceIndexPlus < denseContour.size());

	const unsigned int targetIndexMinus = (unsigned int)modulo(int(targetIndex) - 1, int(denseContour.size()));
	const unsigned int targetIndexPlus = (unsigned int)modulo(int(targetIndex) + 1, int(denseContour.size()));
	ocean_assert(targetIndexMinus < denseContour.size());
	ocean_assert(targetIndexPlus < denseContour.size());

	const PixelPosition& sourceMinus = denseContour[sourceIndexMinus];
	const PixelPosition& source = denseContour[sourceIndex];
	const PixelPosition& sourcePlus = denseContour[sourceIndexPlus];
	ocean_assert(sourceMinus.x() < width);
	ocean_assert_and_suppress_unused(sourceMinus.y() < height, height);
	ocean_assert(source.x() < width);
	ocean_assert(source.y() < height);
	ocean_assert(sourcePlus.x() < width);
	ocean_assert(sourcePlus.y() < height);

	const PixelPosition& targetMinus = denseContour[targetIndexMinus];
	const PixelPosition& target = denseContour[targetIndex];
	const PixelPosition& targetPlus = denseContour[targetIndexPlus];
	ocean_assert(targetMinus.x() < width);
	ocean_assert(targetMinus.y() < height);
	ocean_assert(target.x() < width);
	ocean_assert(target.y() < height);
	ocean_assert(targetPlus.x() < width);
	ocean_assert(targetPlus.y() < height);

	const uint8_t* frameSourceMinus = frame + sourceMinus.y() * frameStrideElements + sourceMinus.x() * tChannels;
	const uint8_t* frameTargetPlus = frame + targetPlus.y() * frameStrideElements + targetPlus.x() * tChannels;
	const unsigned int ssdMinus = SumSquareDifferencesBase::buffer8BitPerChannelTemplate<tChannels>(frameSourceMinus, frameTargetPlus);

	const uint8_t* frameSource = frame + source.y() * frameStrideElements + source.x() * tChannels;
	const uint8_t* frameTarget = frame + target.y() * frameStrideElements + target.x() * tChannels;
	const unsigned int ssd = SumSquareDifferencesBase::buffer8BitPerChannelTemplate<tChannels>(frameSource, frameTarget);

	const uint8_t* frameSourcePlus = frame + sourcePlus.y() * frameStrideElements + sourcePlus.x() * tChannels;
	const uint8_t* frameTargetMinus = frame + targetMinus.y() * frameStrideElements + targetMinus.x() * tChannels;
	const unsigned int ssdPlus = SumSquareDifferencesBase::buffer8BitPerChannelTemplate<tChannels>(frameSourcePlus, frameTargetMinus);

	if (ssd > 100 && (ssdPlus > 2u * ssd || ssdMinus > 2u * ssd))
	{
		return (unsigned int)(-1);
	}

	return (ssdMinus + 2u * ssd + ssdPlus) / 4u;
}

inline unsigned int ContourMapping::sqr(const int value)
{
	return value * value;
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_CONTOUR_MAPPING_H
