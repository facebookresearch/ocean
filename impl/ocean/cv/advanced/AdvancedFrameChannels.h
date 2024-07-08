/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_CHANNLES_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_CHANNLES_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/NEON.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements advanced frame channel conversion, transformation and extraction functions.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameChannels
{
	public:

		/**
		 * Definition of a constant to specify that the number of channels are not known at compile time but at runtime only.
		 */
		static constexpr unsigned int CHANNELS_NOT_KNOWN_AT_COMPILE_TIME = 0u;

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * In addition to CV::FrameChannels::separateTo1Channel(), this function supports multiplication factors for source and target elements.<br>
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * uint8_t* sourceFrame = ...;
		 * const unsigned int sourceFramePaddingElements = ...;
		 *
		 * constexpr unsigned int channels = 2u;
		 *
		 * const float* targetFrames[channels] = {..., ...};
		 * const unsigned int targetFramesPaddingElements[2] = {..., ...};
		 *
		 * constexpr uint8_t sourceFactor = 1u;
		 * constexpr uint8_t targetFactor = 1.0f / 255.0f;
		 *
		 * separateTo1Channel<uint8_t, float, channels>(sourceFrame, targetFrames, width, height, channels, sourceFactor, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements);
		 * @endcode
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the source frame has, with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity), nullptr if all are zero
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels The number of source frames (and target channels) if known at compile time; otherwise CHANNELS_NOT_KNOWN_AT_COMPILE_TIME == 0, if know at compile time must be identical with 'channels'
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels = CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>
		static void separateTo1Channel(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements);

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * In addition to CV::FrameChannels::separateTo1Channel(), this function supports multiplication factors for source and target elements.<br>
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * const uint8_t* sourceFrame = ...;
		 * const unsigned int sourceFramePaddingElements = ...;
		 *
		 * float* targetFrame0 = ...;
		 * float* targetFrame1 = ...;
		 * const unsigned int targetFramePaddingElements0 = ...;
		 * const unsigned int targetFramePaddingElements1 = ...;
		 *
		 * constexpr uint8_t sourceFactor = 1u;
		 * constexpr uint8_t targetFactor = 1.0f / 255.0f;
		 *
		 * separateTo1Channel<uint8_t, float>(sourceFrame, {targetFrame0, targetFrame1}, width, height, sourceFactor, targetFactor, sourceFramePaddingElements, {targetFramePaddingElements0, targetFramePaddingElements1});
		 * @endcode
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void separateTo1Channel(const TSource* const sourceFrame, const std::initializer_list<TTarget*>& targetFrames, const unsigned int width, const unsigned int height, const TSource sourceFactor, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const std::initializer_list<const unsigned int>& targetFramesPaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * In addition to CV::FrameChannels::targetFramePaddingElements(), this function supports multiplication factors for source and target elements.<br>
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * const float* sourceFrames[2] = {..., ...};
		 * const unsigned int sourceFramesPaddingElements[2] = {..., ...};
		 *
		 * uint8_t* targetFrame = ...;
		 * const unsigned int targetFramePaddingElements = ...;
		 *
		 * constexpr float sourceFactor = 255.0f;
		 * constexpr uint8_t targetFactor = 1u;
		 *
		 * zipChannels<float, uint8_t>(sourceFrames, targetFrame, width, height, 2u, sourceFactor, targetFactor, sourceFramesPaddingElements, targetFramePaddingElements);
		 * @endcode
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param channels The number of provided source frames (and the number of channels the target frame will have), with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity), nullptr if all are zero
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels The number of source frames (and target channels) if known at compile time; otherwise CHANNELS_NOT_KNOWN_AT_COMPILE_TIME == 0, if know at compile time must be identical with 'channels'
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels = CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>
		static void zipChannels(const TSource* const* const sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * In addition to CV::FrameChannels::targetFramePaddingElements(), this function supports multiplication factors for source and target elements.<br>
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * const float* sourceFrame0 = ...;
		 * const float* sourceFrame1 = ...;
		 * const unsigned int sourceFramePaddingElements0 = ...;
		 * const unsigned int sourceFramePaddingElements1 = ...;
		 *
		 * uint8_t* targetFrame = ...;
		 * const unsigned int targetFramePaddingElements = ...;
		 *
		 * constexpr float sourceFactor = 255.0f;
		 * constexpr uint8_t targetFactor = 1u;
		 *
		 * zipChannels<float, uint8_t>({sourceFrame0, sourceFrame1}, targetFrame, width, height, sourceFactor, targetFactor, {sourceFramePaddingElements0, sourceFramePaddingElements1}, targetFramePaddingElements);
		 * @endcode
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void zipChannels(const std::initializer_list<const TSource*>& sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const TSource sourceFactor, const TTarget targetFactor, const std::initializer_list<unsigned int>& sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

	protected:

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the source frame has, with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void separateTo1ChannelRuntime(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param channels The number of provided source frames (and the number of channels the target frame will have), with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void zipChannelsRuntime(const TSource* const* const sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param targetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity), nullptr if all are zero
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels The number of source frames (and target channels) if known at compile time; otherwise CHANNELS_NOT_KNOWN_AT_COMPILE_TIME == 0, if know at compile time must be identical with 'channels'
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void separateTo1ChannelOnlyTargetFactorNEON(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param sourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels The number of source frames (and target channels) if known at compile time; otherwise CHANNELS_NOT_KNOWN_AT_COMPILE_TIME == 0, if know at compile time must be identical with 'channels'
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void zipChannelsOnlySourceFactorNEON(const TSource* const* const sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const TSource sourceFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

#endif // OCEAN_HARDWARE_NEON_VERSION
};

template <typename TSource, typename TTarget, unsigned int tChannels>
void AdvancedFrameChannels::separateTo1Channel(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);

	assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));
	assert(sourceFactor != TSource(1) || targetFactor != TTarget(1)); // Identity factors, use FrameChannels::separateTo1Channel() instead!

	ocean_assert(tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME || tChannels == channels);

	if constexpr (tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME)
	{
		separateTo1ChannelRuntime<TSource, TTarget>(sourceFrame, targetFrames, width, height, channels, sourceFactor, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements);
		return;
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (std::is_same<TSource, uint8_t>::value && std::is_same<TTarget, float>::value && tChannels >= 2u && tChannels <= 4u)
	{
		if (sourceFactor == uint8_t(1))
		{
			separateTo1ChannelOnlyTargetFactorNEON<TSource, TTarget, tChannels>(sourceFrame, targetFrames, width, height, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements);

			return;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION

#ifdef OCEAN_DEBUG
	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		ocean_assert(targetFrames[c] != nullptr);
	}
#endif

	if (sourceFramePaddingElements == 0u && targetFramesPaddingElements == nullptr)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				targetFrames[c][n] = TTarget(sourceFrame[n * tChannels + c] * sourceFactor) * targetFactor;
			}
		}
	}
	else if (targetFramesPaddingElements == nullptr)
	{
		ocean_assert(sourceFramePaddingElements != 0u);

		const unsigned int sourceFrameStrideElements = width * tChannels + sourceFramePaddingElements;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			const unsigned int targetRowOffset = y * width;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					*(targetFrames[c] + targetRowOffset + x) = TTarget(*(sourceRow + x * tChannels + c) * sourceFactor) * targetFactor;
				}
			}
		}
	}
	else
	{
		const unsigned int sourceFrameStrideElements = width * tChannels + sourceFramePaddingElements;

		Indices32 targetFrameStrideElements(tChannels);

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			targetFrameStrideElements[c] = width + targetFramesPaddingElements[c];
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					*(targetFrames[c] + y * targetFrameStrideElements[c] + x) = TTarget(*(sourceRow + x * tChannels + c) * sourceFactor) * targetFactor;
				}
			}
		}
	}
}

template <typename TSource, typename TTarget>
void AdvancedFrameChannels::separateTo1Channel(const TSource* const sourceFrame, const std::initializer_list<TTarget*>& targetFrames, const unsigned int width, const unsigned int height, const TSource sourceFactor, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const std::initializer_list<const unsigned int>& targetFramesPaddingElements)
{
	ocean_assert(targetFrames.size() >= 1);
	ocean_assert(targetFramesPaddingElements.size() == 0 || targetFrames.size() == targetFramesPaddingElements.size());

	assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));
	assert(sourceFactor != TSource(1) || targetFactor != TTarget(1)); // Identity factors, use FrameChannels::separateTo1Channel() instead!

	if (targetFrames.size() == 2)
	{
		separateTo1Channel<TSource, TTarget, 2u>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
	else if (targetFrames.size() == 3)
	{
		separateTo1Channel<TSource, TTarget, 3u>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
	else if (targetFrames.size() == 4)
	{
		separateTo1Channel<TSource, TTarget, 4u>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
	else
	{
		separateTo1Channel<TSource, TTarget, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels>
void AdvancedFrameChannels::zipChannels(const TSource* const* sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);

	assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));
	assert(sourceFactor != TSource(1) || targetFactor != TTarget(1)); // Identity factors, use FrameChannels::separateTo1Channel() instead!

	ocean_assert(tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME || tChannels == channels);

	if constexpr (tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME)
	{
		zipChannelsRuntime<TSource, TTarget>(sourceFrames, targetFrame, width, height, channels, sourceFactor, targetFactor, sourceFramesPaddingElements, targetFramePaddingElements);
		return;
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (std::is_same<TSource, float>::value && std::is_same<TTarget, uint8_t>::value && tChannels >= 2u && tChannels <= 4u)
	{
		if (targetFactor == uint8_t(1))
		{
			zipChannelsOnlySourceFactorNEON<TSource, TTarget, tChannels>(sourceFrames, targetFrame, width, height, sourceFactor, sourceFramesPaddingElements, targetFramePaddingElements);

			return;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				targetFrame[n * tChannels + c] = TTarget(sourceFrames[c][n] * sourceFactor) * targetFactor;
			}
		}
	}
	else
	{
		const unsigned int targetFrameStrideElements = width * tChannels + targetFramePaddingElements;

		Indices32 sourceFrameStrideElements(tChannels);

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			if (sourceFramesPaddingElements == nullptr)
			{
				sourceFrameStrideElements[c] = width;
			}
			else
			{
				sourceFrameStrideElements[c] = width + sourceFramesPaddingElements[c];
			}
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			TTarget* const targetRow = targetFrame + y * targetFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					*(targetRow + x * tChannels + c) = TTarget(*(sourceFrames[c] + y * sourceFrameStrideElements[c] + x) * sourceFactor) * targetFactor;
				}
			}
		}
	}
}

template <typename TSource, typename TTarget>
void AdvancedFrameChannels::zipChannels(const std::initializer_list<const TSource*>& sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const TSource sourceFactor, const TTarget targetFactor, const std::initializer_list<unsigned int>& sourceFramePaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames.size() >= 1);
	ocean_assert(sourceFramePaddingElements.size() == 0 || sourceFrames.size() == sourceFramePaddingElements.size());

	assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));
	assert(sourceFactor != TSource(1) || targetFactor != TTarget(1)); // Identity factors, use FrameChannels::separateTo1Channel() instead!

	if (sourceFrames.size() == 2)
	{
		zipChannels<TSource, TTarget, 2u>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
	else if (sourceFrames.size() == 3)
	{
		zipChannels<TSource, TTarget, 3u>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
	else if (sourceFrames.size() == 4)
	{
		zipChannels<TSource, TTarget, 4u>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
	else
	{
		zipChannels<TSource, TTarget, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFactor, targetFactor, sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
}

template <typename TSource, typename TTarget>
void AdvancedFrameChannels::separateTo1ChannelRuntime(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));
	assert(sourceFactor != TSource(1) || targetFactor != TTarget(1)); // Identity factors, use FrameChannels::separateTo1Channel() instead!

#ifdef OCEAN_DEBUG
	for (unsigned int c = 0u; c < channels; ++c)
	{
		ocean_assert(targetFrames[c] != nullptr);
	}
#endif

	if (sourceFramePaddingElements == 0u && targetFramesPaddingElements == nullptr)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				targetFrames[c][n] = TTarget(sourceFrame[n * channels + c] * sourceFactor) * targetFactor;
			}
		}
	}
	else if (targetFramesPaddingElements == nullptr)
	{
		ocean_assert(sourceFramePaddingElements != 0u);

		const unsigned int sourceFrameStrideElements = width * channels + sourceFramePaddingElements;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			const unsigned int targetRowOffset = y * width;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					*(targetFrames[c] + targetRowOffset + x) = TTarget(*(sourceRow + x * channels + c) * sourceFactor) * targetFactor;
				}
			}
		}
	}
	else
	{
		const unsigned int sourceFrameStrideElements = width * channels + sourceFramePaddingElements;

		Indices32 targetFrameStrideElements(channels);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			targetFrameStrideElements[c] = width + targetFramesPaddingElements[c];
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					*(targetFrames[c] + y * targetFrameStrideElements[c] + x) = TTarget(*(sourceRow + x * channels + c) * sourceFactor) * targetFactor;
				}
			}
		}
	}
}

template <typename TSource, typename TTarget>
void AdvancedFrameChannels::zipChannelsRuntime(const TSource* const* sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const TSource sourceFactor, const TTarget targetFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));
	assert(sourceFactor != TSource(1) || targetFactor != TTarget(1)); // Identity factors, use FrameChannels::separateTo1Channel() instead!

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				targetFrame[n * channels + c] = TTarget(sourceFrames[c][n] * sourceFactor) * targetFactor;
			}
		}
	}
	else
	{
		const unsigned int targetFrameStrideElements = width * channels + targetFramePaddingElements;

		Indices32 sourceFrameStrideElements(channels);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			if (sourceFramesPaddingElements == nullptr)
			{
				sourceFrameStrideElements[c] = width;
			}
			else
			{
				sourceFrameStrideElements[c] = width + sourceFramesPaddingElements[c];
			}
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			TTarget* const targetRow = targetFrame + y * targetFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					*(targetRow + x * channels + c) = TTarget(*(sourceFrames[c] + y * sourceFrameStrideElements[c] + x) * sourceFactor) * targetFactor;
				}
			}
		}
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
inline void AdvancedFrameChannels::separateTo1ChannelOnlyTargetFactorNEON<uint8_t, float, 2u>(const uint8_t* const sourceFrame, float* const* const targetFrames, const unsigned int width, const unsigned int height, const float targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);

	ocean_assert(targetFactor != 0.0f);

	constexpr unsigned int tChannels = 2u;

	bool allTargetFramesContinuous = true;

	if (targetFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (targetFramesPaddingElements[n] != 0u)
			{
				allTargetFramesContinuous = false;
				break;
			}
		}
	}

	const float32x4_t targetFactor_32x4 = vdupq_n_f32(targetFactor);

	const uint8_t* source = sourceFrame;
	float* target0 = targetFrames[0];
	float* target1 = targetFrames[1];

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x2_t source_u_8x16x2;

	if (allTargetFramesContinuous && sourceFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_u_8x16x2 = vld2q_u8(source);

			const float32x4x4_t sourceA_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x2.val[0]);
			const float32x4x4_t sourceB_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x2.val[1]);

			vst1q_f32(target0 +  0, vmulq_f32(sourceA_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target0 +  4, vmulq_f32(sourceA_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target0 +  8, vmulq_f32(sourceA_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target0 + 12, vmulq_f32(sourceA_f_32x4x4.val[3], targetFactor_32x4));

			vst1q_f32(target1 +  0, vmulq_f32(sourceB_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target1 +  4, vmulq_f32(sourceB_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target1 +  8, vmulq_f32(sourceB_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target1 + 12, vmulq_f32(sourceB_f_32x4x4.val[3], targetFactor_32x4));

			source += tBlockSize * tChannels;

			target0 += tBlockSize;
			target1 += tBlockSize;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target0[n] = float(source[n * tChannels + 0u]) * targetFactor;
			target1[n] = float(source[n * tChannels + 1u]) * targetFactor;
		}
	}
	else
	{
		const unsigned int targetFrame0PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[0];
		const unsigned int targetFrame1PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[1];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_u_8x16x2 = vld2q_u8(source);

				const float32x4x4_t sourceA_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x2.val[0]);
				const float32x4x4_t sourceB_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x2.val[1]);

				vst1q_f32(target0 +  0, vmulq_f32(sourceA_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target0 +  4, vmulq_f32(sourceA_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target0 +  8, vmulq_f32(sourceA_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target0 + 12, vmulq_f32(sourceA_f_32x4x4.val[3], targetFactor_32x4));

				vst1q_f32(target1 +  0, vmulq_f32(sourceB_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target1 +  4, vmulq_f32(sourceB_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target1 +  8, vmulq_f32(sourceB_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target1 + 12, vmulq_f32(sourceB_f_32x4x4.val[3], targetFactor_32x4));

				source += tBlockSize * tChannels;

				target0 += tBlockSize;
				target1 += tBlockSize;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target0[n] = float(source[n * tChannels + 0u]) * targetFactor;
				target1[n] = float(source[n * tChannels + 1u]) * targetFactor;
			}

			source += remaining * tChannels + sourceFramePaddingElements;
			target0 += remaining + targetFrame0PaddingElements;
			target1 += remaining + targetFrame1PaddingElements;
		}
	}
}

template <>
inline void AdvancedFrameChannels::separateTo1ChannelOnlyTargetFactorNEON<uint8_t, float, 3u>(const uint8_t* const sourceFrame, float* const* const targetFrames, const unsigned int width, const unsigned int height, const float targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);

	constexpr unsigned int tChannels = 3u;

	bool allTargetFramesContinuous = true;

	if (targetFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (targetFramesPaddingElements[n] != 0u)
			{
				allTargetFramesContinuous = false;
				break;
			}
		}
	}

	const float32x4_t targetFactor_32x4 = vdupq_n_f32(targetFactor);

	const uint8_t* source = sourceFrame;
	float* target0 = targetFrames[0];
	float* target1 = targetFrames[1];
	float* target2 = targetFrames[2];

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x3_t source_u_8x16x3;

	if (allTargetFramesContinuous && sourceFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_u_8x16x3 = vld3q_u8(source);

			const float32x4x4_t sourceA_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x3.val[0]);
			const float32x4x4_t sourceB_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x3.val[1]);
			const float32x4x4_t sourceC_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x3.val[2]);

			vst1q_f32(target0 +  0, vmulq_f32(sourceA_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target0 +  4, vmulq_f32(sourceA_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target0 +  8, vmulq_f32(sourceA_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target0 + 12, vmulq_f32(sourceA_f_32x4x4.val[3], targetFactor_32x4));

			vst1q_f32(target1 +  0, vmulq_f32(sourceB_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target1 +  4, vmulq_f32(sourceB_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target1 +  8, vmulq_f32(sourceB_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target1 + 12, vmulq_f32(sourceB_f_32x4x4.val[3], targetFactor_32x4));

			vst1q_f32(target2 +  0, vmulq_f32(sourceC_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target2 +  4, vmulq_f32(sourceC_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target2 +  8, vmulq_f32(sourceC_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target2 + 12, vmulq_f32(sourceC_f_32x4x4.val[3], targetFactor_32x4));

			source += tBlockSize * tChannels;

			target0 += tBlockSize;
			target1 += tBlockSize;
			target2 += tBlockSize;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target0[n] = float(source[n * tChannels + 0u]) * targetFactor;
			target1[n] = float(source[n * tChannels + 1u]) * targetFactor;
			target2[n] = float(source[n * tChannels + 2u]) * targetFactor;
		}
	}
	else
	{
		const unsigned int targetFrame0PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[0];
		const unsigned int targetFrame1PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[1];
		const unsigned int targetFrame2PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[2];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_u_8x16x3 = vld3q_u8(source);

				const float32x4x4_t sourceA_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x3.val[0]);
				const float32x4x4_t sourceB_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x3.val[1]);
				const float32x4x4_t sourceC_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x3.val[2]);

				vst1q_f32(target0 +  0, vmulq_f32(sourceA_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target0 +  4, vmulq_f32(sourceA_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target0 +  8, vmulq_f32(sourceA_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target0 + 12, vmulq_f32(sourceA_f_32x4x4.val[3], targetFactor_32x4));

				vst1q_f32(target1 +  0, vmulq_f32(sourceB_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target1 +  4, vmulq_f32(sourceB_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target1 +  8, vmulq_f32(sourceB_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target1 + 12, vmulq_f32(sourceB_f_32x4x4.val[3], targetFactor_32x4));

				vst1q_f32(target2 +  0, vmulq_f32(sourceC_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target2 +  4, vmulq_f32(sourceC_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target2 +  8, vmulq_f32(sourceC_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target2 + 12, vmulq_f32(sourceC_f_32x4x4.val[3], targetFactor_32x4));

				source += tBlockSize * tChannels;

				target0 += tBlockSize;
				target1 += tBlockSize;
				target2 += tBlockSize;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target0[n] = float(source[n * tChannels + 0u]) * targetFactor;
				target1[n] = float(source[n * tChannels + 1u]) * targetFactor;
				target2[n] = float(source[n * tChannels + 2u]) * targetFactor;
			}

			source += remaining * tChannels + sourceFramePaddingElements;
			target0 += remaining + targetFrame0PaddingElements;
			target1 += remaining + targetFrame1PaddingElements;
			target2 += remaining + targetFrame2PaddingElements;
		}
	}
}

template <>
inline void AdvancedFrameChannels::separateTo1ChannelOnlyTargetFactorNEON<uint8_t, float, 4u>(const uint8_t* const sourceFrame, float* const* const targetFrames, const unsigned int width, const unsigned int height, const float targetFactor, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);

	constexpr unsigned int tChannels = 4u;

	bool allTargetFramesContinuous = true;

	if (targetFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (targetFramesPaddingElements[n] != 0u)
			{
				allTargetFramesContinuous = false;
				break;
			}
		}
	}

	const float32x4_t targetFactor_32x4 = vdupq_n_f32(targetFactor);

	const uint8_t* source = sourceFrame;
	float* target0 = targetFrames[0];
	float* target1 = targetFrames[1];
	float* target2 = targetFrames[2];
	float* target3 = targetFrames[3];

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x4_t source_u_8x16x4;

	if (allTargetFramesContinuous && sourceFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_u_8x16x4 = vld4q_u8(source);

			const float32x4x4_t sourceA_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[0]);
			const float32x4x4_t sourceB_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[1]);
			const float32x4x4_t sourceC_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[2]);
			const float32x4x4_t sourceD_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[3]);

			vst1q_f32(target0 +  0, vmulq_f32(sourceA_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target0 +  4, vmulq_f32(sourceA_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target0 +  8, vmulq_f32(sourceA_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target0 + 12, vmulq_f32(sourceA_f_32x4x4.val[3], targetFactor_32x4));

			vst1q_f32(target1 +  0, vmulq_f32(sourceB_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target1 +  4, vmulq_f32(sourceB_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target1 +  8, vmulq_f32(sourceB_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target1 + 12, vmulq_f32(sourceB_f_32x4x4.val[3], targetFactor_32x4));

			vst1q_f32(target2 +  0, vmulq_f32(sourceC_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target2 +  4, vmulq_f32(sourceC_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target2 +  8, vmulq_f32(sourceC_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target2 + 12, vmulq_f32(sourceC_f_32x4x4.val[3], targetFactor_32x4));

			vst1q_f32(target3 +  0, vmulq_f32(sourceD_f_32x4x4.val[0], targetFactor_32x4));
			vst1q_f32(target3 +  4, vmulq_f32(sourceD_f_32x4x4.val[1], targetFactor_32x4));
			vst1q_f32(target3 +  8, vmulq_f32(sourceD_f_32x4x4.val[2], targetFactor_32x4));
			vst1q_f32(target3 + 12, vmulq_f32(sourceD_f_32x4x4.val[3], targetFactor_32x4));

			source += tBlockSize * tChannels;

			target0 += tBlockSize;
			target1 += tBlockSize;
			target2 += tBlockSize;
			target3 += tBlockSize;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target0[n] = float(source[n * tChannels + 0u]) * targetFactor;
			target1[n] = float(source[n * tChannels + 1u]) * targetFactor;
			target2[n] = float(source[n * tChannels + 2u]) * targetFactor;
			target3[n] = float(source[n * tChannels + 3u]) * targetFactor;
		}
	}
	else
	{
		const unsigned int targetFrame0PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[0];
		const unsigned int targetFrame1PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[1];
		const unsigned int targetFrame2PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[2];
		const unsigned int targetFrame3PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[3];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_u_8x16x4 = vld4q_u8(source);

				const float32x4x4_t sourceA_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[0]);
				const float32x4x4_t sourceB_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[1]);
				const float32x4x4_t sourceC_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[2]);
				const float32x4x4_t sourceD_f_32x4x4 = NEON::cast16ElementsNEON(source_u_8x16x4.val[3]);

				vst1q_f32(target0 +  0, vmulq_f32(sourceA_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target0 +  4, vmulq_f32(sourceA_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target0 +  8, vmulq_f32(sourceA_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target0 + 12, vmulq_f32(sourceA_f_32x4x4.val[3], targetFactor_32x4));

				vst1q_f32(target1 +  0, vmulq_f32(sourceB_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target1 +  4, vmulq_f32(sourceB_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target1 +  8, vmulq_f32(sourceB_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target1 + 12, vmulq_f32(sourceB_f_32x4x4.val[3], targetFactor_32x4));

				vst1q_f32(target2 +  0, vmulq_f32(sourceC_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target2 +  4, vmulq_f32(sourceC_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target2 +  8, vmulq_f32(sourceC_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target2 + 12, vmulq_f32(sourceC_f_32x4x4.val[3], targetFactor_32x4));

				vst1q_f32(target3 +  0, vmulq_f32(sourceD_f_32x4x4.val[0], targetFactor_32x4));
				vst1q_f32(target3 +  4, vmulq_f32(sourceD_f_32x4x4.val[1], targetFactor_32x4));
				vst1q_f32(target3 +  8, vmulq_f32(sourceD_f_32x4x4.val[2], targetFactor_32x4));
				vst1q_f32(target3 + 12, vmulq_f32(sourceD_f_32x4x4.val[3], targetFactor_32x4));

				source += tBlockSize * tChannels;

				target0 += tBlockSize;
				target1 += tBlockSize;
				target2 += tBlockSize;
				target3 += tBlockSize;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target0[n] = float(source[n * tChannels + 0u]) * targetFactor;
				target1[n] = float(source[n * tChannels + 1u]) * targetFactor;
				target2[n] = float(source[n * tChannels + 2u]) * targetFactor;
				target3[n] = float(source[n * tChannels + 3u]) * targetFactor;
			}

			source += remaining * tChannels + sourceFramePaddingElements;
			target0 += remaining + targetFrame0PaddingElements;
			target1 += remaining + targetFrame1PaddingElements;
			target2 += remaining + targetFrame2PaddingElements;
			target3 += remaining + targetFrame3PaddingElements;
		}
	}
}

template <>
void AdvancedFrameChannels::zipChannelsOnlySourceFactorNEON<float, uint8_t, 2u>(const float* const* const sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const float sourceFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);

	constexpr unsigned int tChannels = 2u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const float32x4_t sourceFactor_32x4 = vdupq_n_f32(sourceFactor);

	const float* source0 = sourceFrames[0];
	const float* source1 = sourceFrames[1];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x2_t target_8x16x2;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const float32x4_t source0_A_f_32x4 = vmulq_f32(vld1q_f32(source0 +  0), sourceFactor_32x4);
			const float32x4_t source0_B_f_32x4 = vmulq_f32(vld1q_f32(source0 +  4), sourceFactor_32x4);
			const float32x4_t source0_C_f_32x4 = vmulq_f32(vld1q_f32(source0 +  8), sourceFactor_32x4);
			const float32x4_t source0_D_f_32x4 = vmulq_f32(vld1q_f32(source0 + 12), sourceFactor_32x4);

			const float32x4_t source1_A_f_32x4 = vmulq_f32(vld1q_f32(source1 +  0), sourceFactor_32x4);
			const float32x4_t source1_B_f_32x4 = vmulq_f32(vld1q_f32(source1 +  4), sourceFactor_32x4);
			const float32x4_t source1_C_f_32x4 = vmulq_f32(vld1q_f32(source1 +  8), sourceFactor_32x4);
			const float32x4_t source1_D_f_32x4 = vmulq_f32(vld1q_f32(source1 + 12), sourceFactor_32x4);

			target_8x16x2.val[0] = NEON::cast16ElementsNEON(source0_A_f_32x4, source0_B_f_32x4, source0_C_f_32x4, source0_D_f_32x4);
			target_8x16x2.val[1] = NEON::cast16ElementsNEON(source1_A_f_32x4, source1_B_f_32x4, source1_C_f_32x4, source1_D_f_32x4);

			vst2q_u8(target, target_8x16x2);

			source0 += tBlockSize;
			source1 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
			ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);

			target[n * tChannels + 0u] = uint8_t(source0[n] * sourceFactor);
			target[n * tChannels + 1u] = uint8_t(source1[n] * sourceFactor);
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				const float32x4_t source0_A_f_32x4 = vmulq_f32(vld1q_f32(source0 +  0), sourceFactor_32x4);
				const float32x4_t source0_B_f_32x4 = vmulq_f32(vld1q_f32(source0 +  4), sourceFactor_32x4);
				const float32x4_t source0_C_f_32x4 = vmulq_f32(vld1q_f32(source0 +  8), sourceFactor_32x4);
				const float32x4_t source0_D_f_32x4 = vmulq_f32(vld1q_f32(source0 + 12), sourceFactor_32x4);

				const float32x4_t source1_A_f_32x4 = vmulq_f32(vld1q_f32(source1 +  0), sourceFactor_32x4);
				const float32x4_t source1_B_f_32x4 = vmulq_f32(vld1q_f32(source1 +  4), sourceFactor_32x4);
				const float32x4_t source1_C_f_32x4 = vmulq_f32(vld1q_f32(source1 +  8), sourceFactor_32x4);
				const float32x4_t source1_D_f_32x4 = vmulq_f32(vld1q_f32(source1 + 12), sourceFactor_32x4);

				target_8x16x2.val[0] = NEON::cast16ElementsNEON(source0_A_f_32x4, source0_B_f_32x4, source0_C_f_32x4, source0_D_f_32x4);
				target_8x16x2.val[1] = NEON::cast16ElementsNEON(source1_A_f_32x4, source1_B_f_32x4, source1_C_f_32x4, source1_D_f_32x4);

				vst2q_u8(target, target_8x16x2);

				source0 += tBlockSize;
				source1 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
				ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);

				target[n * tChannels + 0u] = uint8_t(source0[n] * sourceFactor);
				target[n * tChannels + 1u] = uint8_t(source1[n] * sourceFactor);
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
void AdvancedFrameChannels::zipChannelsOnlySourceFactorNEON<float, uint8_t, 3u>(const float* const* const sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const float sourceFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);

	constexpr unsigned int tChannels = 3u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const float32x4_t sourceFactor_32x4 = vdupq_n_f32(sourceFactor);

	const float* source0 = sourceFrames[0];
	const float* source1 = sourceFrames[1];
	const float* source2 = sourceFrames[2];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x3_t target_8x16x3;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const float32x4_t source0_A_f_32x4 = vmulq_f32(vld1q_f32(source0 +  0), sourceFactor_32x4);
			const float32x4_t source0_B_f_32x4 = vmulq_f32(vld1q_f32(source0 +  4), sourceFactor_32x4);
			const float32x4_t source0_C_f_32x4 = vmulq_f32(vld1q_f32(source0 +  8), sourceFactor_32x4);
			const float32x4_t source0_D_f_32x4 = vmulq_f32(vld1q_f32(source0 + 12), sourceFactor_32x4);

			const float32x4_t source1_A_f_32x4 = vmulq_f32(vld1q_f32(source1 +  0), sourceFactor_32x4);
			const float32x4_t source1_B_f_32x4 = vmulq_f32(vld1q_f32(source1 +  4), sourceFactor_32x4);
			const float32x4_t source1_C_f_32x4 = vmulq_f32(vld1q_f32(source1 +  8), sourceFactor_32x4);
			const float32x4_t source1_D_f_32x4 = vmulq_f32(vld1q_f32(source1 + 12), sourceFactor_32x4);

			const float32x4_t source2_A_f_32x4 = vmulq_f32(vld1q_f32(source2 +  0), sourceFactor_32x4);
			const float32x4_t source2_B_f_32x4 = vmulq_f32(vld1q_f32(source2 +  4), sourceFactor_32x4);
			const float32x4_t source2_C_f_32x4 = vmulq_f32(vld1q_f32(source2 +  8), sourceFactor_32x4);
			const float32x4_t source2_D_f_32x4 = vmulq_f32(vld1q_f32(source2 + 12), sourceFactor_32x4);

			target_8x16x3.val[0] = NEON::cast16ElementsNEON(source0_A_f_32x4, source0_B_f_32x4, source0_C_f_32x4, source0_D_f_32x4);
			target_8x16x3.val[1] = NEON::cast16ElementsNEON(source1_A_f_32x4, source1_B_f_32x4, source1_C_f_32x4, source1_D_f_32x4);
			target_8x16x3.val[2] = NEON::cast16ElementsNEON(source2_A_f_32x4, source2_B_f_32x4, source2_C_f_32x4, source2_D_f_32x4);

			vst3q_u8(target, target_8x16x3);

			source0 += tBlockSize;
			source1 += tBlockSize;
			source2 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
			ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
			ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);

			target[n * tChannels + 0u] = uint8_t(source0[n] * sourceFactor);
			target[n * tChannels + 1u] = uint8_t(source1[n] * sourceFactor);
			target[n * tChannels + 2u] = uint8_t(source2[n] * sourceFactor);
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];
		const unsigned int sourceFrame2PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[2];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				const float32x4_t source0_A_f_32x4 = vmulq_f32(vld1q_f32(source0 +  0), sourceFactor_32x4);
				const float32x4_t source0_B_f_32x4 = vmulq_f32(vld1q_f32(source0 +  4), sourceFactor_32x4);
				const float32x4_t source0_C_f_32x4 = vmulq_f32(vld1q_f32(source0 +  8), sourceFactor_32x4);
				const float32x4_t source0_D_f_32x4 = vmulq_f32(vld1q_f32(source0 + 12), sourceFactor_32x4);

				const float32x4_t source1_A_f_32x4 = vmulq_f32(vld1q_f32(source1 +  0), sourceFactor_32x4);
				const float32x4_t source1_B_f_32x4 = vmulq_f32(vld1q_f32(source1 +  4), sourceFactor_32x4);
				const float32x4_t source1_C_f_32x4 = vmulq_f32(vld1q_f32(source1 +  8), sourceFactor_32x4);
				const float32x4_t source1_D_f_32x4 = vmulq_f32(vld1q_f32(source1 + 12), sourceFactor_32x4);

				const float32x4_t source2_A_f_32x4 = vmulq_f32(vld1q_f32(source2 +  0), sourceFactor_32x4);
				const float32x4_t source2_B_f_32x4 = vmulq_f32(vld1q_f32(source2 +  4), sourceFactor_32x4);
				const float32x4_t source2_C_f_32x4 = vmulq_f32(vld1q_f32(source2 +  8), sourceFactor_32x4);
				const float32x4_t source2_D_f_32x4 = vmulq_f32(vld1q_f32(source2 + 12), sourceFactor_32x4);

				target_8x16x3.val[0] = NEON::cast16ElementsNEON(source0_A_f_32x4, source0_B_f_32x4, source0_C_f_32x4, source0_D_f_32x4);
				target_8x16x3.val[1] = NEON::cast16ElementsNEON(source1_A_f_32x4, source1_B_f_32x4, source1_C_f_32x4, source1_D_f_32x4);
				target_8x16x3.val[2] = NEON::cast16ElementsNEON(source2_A_f_32x4, source2_B_f_32x4, source2_C_f_32x4, source2_D_f_32x4);

				vst3q_u8(target, target_8x16x3);

				source0 += tBlockSize;
				source1 += tBlockSize;
				source2 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
				ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
				ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);

				target[n * tChannels + 0u] = uint8_t(source0[n] * sourceFactor);
				target[n * tChannels + 1u] = uint8_t(source1[n] * sourceFactor);
				target[n * tChannels + 2u] = uint8_t(source2[n] * sourceFactor);
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			source2 += remaining + sourceFrame2PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
void AdvancedFrameChannels::zipChannelsOnlySourceFactorNEON<float, uint8_t, 4u>(const float* const* const sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const float sourceFactor, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);

	constexpr unsigned int tChannels = 4u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const float32x4_t sourceFactor_32x4 = vdupq_n_f32(sourceFactor);

	const float* source0 = sourceFrames[0];
	const float* source1 = sourceFrames[1];
	const float* source2 = sourceFrames[2];
	const float* source3 = sourceFrames[3];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x4_t target_8x16x4;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const float32x4_t source0_A_f_32x4 = vmulq_f32(vld1q_f32(source0 +  0), sourceFactor_32x4);
			const float32x4_t source0_B_f_32x4 = vmulq_f32(vld1q_f32(source0 +  4), sourceFactor_32x4);
			const float32x4_t source0_C_f_32x4 = vmulq_f32(vld1q_f32(source0 +  8), sourceFactor_32x4);
			const float32x4_t source0_D_f_32x4 = vmulq_f32(vld1q_f32(source0 + 12), sourceFactor_32x4);

			const float32x4_t source1_A_f_32x4 = vmulq_f32(vld1q_f32(source1 +  0), sourceFactor_32x4);
			const float32x4_t source1_B_f_32x4 = vmulq_f32(vld1q_f32(source1 +  4), sourceFactor_32x4);
			const float32x4_t source1_C_f_32x4 = vmulq_f32(vld1q_f32(source1 +  8), sourceFactor_32x4);
			const float32x4_t source1_D_f_32x4 = vmulq_f32(vld1q_f32(source1 + 12), sourceFactor_32x4);

			const float32x4_t source2_A_f_32x4 = vmulq_f32(vld1q_f32(source2 +  0), sourceFactor_32x4);
			const float32x4_t source2_B_f_32x4 = vmulq_f32(vld1q_f32(source2 +  4), sourceFactor_32x4);
			const float32x4_t source2_C_f_32x4 = vmulq_f32(vld1q_f32(source2 +  8), sourceFactor_32x4);
			const float32x4_t source2_D_f_32x4 = vmulq_f32(vld1q_f32(source2 + 12), sourceFactor_32x4);

			const float32x4_t source3_A_f_32x4 = vmulq_f32(vld1q_f32(source3 +  0), sourceFactor_32x4);
			const float32x4_t source3_B_f_32x4 = vmulq_f32(vld1q_f32(source3 +  4), sourceFactor_32x4);
			const float32x4_t source3_C_f_32x4 = vmulq_f32(vld1q_f32(source3 +  8), sourceFactor_32x4);
			const float32x4_t source3_D_f_32x4 = vmulq_f32(vld1q_f32(source3 + 12), sourceFactor_32x4);

			target_8x16x4.val[0] = NEON::cast16ElementsNEON(source0_A_f_32x4, source0_B_f_32x4, source0_C_f_32x4, source0_D_f_32x4);
			target_8x16x4.val[1] = NEON::cast16ElementsNEON(source1_A_f_32x4, source1_B_f_32x4, source1_C_f_32x4, source1_D_f_32x4);
			target_8x16x4.val[2] = NEON::cast16ElementsNEON(source2_A_f_32x4, source2_B_f_32x4, source2_C_f_32x4, source2_D_f_32x4);
			target_8x16x4.val[3] = NEON::cast16ElementsNEON(source3_A_f_32x4, source3_B_f_32x4, source3_C_f_32x4, source3_D_f_32x4);

			vst4q_u8(target, target_8x16x4);

			source0 += tBlockSize;
			source1 += tBlockSize;
			source2 += tBlockSize;
			source3 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
			ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
			ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);
			ocean_assert(source3[n] >= 0.0f && source3[n] < 256.0f);

			target[n * tChannels + 0u] = uint8_t(source0[n] * sourceFactor);
			target[n * tChannels + 1u] = uint8_t(source1[n] * sourceFactor);
			target[n * tChannels + 2u] = uint8_t(source2[n] * sourceFactor);
			target[n * tChannels + 3u] = uint8_t(source3[n] * sourceFactor);
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];
		const unsigned int sourceFrame2PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[2];
		const unsigned int sourceFrame3PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[3];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				const float32x4_t source0_A_f_32x4 = vmulq_f32(vld1q_f32(source0 +  0), sourceFactor_32x4);
				const float32x4_t source0_B_f_32x4 = vmulq_f32(vld1q_f32(source0 +  4), sourceFactor_32x4);
				const float32x4_t source0_C_f_32x4 = vmulq_f32(vld1q_f32(source0 +  8), sourceFactor_32x4);
				const float32x4_t source0_D_f_32x4 = vmulq_f32(vld1q_f32(source0 + 12), sourceFactor_32x4);

				const float32x4_t source1_A_f_32x4 = vmulq_f32(vld1q_f32(source1 +  0), sourceFactor_32x4);
				const float32x4_t source1_B_f_32x4 = vmulq_f32(vld1q_f32(source1 +  4), sourceFactor_32x4);
				const float32x4_t source1_C_f_32x4 = vmulq_f32(vld1q_f32(source1 +  8), sourceFactor_32x4);
				const float32x4_t source1_D_f_32x4 = vmulq_f32(vld1q_f32(source1 + 12), sourceFactor_32x4);

				const float32x4_t source2_A_f_32x4 = vmulq_f32(vld1q_f32(source2 +  0), sourceFactor_32x4);
				const float32x4_t source2_B_f_32x4 = vmulq_f32(vld1q_f32(source2 +  4), sourceFactor_32x4);
				const float32x4_t source2_C_f_32x4 = vmulq_f32(vld1q_f32(source2 +  8), sourceFactor_32x4);
				const float32x4_t source2_D_f_32x4 = vmulq_f32(vld1q_f32(source2 + 12), sourceFactor_32x4);

				const float32x4_t source3_A_f_32x4 = vmulq_f32(vld1q_f32(source3 +  0), sourceFactor_32x4);
				const float32x4_t source3_B_f_32x4 = vmulq_f32(vld1q_f32(source3 +  4), sourceFactor_32x4);
				const float32x4_t source3_C_f_32x4 = vmulq_f32(vld1q_f32(source3 +  8), sourceFactor_32x4);
				const float32x4_t source3_D_f_32x4 = vmulq_f32(vld1q_f32(source3 + 12), sourceFactor_32x4);

				target_8x16x4.val[0] = NEON::cast16ElementsNEON(source0_A_f_32x4, source0_B_f_32x4, source0_C_f_32x4, source0_D_f_32x4);
				target_8x16x4.val[1] = NEON::cast16ElementsNEON(source1_A_f_32x4, source1_B_f_32x4, source1_C_f_32x4, source1_D_f_32x4);
				target_8x16x4.val[2] = NEON::cast16ElementsNEON(source2_A_f_32x4, source2_B_f_32x4, source2_C_f_32x4, source2_D_f_32x4);
				target_8x16x4.val[3] = NEON::cast16ElementsNEON(source3_A_f_32x4, source3_B_f_32x4, source3_C_f_32x4, source3_D_f_32x4);

				vst4q_u8(target, target_8x16x4);

				source0 += tBlockSize;
				source1 += tBlockSize;
				source2 += tBlockSize;
				source3 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
				ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
				ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);
				ocean_assert(source3[n] >= 0.0f && source3[n] < 256.0f);

				target[n * tChannels + 0u] = uint8_t(source0[n] * sourceFactor);
				target[n * tChannels + 1u] = uint8_t(source1[n] * sourceFactor);
				target[n * tChannels + 2u] = uint8_t(source2[n] * sourceFactor);
				target[n * tChannels + 3u] = uint8_t(source3[n] * sourceFactor);
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			source2 += remaining + sourceFrame2PaddingElements;
			source3 += remaining + sourceFrame3PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_CHANNLES_H
