/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_MASK_ANALYZER_H
#define META_OCEAN_CV_MASK_ANALYZER_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements basic maks analyzing functions.
 * More advanced mask analyzing functions are available in CV::Segmentation::MaskAnalyzer.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT MaskAnalyzer
{
	public:

		/**
		 * Detects the smallest axis-aligned bounding box enclosing all isolated mask islands in a binary (but 8-bit) mask frame.
		 * @param mask The 8 bit mask frame in which the enclosing bounding box will be determined, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param nonMaskPixel The value of pixels not part of the mask, all other values will be mask pixels, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, with range [0, infinity)
		 * @return Resulting mask bounding box, invalid if no mask could be found
		 * @see CV::Segmentation::MaskAnalyzer::detectBoundingBoxes().
		 */
		static PixelBoundingBox detectBoundingBox(const uint8_t* mask, const unsigned int width, const unsigned int height, const uint8_t nonMaskPixel, const unsigned int maskPaddingElements);

		/**
		 * Detects the smallest axis-aligned bounding box enclosing all isolated mask islands inside a given mask frame using a rough approximation of the bounding box.
		 * @param mask The 8 bit mask frame in which the bounding box will be determined, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param rough Rough approximation of the final bounding box, must be valid
		 * @param maxUncertainty Maximal pixel uncertainty of the rough box guess, with range [1, infinity)
		 * @param nonMaskPixel The value of pixel not part of the mask, all other values will be mask pixels, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, with range [0, infinity)
		 * @return Resulting mask bounding box, invalid if no mask could be found
		 */
		static PixelBoundingBox detectBoundingBox(const uint8_t* const mask, const unsigned int width, const unsigned int height, const PixelBoundingBox& rough, const unsigned int maxUncertainty, const uint8_t nonMaskPixel = 0xFFu, const unsigned int maskPaddingElements = 0u);

		/**
		 * Detects the smallest bounding box enclosing all opaque pixels in a given frame.
		 * @param frame The frame in which the bounding box enclosing all opaque pixels is determined, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param alphaChannelIndex The index of the alpha channel, with range [0, channels)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param fullTransparentValue The alpha pixel value of a full transparent pixel
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The smallest bounding box enclosing all opaque pixels, an invalid bounding box if the frames does not contain an opaque pixel
		 * @tparam T Data type of each pixel channel value
		 */
		template <typename T>
		static PixelBoundingBox detectOpaqueBoundingBox(const T* frame, const unsigned int width, const unsigned int height, const unsigned int alphaChannelIndex, const unsigned int channels, const T fullTransparentValue, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Detects the smallest bounding box of opaque pixels in a given frame.
		 * @param frame The frame in the opaque pixels are determined
		 * @param transparentIs0xFF True, if 0xFF is interpreted as a full transparent pixel
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting smallest bounding box, an invalid bounding box if no opaque pixel has been found
		 */
		static PixelBoundingBox detectOpaqueBoundingBox(const Frame& frame, const bool transparentIs0xFF, Worker* worker = nullptr);

		/**
		 * Returns whether a mask frame has at least one pixel with a specific mask value.
		 * @param mask The 8 bit mask frame in which the pixel value is sought, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param value The value to be sought, with range [0, 255]
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param boundingBox Optional bounding box to speedup the computation, must fit into the given frame if defined
		 * @return True, if at least one mask pixel has the specified value
		 */
		static inline bool hasValue(const uint8_t* mask, const unsigned int width, const unsigned int height, const uint8_t value, const unsigned int maskPaddingElements, const PixelBoundingBox& boundingBox = CV::PixelBoundingBox());

	protected:

		/**
		 * Detects the smallest bounding box enclosing all opaque pixels in a subset of a given frame.
		 * @param frame The frame in which the bounding box enclosing all opaque pixels is determined, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param alphaChannelIndex The index of the alpha channel, with range [0, channels - 1]
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param fullTransparentValue The alpha pixel value of a full transparent pixel
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param left Resulting left bounding box position
		 * @param top Resulting top bounding box position
		 * @param right Resulting right bounding box position
		 * @param bottom Resulting bottom bounding box position
		 * @param lock Optional lock if this function is executed distributed within several threads
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam T Data type of each pixel channel value
		 */
		template <typename T>
		static void detectOpaqueBoundingBoxSubset(const T* frame, const unsigned int width, const unsigned int height, const unsigned int alphaChannelIndex, const unsigned int channels, const T fullTransparentValue, const unsigned int framePaddingElements, unsigned int* left, unsigned int* top, unsigned int* right, unsigned int* bottom, Lock* lock, const unsigned int firstRow, const unsigned int numberRows);
};

template <typename T>
PixelBoundingBox MaskAnalyzer::detectOpaqueBoundingBox(const T* frame, const unsigned int width, const unsigned int height, const unsigned int alphaChannelIndex, const unsigned int channels, const T fullTransparentValue, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(frame != nullptr);
	ocean_assert(channels != 0u);
	ocean_assert(alphaChannelIndex < channels);

	unsigned int left = (unsigned int)(-1);
	unsigned int top = (unsigned int)(-1);
	unsigned int right = 0u;
	unsigned int bottom = 0u;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&MaskAnalyzer::detectOpaqueBoundingBoxSubset<T>, frame, width, height, alphaChannelIndex, channels, fullTransparentValue, framePaddingElements, &left, &top, &right, &bottom, &lock, 0u, 0u), 0u, height, 12u, 13u, 20u);
	}
	else
	{
		detectOpaqueBoundingBoxSubset<T>(frame, width, height, alphaChannelIndex, channels, fullTransparentValue, framePaddingElements, &left, &top, &right, &bottom, nullptr, 0u, height);
	}

	return PixelBoundingBox(left, top, right, bottom);
}

template <typename T>
void MaskAnalyzer::detectOpaqueBoundingBoxSubset(const T* frame, const unsigned int width, const unsigned int height, const unsigned int alphaChannelIndex, const unsigned int channels, const T fullTransparentValue, const unsigned int framePaddingElements, unsigned int* left, unsigned int* top, unsigned int* right, unsigned int* bottom, Lock* lock, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr);
	ocean_assert(alphaChannelIndex < channels);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int frameStrideElements = width * channels + framePaddingElements;

	unsigned int localLeft = (unsigned int)(-1);
	unsigned int localRight = 0u;
	unsigned int localTop = (unsigned int)(-1);
	unsigned int localBottom = 0u;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const T* frameRow = frame + y * frameStrideElements + alphaChannelIndex;

		for (unsigned int x = 0u; x < width; ++x)
		{
			if (*frameRow != fullTransparentValue)
			{
				if (x < localLeft)
				{
					localLeft = x;
				}

				if (x > localRight)
				{
					localRight = x;
				}

				if (y < localTop)
				{
					localTop = y;
				}

				if (y > localBottom)
				{
					localBottom = y;
				}
			}

			frameRow += channels;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	*left = min(*left, localLeft);
	*top = min(*top, localTop);

	*right = max(*right, localRight);
	*bottom = max(*bottom, localBottom);
}

inline bool MaskAnalyzer::hasValue(const uint8_t* mask, const unsigned int width, const unsigned int height, const uint8_t value, const unsigned int maskPaddingElements, const PixelBoundingBox& boundingBox)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);

	const Frame maskFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), mask, Frame::CM_USE_KEEP_LAYOUT, maskPaddingElements);

	const Frame::PixelType<uint8_t, 1u> pixelValue = {{value}};

	if (boundingBox.isValid())
	{
		return maskFrame.subFrame(boundingBox.left(), boundingBox.top(), boundingBox.width(), boundingBox.height(), Frame::CM_USE_KEEP_LAYOUT).containsValue<uint8_t, 1u>(pixelValue);
	}

	return maskFrame.containsValue<uint8_t, 1u>(pixelValue);
}

}

}

#endif // META_OCEAN_CV_MASK_ANALYZER_H
