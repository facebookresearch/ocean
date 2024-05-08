/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_BASE_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_BASE_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/ZeroMeanSumSquareDifferences.h"

#include "ocean/cv/advanced/AdvancedSumSquareDifferencesBase.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements functions calculating the zero-mean sum of square differences.
 * @ingroup cvadvanced
 */
class AdvancedZeroMeanSumSquareDifferencesBase
{
	public:

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY0 Vertical center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param centerX1 Horizontal center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2)
		 * @param centerY0 Vertical center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2)
		 * @param centerX1 Horizontal center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Determines the partial zero-mean ssd of two image patches between two frames while for each frame a binary mask specifies whether a pixel is valid or skipped.
		 * Only pixels with two corresponding mask pixels in both frames are used for mean and ssd calculation.<br>
		 * Further, the size of the image patch may be arbitrary (even or odd) and parts of the patch may lie outside the frames (as long as one pixel lies inside each frame).<br>
		 * Beware: The patch coordinate is not determined by the center position but by the upper left position of the patch (as the patch may have even dimensions).<br>
		 * @param frame0 The first frame for ssd determination
		 * @param frame1 The second frame for ssd determination
		 * @param mask0 The 8 bit binary mask corresponding to the first frame (with same dimension), pixels not equal to 0x00 are valid in frame0
		 * @param mask1 The 8 bit binary mask corresponding to the second frame (with same dimension), pixels not equal to 0x00 are valid in frame1
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param left0 The left position of the patch in the first frame, with range [-sizeX + 1, width0)
		 * @param top0 The top position of the patch in the first frame, with range[-sizeY + 1, height0)
		 * @param left1 The left position of the patch in the second frame, with range [-sizeX + 1, width1)
		 * @param top1 The top position of the patch in the second frame, with range [-sizeY + 1, height0),
		 * @return A pair holding the resulting ssd in the first parameter, and the number of valid pixels (that have been used to determined the ssd) in the second parameter
		 * @tparam tChannels The number of data channels of the frames
		 */
		template <unsigned int tChannels>
		static inline IndexPair32 determine8BitPerChannelPartialTemplate(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, unsigned int sizeX, unsigned int sizeY, const int left0, const int top0, const int left1, const int top1);

	protected:

		/**
		 * Determines the mean values of given image patch with arbitrary sizes while a binary mask specifies whether a pixel is valid or skipped.
		 * A pixel is valid if the pixel is a non-mask pixels and lies inside the frame.
		 * @param frame The frame in which the mean values are determined
		 * @param mask The 8 bit binary mask defining whether a pixel is valid or skipped, 0x00 specifies a mask pixel
		 * @param width The width of the given frame (and the mask) in pixel, with range [1, infinity)
		 * @param height The height of the given frame (and the mask) in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param left The left position of the patch in the frame, with range [-sizeX + 1, width)
		 * @param top The top position of the patch in the frame, with range[-sizeY + 1, height)
		 * @param sums The resulting sum values, individual for each frame channel
		 * @return The number of valid pixels
		 */
		template <unsigned int tChannels>
		static inline unsigned int sum8BitPerChannelPartialTemplate(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int sizeX, const unsigned int sizeY, const int left, const int top, unsigned int sums[tChannels]);

		/**
		 * Determines the mean values for two given image patches in two individual frames with arbitrary sizes while two binary masks specifies whether a pixel is valid or skipped.
		 * A pixel is valid if in both patches the corresponding pixel is a non-mask pixels and lies inside both frames.<br>
		 * @param frame0 The first frame in which the first mean values are determined
		 * @param frame1 The second frame in which the second mean values are determined
		 * @param mask0 The first 8 bit binary mask defining whether a pixel is valid or skipped in the first frame, 0x00 specifies a mask pixel
		 * @param mask1 The second 8 bit binary mask defining whether a pixel is valid or skipped in the second frame, 0x00 specifies a mask pixel
		 * @param width0 Width of the first frame (and the first mask) in pixel, with range [1, infinity)
		 * @param height0 Height of the first frame (and the first mask) in pixel, with range [1, infinity)
		 * @param width1 Width of the second frame (and the second mask) in pixel, with range [1, infinity)
		 * @param height1 Height of the second frame (and the second mask) in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param left0 The left position of the patch in the first frame, with range [-sizeX + 1, width0)
		 * @param top0 The top position of the patch in the first frame, with range[-sizeY + 1, height0)
		 * @param left1 The left position of the patch in the second frame, with range [-sizeX + 1, width1)
		 * @param top1 The top position of the patch in the second frame, with range[-sizeY + 1, height1)
		 * @param sums0 The resulting sum values from the first frame, individual for each frame channel
		 * @param sums1 The resulting sum values from the second frame, individual for each frame channel
		 * @return The number of valid pixels
		 */
		template <unsigned int tChannels>
		static inline unsigned int sum8BitPerChannelPartialTemplate(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, unsigned int sizeX, unsigned int sizeY, const int left0, const int top0, const int left1, const int top1, unsigned int sums0[tChannels], unsigned int sums1[tChannels]);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);
	ocean_assert(width1 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u));
	ocean_assert_and_suppress_unused(centerY0 >= Scalar(tPatchSize_2), tPatchSize_2);

	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));
	ocean_assert(centerY1 >= Scalar(tPatchSize_2));

	constexpr unsigned int tPatchPixels = tPatchSize * tPatchSize;
	constexpr unsigned int tPatchElements = tPatchPixels * tChannels;

	uint8_t target[tPatchElements * 2u];

	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image0, width0, image0PaddingElements, target, Vector2(centerX0, centerY0));
	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image1, width1, image1PaddingElements, target + tPatchElements, Vector2(centerX1, centerY1));

	return ZeroMeanSumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, target + tPatchElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize);
	ocean_assert(width1 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);
	ocean_assert_and_suppress_unused(centerY0 >= tPatchSize_2, tPatchSize_2);

	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));
	ocean_assert(centerY1 >= Scalar(tPatchSize_2));

	constexpr unsigned int tPatchPixels = tPatchSize * tPatchSize;
	constexpr unsigned int tPatchElements = tPatchPixels * tChannels;

	uint8_t target[tPatchElements * 2u];

	constexpr unsigned int targetPaddingElements = 0u;

	CV::FrameConverter::patchFrame(image0, target, width0, tChannels, centerX0, centerY0, tPatchSize, image0PaddingElements, targetPaddingElements);
	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image1, width1, image1PaddingElements, target + tPatchElements, Vector2(centerX1, centerY1));

	return ZeroMeanSumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, target + tPatchElements);
}

template <unsigned int tChannels>
inline IndexPair32 AdvancedZeroMeanSumSquareDifferencesBase::determine8BitPerChannelPartialTemplate(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int sizeX, const unsigned int sizeY, const int left0, const int top0, const int left1, const int top1)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");

	ocean_assert(frame0 && frame1);
	ocean_assert(mask0 && mask1);

	ocean_assert(sizeX >= 1u && sizeY >= 1u);
	ocean_assert(width0 >= sizeX && width1 >= sizeX);
	ocean_assert(height0 >= sizeY && height1 >= sizeY);

	ocean_assert(left0 >= -int(sizeX) + 1 && left0 < int(width0));
	ocean_assert(top0 >= -int(sizeY) + 1 && top0 < int(height0));
	ocean_assert(left1 >= -int(sizeX) + 1 && left1 < int(width1));
	ocean_assert(top1 >= -int(sizeY) + 1 && top1 < int(height1));

	const int patchLeftBorder = max(0, max(-left0, -left1));
	const int patchTopBorder = max(0, max(-top0, -top1));

	const int patchRightBorder = max(0, max(left0 + int(sizeX) - int(width0), left1 + int(sizeX) - int(width1)));
	const int patchBottomBorder = max(0, max(top0 + int(sizeY) - int(height0), top1 + int(sizeY) - int(height1)));

	ocean_assert(patchLeftBorder >= 0 && patchLeftBorder < int(sizeX));
	ocean_assert(patchTopBorder >= 0 && patchTopBorder < int(sizeY));
	ocean_assert(patchRightBorder >= 0 && patchRightBorder < int(sizeX));
	ocean_assert(patchBottomBorder >= 0 && patchBottomBorder < int(sizeY));

	const int pLeft0 = left0 + patchLeftBorder;
	const int pLeft1 = left1 + patchLeftBorder;

	const int pTop0 = top0 + patchTopBorder;
	const int pTop1 = top1 + patchTopBorder;

	const unsigned int pSizeX = sizeX - patchLeftBorder - patchRightBorder;
	const unsigned int pSizeY = sizeY - patchTopBorder - patchBottomBorder;

	if (pSizeX > sizeX || pSizeY > sizeY)
	{
		return IndexPair32(0u, 0u);
	}

	unsigned int means0[tChannels] = {0u};
	unsigned int means1[tChannels] = {0u};

	const unsigned int pixels = sum8BitPerChannelPartialTemplate<tChannels>(frame0, frame1, mask0, mask1, width0, height0, width1, height1, sizeX, sizeY, left0, top0, left1, top1, means0, means1);

	if (pixels == 0u)
	{
		return IndexPair32(0u, 0u);
	}

	int means1_0[tChannels];

	for (unsigned int n = 0u; n < tChannels; ++n)
		means1_0[n] = int((means1[n] + (pixels / 2u)) / pixels) - int((means0[n] + (pixels / 2u)) / pixels);

	unsigned int ssd = 0u;

	frame0 += (pTop0 * int(width0) + pLeft0) * int(tChannels);
	frame1 += (pTop1 * int(width1) + pLeft1) * int(tChannels);

	mask0 += (pTop0 * int(width0) + pLeft0);
	mask1 += (pTop1 * int(width1) + pLeft1);

	const uint8_t* const mask0End = mask0 + pSizeY * width0;

	while (mask0 != mask0End)
	{
		ocean_assert(mask0 < mask0End);

		const uint8_t* const mask0RowEnd = mask0 + pSizeX;

		while (mask0 != mask0RowEnd)
		{
			ocean_assert(mask0 < mask0End);
			ocean_assert(mask0 < mask0RowEnd);

			const unsigned int factor = (*mask0 * *mask1) != 0u;

			for (unsigned int n = 0u; n < tChannels; ++n)
				ssd += sqr(int(frame0[n]) - int(frame1[n]) + means1_0[n]) * factor;

			frame0 += tChannels;
			frame1 += tChannels;

			mask0++;
			mask1++;
		}

		frame0 += (width0 - pSizeX) * tChannels;
		frame1 += (width1 - pSizeX) * tChannels;

		mask0 += (width0 - pSizeX);
		mask1 += (width1 - pSizeX);
	}

	return std::make_pair(ssd, pixels);
}

template <unsigned int tChannels>
inline unsigned int AdvancedZeroMeanSumSquareDifferencesBase::sum8BitPerChannelPartialTemplate(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int sizeX, const unsigned int sizeY, const int left, const int top, unsigned int sums[tChannels])
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");

	ocean_assert(frame && mask);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(sizeX >= 1u && sizeY >= 1u);

	ocean_assert(width >= sizeX &&  height >= sizeY);
	ocean_assert(left >= -int(sizeX) + 1 && left < int(width));
	ocean_assert(top >= -int(sizeY) + 1 && top < int(height));

	const int patchLeftBorder = max(0, -left);
	const int patchTopBorder = max(0, -top);

	const int patchRightBorder = max(0, left + int(sizeX) - int(width));
	const int patchBottomBorder = max(0, top + int(sizeY) - int(height));

	ocean_assert(patchLeftBorder >= 0 && patchLeftBorder < int(sizeX));
	ocean_assert(patchTopBorder >= 0 && patchTopBorder < int(sizeY));
	ocean_assert(patchRightBorder >= 0 && patchRightBorder < int(sizeX));
	ocean_assert(patchBottomBorder >= 0 && patchBottomBorder < int(sizeY));

	const int pLeft = left + patchLeftBorder;
	const int pTop = top + patchTopBorder;

	const unsigned int pSizeX = sizeX - patchLeftBorder - patchRightBorder;
	const unsigned int pSizeY = sizeY - patchTopBorder - patchBottomBorder;

	if (pSizeX > sizeX || pSizeY > sizeY)
		return 0u;

	unsigned int pixels = 0u;

	for (unsigned int n = 0u; n < tChannels; ++n)
		sums[n] = 0u;

	frame += (pTop * int(width) + pLeft) * int(tChannels);
	mask += (pTop * int(width) + pLeft);

	const uint8_t* const maskEnd = mask + pSizeY * width;

	while (mask != maskEnd)
	{
		ocean_assert(mask < maskEnd);

		const uint8_t* const maskRowEnd = mask + pSizeX;

		while (mask != maskRowEnd)
		{
			ocean_assert(mask < maskEnd);
			ocean_assert(mask < maskRowEnd);

			const unsigned int factor = *mask != 0u;

			for (unsigned int n = 0u; n < tChannels; ++n)
				sums[n] += frame[n] * factor;

			pixels += factor;

			frame += tChannels;
			mask++;
		}

		frame += int(width - pSizeX) * int(tChannels);
		mask += int(width - pSizeX);
	}

	return pixels;
}

template <unsigned int tChannels>
inline unsigned int AdvancedZeroMeanSumSquareDifferencesBase::sum8BitPerChannelPartialTemplate(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, unsigned int sizeX, unsigned int sizeY, const int left0, const int top0, const int left1, const int top1, unsigned int sums0[tChannels], unsigned int sums1[tChannels])
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");

	ocean_assert(frame0 && frame1);
	ocean_assert(mask0 && mask1);

	ocean_assert(sizeX >= 1u && sizeY >= 1u);
	ocean_assert(width0 >= sizeX && width1 >= sizeX);
	ocean_assert(height0 >= sizeY && height1 >= sizeY);

	ocean_assert(left0 >= -int(sizeX) + 1 && left0 < int(width0));
	ocean_assert(top0 >= -int(sizeY) + 1 && top0 < int(height0));
	ocean_assert(left1 >= -int(sizeX) + 1 && left1 < int(width1));
	ocean_assert(top1 >= -int(sizeY) + 1 && top1 < int(height1));

	const int patchLeftBorder = max(0, max(-left0, -left1));
	const int patchTopBorder = max(0, max(-top0, -top1));

	const int patchRightBorder = max(0, max(left0 + int(sizeX) - int(width0), left1 + int(sizeX) - int(width1)));
	const int patchBottomBorder = max(0, max(top0 + int(sizeY) - int(height0), top1 + int(sizeY) - int(height1)));

	ocean_assert(patchLeftBorder >= 0 && patchLeftBorder < int(sizeX));
	ocean_assert(patchTopBorder >= 0 && patchTopBorder < int(sizeY));
	ocean_assert(patchRightBorder >= 0 && patchRightBorder < int(sizeX));
	ocean_assert(patchBottomBorder >= 0 && patchBottomBorder < int(sizeY));

	const int pLeft0 = left0 + patchLeftBorder;
	const int pLeft1 = left1 + patchLeftBorder;

	const int pTop0 = top0 + patchTopBorder;
	const int pTop1 = top1 + patchTopBorder;

	const unsigned int pSizeX = sizeX - patchLeftBorder - patchRightBorder;
	const unsigned int pSizeY = sizeY - patchTopBorder - patchBottomBorder;

	if (pSizeX > sizeX || pSizeY > sizeY)
		return 0u;

	for (unsigned int n = 0u; n < tChannels; ++n)
		sums0[n] = 0u;
	for (unsigned int n = 0u; n < tChannels; ++n)
		sums1[n] = 0u;

	unsigned int pixels = 0u;

	frame0 += (pTop0 * int(width0) + pLeft0) * int(tChannels);
	frame1 += (pTop1 * int(width1) + pLeft1) * int(tChannels);

	mask0 += (pTop0 * int(width0) + pLeft0);
	mask1 += (pTop1 * int(width1) + pLeft1);

	const uint8_t* const mask0End = mask0 + pSizeY * width0;

	while (mask0 != mask0End)
	{
		ocean_assert(mask0 < mask0End);

		const uint8_t* const mask0RowEnd = mask0 + pSizeX;

		while (mask0 != mask0RowEnd)
		{
			ocean_assert(mask0 < mask0End);
			ocean_assert(mask0 < mask0RowEnd);

			const unsigned int factor = (*mask0 * *mask1) != 0u;

			for (unsigned int n = 0u; n < tChannels; ++n)
				sums0[n] += frame0[n] * factor;

			for (unsigned int n = 0u; n < tChannels; ++n)
				sums1[n] += frame1[n] * factor;

			pixels += factor;

			frame0 += tChannels;
			frame1 += tChannels;

			mask0++;
			mask1++;
		}

		frame0 += int(width0 - pSizeX) * int(tChannels);
		frame1 += int(width1 - pSizeX) * int(tChannels);

		mask0 += int(width0 - pSizeX);
		mask1 += int(width1 - pSizeX);
	}

	return pixels;
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_BASE_H
