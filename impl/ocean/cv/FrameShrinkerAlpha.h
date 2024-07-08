/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_SHRINKER_ALPHA_H
#define META_OCEAN_CV_FRAME_SHRINKER_ALPHA_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameBlender.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions downsizing frames that hold alpha channels.
 * @ingroup cv
 */
class FrameShrinkerAlpha
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Divides a given frame by two, taking four pixel values into account:<br>
				 * | 1 1 |<br>
				 * | 1 1 |<br>
				 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
				 * If the type of the target frame does not match to the input frame the target frame (and image buffer) will be replaced by a correct one.
				 * @param source The source frame to resize
				 * @param target The target frame receiving the smaller frame data
				 * @param worker Optional worker object to distribute the computational load to several CPU cores
				 * @return True, if succeeded
				 */
				template <bool tTransparentIs0xFF>
				static bool divideByTwo(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Divides a given frame by two, taking four pixel values into account:<br>
				 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
				 * @param frame The frame to resize
				 * @param worker Optional worker object to distribute the computational load to several CPU cores
				 * @return True, if succeeded
				 */
				template <bool tTransparentIs0xFF>
				static bool divideByTwo(Frame& frame, Worker* worker = nullptr);
		};

	public:

		/**
		 * Divides a given 8 bit per channel frame by two, taking four pixel values into account:<br>
		 * | 1 1 |<br>
		 * | 1 1 |<br>
		 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
		 * @param source The source frame buffer to resize
		 * @param target The target frame buffer
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @tparam tChannels Number of channels of the frame (including the alpha channel), with range [1, infinity)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
		static void divideByTwo8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Divides a subset of a given 8 bit per channel frame by two, taking the average of four pixel values.
		 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
		 * @param source The source frame buffer to resize
		 * @param target The target frame buffer
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow First (including) row to halve
		 * @param numberTargetRows Number of rows to halve
		 * @tparam tChannels Number of channels of the frame (including the alpha channel), with range [1, infinity)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
		static void divideByTwo8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);
};

template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
void FrameShrinkerAlpha::divideByTwo8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);

	const unsigned int targetHeight = sourceHeight / 2u;
	ocean_assert(targetHeight > 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&divideByTwo8BitPerChannelSubset<tChannels, tAlphaAtFront, tTransparentIs0xFF>, source, target, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight, 6u, 7u, 20u);
	}
	else
	{
		divideByTwo8BitPerChannelSubset<tChannels, tAlphaAtFront, tTransparentIs0xFF>(source, target, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
	}
}

template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
void FrameShrinkerAlpha::divideByTwo8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);
	ocean_assert(firstTargetRow + numberTargetRows <= sourceHeight / 2u);

	const unsigned int targetWidth = sourceWidth / 2u;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const bool xEven = sourceWidth % 2u == 0u;
	const bool yEven = sourceHeight % 2u == 0u;

	const uint8_t* sourceFirst = source + firstTargetRow * 2u * sourceStrideElements;
	const uint8_t* sourceSecond = sourceFirst + sourceStrideElements;

	target += firstTargetRow * targetStrideElements;

	const bool threeBottomRows = !yEven && firstTargetRow + numberTargetRows == sourceHeight / 2u;

	const unsigned int numberTwoTargetRows = threeBottomRows ? (unsigned int)(max(int(numberTargetRows) - 1, 0)) : numberTargetRows;
	const unsigned int numberTwoTargetColumns = xEven ? targetWidth : (unsigned int)(max(int(targetWidth) - 1, 0));

	for (unsigned int y = 0u; y < numberTwoTargetRows; ++y)
	{
		for (unsigned int x = 0u; x < numberTwoTargetColumns; ++x)
		{
			// source0: | 1 1 |
			// source1: | 1 1 | / 4

			const unsigned int denominator = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]);

			if (denominator != 0u)
			{
				const unsigned int denominator_2 = denominator / 2u;

				// data channel
				for (unsigned int n = FrameBlender::SourceOffset<tAlphaAtFront>::data(); n < tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::data() - 1u; ++n)
					target[n] = uint8_t((sourceFirst[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceFirst[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels])
									+ sourceSecond[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceSecond[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) + denominator_2) / denominator);

				// alpha channel
				target[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = uint8_t((sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
								+ sourceFirst[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
								+ sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
								+ sourceSecond[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] + 2u) / 4u);
			}
			else
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					target[n] = uint8_t((sourceFirst[n] + sourceFirst[tChannels + n] + sourceSecond[n] + sourceSecond[tChannels + n] + 2u) / 4u);
				}
			}

			target += tChannels;
			sourceFirst += tChannels * 2u;
			sourceSecond += tChannels * 2u;
		}

		if (!xEven)
		{
			// three pixels left

			// source0: | 1 2 1 |
			// source1: | 1 2 1 | / 8

			const unsigned int denominator = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u]);

			if (denominator != 0u)
			{
				const unsigned int denominator_2 = denominator / 2u;

				// data channel
				for (unsigned int n = FrameBlender::SourceOffset<tAlphaAtFront>::data(); n < tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::data() - 1u; ++n)
					target[n] = uint8_t((sourceFirst[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceFirst[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
									+ sourceFirst[tChannels * 2u + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u])
									+ sourceSecond[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceSecond[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
									+ sourceSecond[tChannels * 2u + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u]) + denominator_2) / denominator);

				// alpha channel
				target[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] =
								uint8_t((sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
									+ sourceFirst[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
									+ sourceFirst[tChannels * 2u + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
									+ sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
									+ sourceSecond[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
									+ sourceSecond[tChannels * 2u + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] + 4u) / 8u);
			}
			else
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					target[n] = uint8_t((sourceFirst[n] + sourceFirst[tChannels + n] * 2u + sourceFirst[tChannels * 2u + n]
									+ sourceSecond[n] + sourceSecond[tChannels + n] * 2u + sourceSecond[tChannels * 2u + n] + 4u) / 8u);
				}
			}

			target += tChannels;
			sourceFirst += tChannels * 3u;
			sourceSecond += tChannels * 3u;
		}

		target += targetPaddingElements;
		sourceFirst += sourcePaddingElements + sourceStrideElements;
		sourceSecond += sourcePaddingElements + sourceStrideElements;
	}

	if (threeBottomRows)
	{
		const uint8_t* sourceThird = sourceSecond + sourceStrideElements;

		for (unsigned int x = 0u; x < numberTwoTargetColumns; ++x)
		{
			// source0: | 1 1 |
			// source1: | 2 2 | / 8
			// source2: | 1 1 |

			const unsigned int denominator = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]);

			if (denominator != 0u)
			{
				const unsigned int denominator_2 = denominator / 2u;

				// data channel
				for (unsigned int n = FrameBlender::SourceOffset<tAlphaAtFront>::data(); n < tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::data() - 1u; ++n)
					target[n] = uint8_t((sourceFirst[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceFirst[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels])
									+ sourceSecond[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]) * 2u
									+ sourceSecond[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
									+ sourceThird[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceThird[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) + denominator_2) / denominator);

				// alpha channel
				target[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] =
								uint8_t((sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
									+ sourceFirst[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
									+ sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
									+ sourceSecond[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
									+ sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
									+ sourceThird[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] + 4u) / 8u);
			}
			else
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					target[n] = uint8_t((sourceFirst[n] + sourceFirst[tChannels + n]
									+ sourceSecond[n] * 2u + sourceSecond[tChannels + n] * 2u
									+ sourceThird[n] + sourceThird[tChannels + n] + 4u) / 8u);
				}
			}

			target += tChannels;
			sourceFirst += tChannels * 2u;
			sourceSecond += tChannels * 2u;
			sourceThird += tChannels * 2u;
		}

		if (!xEven)
		{
			// three pixels left

			// source0: | 1 2 1 |
			// source1: | 2 4 2 | / 16
			// source2: | 1 2 1 |


			const unsigned int denominator = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 4u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
												+ FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u]);

			if (denominator != 0u)
			{
				const unsigned int denominator_2 = denominator / 2u;

				// data channel
				for (unsigned int n = FrameBlender::SourceOffset<tAlphaAtFront>::data(); n < tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::data() - 1u; ++n)
					target[n] = uint8_t((sourceFirst[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceFirst[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
									+ sourceFirst[tChannels * 2u + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u])
									+ sourceSecond[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]) * 2u
									+ sourceSecond[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 4u
									+ sourceSecond[tChannels * 2u + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u]) * 2u
									+ sourceThird[n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
									+ sourceThird[tChannels + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels]) * 2u
									+ sourceThird[tChannels * 2u + n] * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>() + tChannels * 2u]) + denominator_2) / denominator);

				// alpha channel
				target[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] =
								uint8_t((sourceFirst[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
								+ sourceFirst[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
								+ sourceFirst[tChannels * 2u + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
								+ sourceSecond[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
								+ sourceSecond[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 4u
								+ sourceSecond[tChannels * 2u + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
								+ sourceThird[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()]
								+ sourceThird[tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] * 2u
								+ sourceThird[tChannels * 2u + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] + 8u) / 16u);
			}
			else
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
					target[n] = uint8_t((sourceFirst[n] + sourceFirst[tChannels + n] * 2u + sourceFirst[tChannels * 2u + n]
									+ sourceSecond[n] * 2u + sourceSecond[tChannels + n] * 4u + sourceSecond[tChannels * 2u + n] * 2u
									+ sourceThird[n] + sourceThird[tChannels + n] * 2u + sourceThird[tChannels * 2u + n] + 4u) / 8u);
			}
		}
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_SHRINKER_ALPHA_H
