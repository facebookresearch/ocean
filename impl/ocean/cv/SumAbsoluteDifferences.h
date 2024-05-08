/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_ABSOLUTE_DIFFERENCES_H
#define META_OCEAN_CV_SUM_ABSOLUTE_DIFFERENCES_H

#include "ocean/cv/CV.h"
#include "ocean/cv/SumAbsoluteDifferencesBase.h"
#include "ocean/cv/SumAbsoluteDifferencesNEON.h"
#include "ocean/cv/SumAbsoluteDifferencesSSE.h"

#include "ocean/base/DataType.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions calculation the sum of absolute differences.
 * @ingroup cv
 */
class SumAbsoluteDifferences : public SumAbsoluteDifferencesBase
{
	public:

		/**
		 * Returns the sum of absolute differences between two square image patches.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of absolute differences between an image patch and a memory buffer.
		 * @param image0 The image in which the image patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting sum of absolute differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patchBuffer8BitPerChannel(const uint8_t* const image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* const buffer1);

		/**
		 * Returns the sum of square differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @return The resulting sum of square differences
		 * @tparam tChannels The number of channels the buffers have, with range [1, infinity)
		 * @tparam tPixels The number of pixels the buffer holds, in pixels, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static inline uint32_t buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1);

		/**
		 * Returns the sum of absolute differences between two patches within an image, patch pixels outside the image will be mirrored back into the image.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize, infinity)
		 * @param height0 The height of the first image, in pixels, with range [tPatchSize, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize, infinity)
		 * @param height1 The height of the second image, in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of absolute differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Determines the sum of absolute differences between two individual frames, individually for each channel.
		 * @param firstFrame The first frame for which the absolute difference will be determined, must be valid
		 * @param secondFrame The second frame for which the absolute difference will be determined, must be valid
		 * @param width The with of the first frame and second frame in pixel, with range [1, infinity)
		 * @param height The height of the first frame and second frame in pixel, with range [1, infinity)
		 * @param absoluteDifferences The resulting absolute differences, one for each channel, must be valid, with range [0, infinity)
		 * @param firstFramePaddingElements The number of optional padding elements at the end of each row of the first frame, in elements, with range [0, infinity)
		 * @param secondFramePaddingElements The number of optional padding elements at the end of each row of the second frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t' or 'float'
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void determine(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, typename AbsoluteDifferenceValueTyper<T>::Type* absoluteDifferences, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements);

		/**
		 * Determines the sum of absolute differences between two individual frames, individually for each plane, and individually for each channel.
		 * @param firstFrame The first frame for which the absolute difference will be determined, must be valid, must have an element type of FrameType::DT_UNSIGNED_INTEGER_8.
		 * @param secondFrame The second frame for which the absolute difference will be determined, must be valid, must have the same frame type as 'firstFrame'
		 * @param absoluteDifferences The resulting absolute differences, one for each plane and channel; first all differences for the first plane, then all differences for the second plane etc., with range [0, infinity)
		 * @return True, if succeeded
		 */
		static inline bool determine(const Frame& firstFrame, const Frame& secondFrame, Indices32& absoluteDifferences);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumAbsoluteDifferences::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize);
	ocean_assert(width1 >= tPatchSize);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerY0 >= tPatchSize_2);
	ocean_assert(centerX1 >= tPatchSize_2 && centerY1 >= tPatchSize_2);

	ocean_assert(centerX0 < width0 - tPatchSize_2);
	ocean_assert(centerX1 < width1 - tPatchSize_2);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const uint8_t* const patch0 = image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels;
	const uint8_t* const patch1 = image1 + (centerY1 - tPatchSize_2) * image1StrideElements + (centerX1 - tPatchSize_2) * tChannels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tPatchSize >= 5u)
	{
		return SumAbsoluteDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		return SumAbsoluteDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumAbsoluteDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumAbsoluteDifferences::patchBuffer8BitPerChannel(const uint8_t* const image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* const buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(width0 >= tPatchSize);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerY0 >= tPatchSize_2);

	ocean_assert(centerX0 < width0 - tPatchSize_2);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;

	const uint8_t* const patch0 = image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tPatchSize >= 5u)
	{
		return SumAbsoluteDifferencesSSE::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, image0StrideElements);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		return SumAbsoluteDifferencesNEON::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, image0StrideElements);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumAbsoluteDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(patch0, buffer1, image0StrideElements);
}

template <unsigned int tChannels, unsigned int tPixels>
inline uint32_t SumAbsoluteDifferences::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid pixel number!");

	constexpr unsigned int tElements = tChannels * tPixels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tElements >= 15u)
	{
		return SumAbsoluteDifferencesSSE::buffer8BitPerChannel<tElements>(buffer0, buffer1);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tElements >= 8u)
	{
		return SumAbsoluteDifferencesNEON::buffer8BitPerChannel<tElements>(buffer0, buffer1);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumAbsoluteDifferencesBase::buffer8BitPerChannelTemplate<tElements>(buffer0, buffer1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t SumAbsoluteDifferences::patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	return SumAbsoluteDifferencesBase::patchMirroredBorder8BitPerChannelTemplate<tChannels>(image0, image1, tPatchSize, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

template <typename T, unsigned int tChannels>
void SumAbsoluteDifferences::determine(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, typename AbsoluteDifferenceValueTyper<T>::Type* absoluteDifferences, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements)
{
	ocean_assert(firstFrame != nullptr);
	ocean_assert(secondFrame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(absoluteDifferences != nullptr);

	typedef typename AbsoluteDifferenceValueTyper<T>::Type TResult;
	typedef typename DifferenceValueTyper<T>::Type TDifferenceType;

	const unsigned int firstFrameStrideElements = width * tChannels + firstFramePaddingElements;
	const unsigned int secondFrameStrideElements = width * tChannels + secondFramePaddingElements;

	TResult result[tChannels];

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		result[n] = TResult(0);
	}

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] += TResult(NumericT<TDifferenceType>::abs(TDifferenceType(firstFrame[x * tChannels + n] - secondFrame[x * tChannels + n])));
			}
		}

		firstFrame += firstFrameStrideElements;
		secondFrame += secondFrameStrideElements;
	}

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		absoluteDifferences[n] = result[n];
	}
}

inline bool SumAbsoluteDifferences::determine(const Frame& firstFrame, const Frame& secondFrame, Indices32& absoluteDifferences)
{
	ocean_assert(firstFrame.isValid() && secondFrame.isValid());
	ocean_assert(firstFrame.frameType() == secondFrame.frameType());
	ocean_assert(firstFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	absoluteDifferences.clear();

	if (!firstFrame.isValid() || firstFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8 || firstFrame.frameType() != secondFrame.frameType())
	{
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < firstFrame.numberPlanes(); ++planeIndex)
	{
		switch (firstFrame.planeChannels(planeIndex))
		{
			case 1u:
			{
				unsigned int planeAbsoluteDifference = 0u;
				determine<uint8_t, 1u>(firstFrame.constdata<uint8_t>(planeIndex), secondFrame.constdata<uint8_t>(planeIndex), firstFrame.planeWidth(planeIndex), firstFrame.planeHeight(planeIndex), &planeAbsoluteDifference, firstFrame.paddingElements(planeIndex), secondFrame.paddingElements(planeIndex));
				absoluteDifferences.emplace_back(planeAbsoluteDifference);
				break;
			}

			case 2u:
			{
				unsigned int planeAbsoluteDifferences[2] = {0u, 0u};
				determine<uint8_t, 2u>(firstFrame.constdata<uint8_t>(planeIndex), secondFrame.constdata<uint8_t>(planeIndex), firstFrame.planeWidth(planeIndex), firstFrame.planeHeight(planeIndex), planeAbsoluteDifferences, firstFrame.paddingElements(planeIndex), secondFrame.paddingElements(planeIndex));
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[0]);
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[1]);
				break;
			}

			case 3u:
			{
				unsigned int planeAbsoluteDifferences[3] = {0u, 0u, 0u};
				determine<uint8_t, 3u>(firstFrame.constdata<uint8_t>(planeIndex), secondFrame.constdata<uint8_t>(planeIndex), firstFrame.planeWidth(planeIndex), firstFrame.planeHeight(planeIndex), planeAbsoluteDifferences, firstFrame.paddingElements(planeIndex), secondFrame.paddingElements(planeIndex));
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[0]);
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[1]);
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[2]);
				break;
			}

			case 4u:
			{
				unsigned int planeAbsoluteDifferences[4] = {0u, 0u, 0u, 0u};
				determine<uint8_t, 4u>(firstFrame.constdata<uint8_t>(planeIndex), secondFrame.constdata<uint8_t>(planeIndex), firstFrame.planeWidth(planeIndex), firstFrame.planeHeight(planeIndex), planeAbsoluteDifferences, firstFrame.paddingElements(planeIndex), secondFrame.paddingElements(planeIndex));
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[0]);
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[1]);
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[2]);
				absoluteDifferences.emplace_back(planeAbsoluteDifferences[3]);
				break;
			}

			default:
				ocean_assert(false && "Invalid channel number!");
				return false;
		}
	}

	return true;
}

}

}

#endif // META_OCEAN_CV_SUM_ABSOLUTE_DIFFERENCES_H
