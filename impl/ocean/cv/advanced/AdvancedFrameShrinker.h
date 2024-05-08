/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_SHRINKER_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_SHRINKER_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements an advanced frame shrinker e.g., not simply combining visual information from the finer layer but also taking a corresponding mask into account.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameShrinker
{
	public:

		/**
		 * Bisects a given frame while taking a corresponding mask into account.
		 * This function combines color information of corresponding non-mask pixels only and normalizes the resulting color due to the number of valid pixels.<br>
		 * In the following, the downsampling scheme is depicted for a 1 channel 8 bit frame:
		 * <pre>
		 * 2x2 source pixels:    with 2x2 source mask pixels:       resulting 1x1 target source pixel:      resulting 1x1 target mask pixel:
		 * | 0x80 0x50 |         | 0xFF 0x00 |                      | (0x80 + 0x40) / 2 |                   | 0xFF |
		 * | 0x70 0x40 |         | 0x00 0xFF |
		 * </pre>
		 * @param source The source frame that will be bisected, with a frame dimension with at least 2x2, must be valid
		 * @param target The target frame receiving the bisected information, must be valid
		 * @param sourceMask Binary source mask to be bisected with 0x00 as mask pixels and 0xFF as non-mask pixels, other values are not allowed
		 * @param targetMask The target binary mask receiving the bisected binary mask, a mask pixel will be set to 0xFF if at least one non-mask pixels exists at the corresponding location in the source mask, 0x00 otherwise
		 * @param handleFullMaskPixel True, to handle also frame pixels corresponding to four mask pixels
		 * @param targetMaskHasPixel Optional state to determine whether the target mask has at least one mask pixel
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool divideByTwo(const Frame& source, Frame& target, const Frame& sourceMask, Frame& targetMask, const bool handleFullMaskPixel, bool* targetMaskHasPixel = nullptr, Worker* worker = nullptr);

		/**
		 * Bisects a given frame with 8 bit per frame channel while taking a corresponding mask into account.
		 * This function combines color information of corresponding non-mask pixels only and normalizes the resulting color due to the number of valid pixels.
		 * In the following, the downsampling scheme is depicted for a 1 channel 8 bit frame:
		 * <pre>
		 * 2x2 source pixels:    with 2x2 source mask pixels:       resulting 1x1 target source pixel:      resulting 1x1 target mask pixel:
		 * | 0x80 0x50 |         | 0xFF 0x00 |                      | (0x80 + 0x40) / 2 |                   | 0xFF |
		 * | 0x70 0x40 |         | 0x00 0xFF |
		 * </pre>
		 * @param source The source frame that will be bisected, must be valid
		 * @param target The target frame receiving the bisected information, must be valid
		 * @param sourceMask Binary source mask to be bisected with 0x00 as mask pixels and 0xFF as non-mask pixels, other values are not allowed
		 * @param targetMask The target binary mask receiving the bisected binary mask, a mask pixel will be set to 0xFF if at least one non-mask pixels exists at the corresponding location in the source mask, 0x00 otherwise
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param sourceMaskPaddingElements The number of padding elements at the end of each source mask row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param handleFullMaskPixel True, to handle also frame pixels corresponding to four mask pixels, otherwise this pixel will be untouched
		 * @param targetMaskHasPixel Optional resulting state to determine whether the target mask has at least one mask pixel
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void divideByTwo8BitPerChannel(const uint8_t* source, uint8_t* target, const uint8_t* sourceMask, uint8_t* targetMask, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetMaskPaddingElements, const bool handleFullMaskPixel, bool* targetMaskHasPixel = nullptr, Worker* worker = nullptr);

	private:

		/**
		 * Bisects a subset of a given frame with 8 bit per frame channel.
		 * @param source The source frame that will be bisected, must be valid
		 * @param target The target frame receiving the bisected information, must be valid
		 * @param sourceMask Binary source mask to be bisected with 0x00 as mask pixels and 0xFF as non-mask pixels, other values are not allowed
		 * @param targetMask The target binary mask receiving the bisected binary mask, a mask pixel will be set to 0xFF if at least one non-mask pixels exists at the corresponding location in the source mask, 0x00 otherwise
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param sourceMaskPaddingElements The number of padding elements at the end of each source mask row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param handleFullMaskPixel True, to handle also frame pixels corresponding to four mask pixels, otherwise this pixel will be untouched
		 * @param targetMaskHasPixel Optional resulting state to determine whether the target mask has at least one mask pixel
		 * @param firstTargetRow First target row to be handled, with range [0, sourceHeight / 2)
		 * @param numberTargetRows Number of target rows to be handled, with range [1, sourceHeight / 2]
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void divideByTwo8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const uint8_t* sourceMask, uint8_t* targetMask, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetMaskPaddingElements, const bool handleFullMaskPixel, bool* targetMaskHasPixel, const unsigned int firstTargetRow, const unsigned int numberTargetRows);
};

template <unsigned int tChannels>
inline void AdvancedFrameShrinker::divideByTwo8BitPerChannel(const uint8_t* source, uint8_t* target, const uint8_t* sourceMask, uint8_t* targetMask, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetMaskPaddingElements, const bool handleFullMaskPixel, bool* targetMaskHasPixel, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(sourceMask && targetMask);

	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);

	const unsigned int targetHeight = sourceHeight / 2u;

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&AdvancedFrameShrinker::divideByTwo8BitPerChannelSubset<tChannels>, source, target, sourceMask, targetMask, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, sourceMaskPaddingElements, targetMaskPaddingElements, handleFullMaskPixel, targetMaskHasPixel, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		divideByTwo8BitPerChannelSubset<tChannels>(source, target, sourceMask, targetMask, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, sourceMaskPaddingElements, targetMaskPaddingElements, handleFullMaskPixel, targetMaskHasPixel, 0u, targetHeight);
	}
}

template <unsigned int tChannels>
void AdvancedFrameShrinker::divideByTwo8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const uint8_t* sourceMask, uint8_t* targetMask, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetMaskPaddingElements, const bool handleFullMaskPixel, bool* targetMaskHasPixel, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceMask != nullptr && targetMask != nullptr);

	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);

	const unsigned int targetWidth = sourceWidth / 2u;
	const unsigned int targetHeight = sourceHeight / 2u;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const unsigned int sourceMaskStrideElements = sourceWidth + sourceMaskPaddingElements;
	const unsigned int targetMaskStrideElements = targetWidth + targetMaskPaddingElements;

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const bool threeRightColumns = sourceWidth % 2u == 1u;
	const bool threeBottomRows = sourceHeight % 2u == 1u && firstTargetRow + numberTargetRows == targetHeight;

	ocean_assert(numberTargetRows >= 1u);
	const unsigned int twoTargetRows = threeBottomRows ? numberTargetRows - 1u : numberTargetRows;

	ocean_assert(targetWidth >= 1u);
	const unsigned int twoTargetColumns = threeRightColumns ? targetWidth - 1u : targetWidth;

	const uint8_t* sourceTop = source + firstTargetRow * 2u * sourceStrideElements;
	const uint8_t* sourceBottom = sourceTop + sourceStrideElements;

	const uint8_t* sourceMaskTop = sourceMask + firstTargetRow * 2u * sourceMaskStrideElements;
	const uint8_t* sourceMaskBottom = sourceMaskTop + sourceMaskStrideElements;

	target += firstTargetRow * targetStrideElements;
	targetMask += firstTargetRow * targetMaskStrideElements;

	for (unsigned int ty = firstTargetRow; ty < firstTargetRow + twoTargetRows; ++ty)
	{
		for (unsigned int tx = 0u; tx < twoTargetColumns; ++tx)
		{
			const uint32_t state = uint32_t(sourceMaskTop[0]) | uint32_t(sourceMaskTop[1]) << 8u
									| uint32_t(sourceMaskBottom[0]) << 16u | uint32_t(sourceMaskBottom[1]) << 24u;

			switch (state)
			{
				// FF FF
				// FF FF
				case 0xFFFFFFFFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceTop[tChannels + n] + sourceBottom[n] + sourceBottom[tChannels + n] + 2u) / 4u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 FF
				// FF FF
				case 0xFFFFFF00u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[tChannels + n] + sourceBottom[n] + sourceBottom[tChannels + n] + 1u) / 3u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF 00
				// FF FF
				case 0xFFFF00FFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceBottom[n] + sourceBottom[tChannels + n] + 1u) / 3u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF FF
				// 00 FF
				case 0xFF00FFFFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceTop[tChannels + n] + sourceBottom[tChannels + n] + 1u) / 3u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF FF
				// FF 00
				case 0x00FFFFFFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceTop[tChannels + n] + sourceBottom[n] + 1u) / 3u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 00
				// FF FF
				case 0xFFFF0000u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceBottom[n] + sourceBottom[tChannels + n] + 1u) / 2u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF 00
				// FF 00
				case 0x00FF00FFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceBottom[n] + 1u) / 2u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF FF
				// 00 00
				case 0x0000FFFFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceTop[tChannels + n] + 1u) / 2u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 FF
				// 00 FF
				case 0xFF00FF00u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[tChannels + n] + sourceBottom[tChannels + n] + 1u) / 2u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 FF
				// FF 00
				case 0x00FFFF00u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[tChannels + n] + sourceBottom[n] + 1u) / 2u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF 00
				// 00 FF
				case 0xFF0000FFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = uint8_t((sourceTop[n] + sourceBottom[tChannels + n] + 1u) / 2u);
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 00
				// 00 FF
				case 0xFF000000u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = sourceBottom[tChannels + n];
					}
					*targetMask++ = 0xFF;
					break;
				}

				// FF 00
				// 00 00
				case 0x000000FFu:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = sourceTop[n];
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 FF
				// 00 00
				case 0x0000FF00u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = sourceTop[tChannels + n];
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 00
				// FF 00
				case 0x00FF0000u:
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						*target++ = sourceBottom[n];
					}
					*targetMask++ = 0xFF;
					break;
				}

				// 00 00
				// 00 00
				case 0x00000000u:
				{
					*targetMask++ = 0x00;

					if (targetMaskHasPixel)
					{
						*targetMaskHasPixel = true;
					}

					if (handleFullMaskPixel)
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							target[n] = uint8_t((sourceTop[n] + sourceTop[tChannels + n] + sourceBottom[n] + sourceBottom[tChannels + n] + 2u) / 4u);
						}
					}

					target += tChannels;
					break;
				}

				default:
					ocean_assert(false && "Invalid mask!");
					target += tChannels;
					++targetMask;
			}

			sourceTop += tChannels * 2u;
			sourceBottom += tChannels * 2u;

			sourceMaskTop += 2;
			sourceMaskBottom += 2;
		}

		if (threeRightColumns)
		{
			unsigned int values[tChannels] = {0u};
			unsigned int validPixels = 0u;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				if (sourceMaskTop[i] == 0xFF)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += sourceTop[i * tChannels + n];
					}

					++validPixels;
				}
			}

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				if (sourceMaskBottom[i] == 0xFF)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += sourceBottom[i * tChannels + n];
					}

					++validPixels;
				}
			}

			if (validPixels != 0u)
			{
				*targetMask = 0xFF;

				const unsigned int validPixels_2 = validPixels / 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					target[n] = (uint8_t)((values[n] + validPixels_2) / validPixels);
				}
			}
			else
			{
				*targetMask = 0x00;

				if (targetMaskHasPixel)
				{
					*targetMaskHasPixel = true;
				}

				if (handleFullMaskPixel)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						target[n] = (sourceTop[n] + sourceTop[tChannels + n] + sourceTop[tChannels * 2u + n] + sourceBottom[n] + sourceBottom[tChannels + n] + sourceBottom[tChannels * 2u + n] + 3u) / 6u;
					}
				}
			}

			sourceTop += tChannels * 3u;
			sourceBottom += tChannels * 3u;
			target += tChannels;

			sourceMaskTop += 3;
			sourceMaskBottom += 3;

			++targetMask;
		}

		sourceTop = sourceBottom + sourcePaddingElements;
		sourceBottom = sourceTop + sourceStrideElements;

		sourceMaskTop = sourceMaskBottom + sourceMaskPaddingElements;
		sourceMaskBottom = sourceMaskTop + sourceMaskStrideElements;

		target += targetPaddingElements;
		targetMask += targetMaskPaddingElements;
	}

	if (threeBottomRows)
	{
		for (unsigned int tx = 0u; tx < targetWidth; ++tx)
		{
			unsigned int values[tChannels] = {0u};
			unsigned int validPixels = 0u;

			const unsigned int patchWidth = tx < twoTargetColumns ? 2u : 3u;

			for (unsigned int i = 0u; i < patchWidth; ++i)
			{
				if (sourceMaskTop[i] == 0xFF)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += sourceTop[i * tChannels + n];
					}

					++validPixels;
				}
			}

			for (unsigned int i = 0u; i < patchWidth; ++i)
			{
				if (sourceMaskBottom[i] == 0xFF)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += sourceBottom[i * tChannels + n];
					}

					++validPixels;
				}
			}

			for (unsigned int i = 0u; i < patchWidth; ++i)
			{
				if (sourceMaskBottom[sourceMaskStrideElements + i] == 0xFF)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += sourceBottom[sourceStrideElements + i * tChannels + n];
					}

					++validPixels;
				}
			}

			if (validPixels != 0u)
			{
				*targetMask = 0xFF;

				const unsigned int validPixels_2 = validPixels / 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					target[n] = (uint8_t)((values[n] + validPixels_2) / validPixels);
				}
			}
			else
			{
				*targetMask = 0x00;

				if (targetMaskHasPixel)
				{
					*targetMaskHasPixel = true;
				}

				if (handleFullMaskPixel)
				{
					for (unsigned int i = 0u; i < patchWidth; ++i)
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							values[n] += sourceTop[i * tChannels + n];
							values[n] += sourceBottom[i * tChannels + n];
							values[n] += sourceBottom[sourceStrideElements + i * tChannels + n];
						}
					}

					validPixels = patchWidth * 3u;
					const unsigned int validPixels_2 = validPixels / 2u;

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						target[n] = uint8_t((values[n] + validPixels_2) / validPixels);
					}
				}
			}

			sourceTop += tChannels * 2u;
			sourceBottom += tChannels * 2u;
			target += tChannels;

			sourceMaskTop += 2;
			sourceMaskBottom += 2;

			++targetMask;
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_SHRINKER_H
