/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_EROSION_H
#define META_OCEAN_CV_FRAME_FILTER_EROSION_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterMorphology.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements an erosion filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterErosion : public FrameFilterMorphology
{
	protected:

		/**
		 * Definition of an unordered set holding pixel positions.
		 */
		typedef std::unordered_set<CV::PixelPosition, CV::PixelPosition> PixelPositionSet;

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
				 * Closes holes inside a frame using a shrinking/in-bleeding approach based on either a 4-neighborhood or an 8-neighborhood.
				 * The frame and mask will be changed during the filtering process.<br>
				 * Hole pixels are defined by a 0x00 mask value while non-hole pixels have 0xFF as musk value.
				 * Beware: Frame and mask must have the same frame dimension and pixel origin.
				 * @param frame The frame to be handled, must be valid
				 * @param mask The 8 bit mask defining the hole(s), must be valid
				 * @param erosionFilter Defines the shape of the neighborhood
				 * @param randomNoise Optional the maximal random noise (+/-) that will be added to each channel and pixel, with range [0, 255]
				 * @param randomSeed The random seed value to be used when generating random values, with range [0, infinity)
				 * @return True, if succeeded
				 */
				static bool shrinkMask(Frame& frame, Frame& mask, const MorphologyFilter erosionFilter, const unsigned int randomNoise = 3u, const unsigned int randomSeed = RandomI::random32());

				/**
				 * Closes a hole inside an 8 bit grayscale frame using a randomized erosion filter.
				 * The frame and mask buffer will be changed during the filtering process.<br>
				 * Hole pixels must have 0x00 as grayscale value, all other pixels must have 0xFF as value.<br>
				 * Beware: Both frame must have the same frame dimension and pixel origin.
				 * @param frame The frame to be handled, must be valid
				 * @param mask The 8 bit mask defining the hole(s), must be valid
				 * @param erosionFilter Defines the shape of the erosion filter mask
				 * @param randomNoise Optional the maximal random noise (+/-) that will be added to each channel and pixel, with range [0, 255]
				 * @param randomSeed The random seed value to be used when generating random values, with range [0, infinity)
				 * @return True, if succeeded
				 */
				static bool shrinkMaskRandom(Frame& frame, Frame& mask, const MorphologyFilter erosionFilter, const unsigned int randomNoise = 3u, const unsigned int randomSeed = RandomI::random32());
		};

		/**
		 * Closes a hole inside a frame by using a shrinking/in-bleeding approach based on a 4-neighborhood.
		 * The frame and mask buffer will be changed during the filtering process.<br>
		 * Hole pixels are defined by a 0x00 mask value while non-hole pixels have 0xFF as musk value.
		 * @param frame The frame to be filtered, must be valid
		 * @param mask The 8 bit mask defining the hole, must be valid
		 * @param width The width of all frames in pixel, with range [2, infinity)
		 * @param height The height of all frames in pixel, with range [2, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param randomNoise Optional the maximal random noise (+/-) that will be added to each channel and pixel, with range [0, 255]
		 * @param randomSeed The random seed value to be used when generating random values, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels with range [1, infinity)
		 * @tparam tUseRandomNoise True, to add/apply random noise to each channel and pixel; False, to create a deterministic result
		 */
		template <unsigned int tChannels, bool tUseRandomNoise>
		static bool shrinkMask8BitPerChannel4Neighbor(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int randomNoise = 0u, const unsigned int randomSeed = RandomI::random32());

		/**
		 * Closes a hole inside a frame by using a shrinking/in-bleeding approach based on an 8-neighborhood.
		 * The frame and mask buffer will be changed during the filtering process.<br>
		 * Hole pixels must have 0x00 as grayscale value, all other pixels must have 0xFF as value.<br>
		 * Beware: All three frame buffers must have the same size.
		 * @param frame The frame to be filtered, must be valid
		 * @param mask The 8 bit mask defining the hole, must be valid
		 * @param width The width of all frames in pixel, with range [1, infinity)
		 * @param height The height of all frames in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param randomNoise Optional the maximal random noise (+/-) that will be added to each channel and pixel, with range [0, 255]
		 * @param randomSeed The random seed value to be used when generating random values, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels with range [1, infinity)
		 * @tparam tUseRandomNoise Optional random noise for each channel and pixel, with range [0, 255], 0 to create a deterministic result
		 */
		template <unsigned int tChannels, bool tUseRandomNoise>
		static bool shrinkMask8BitPerChannel8Neighbor(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int randomNoise = 0u, const unsigned int randomSeed = RandomI::random32());

		/**
		 * Closes a hole inside a frame by using a randomized shrinking/in-bleeding approach based on an 8-neighborhood.
		 * The frame and mask buffer will be changed during the filtering process.<br>
		 * Hole pixels must have 0x00 as grayscale value, all other pixels must have 0xFF as value.
		 * @param frame The frame to be filtered, must be valid
		 * @param mask The 8 bit mask defining the hole, must be valid
		 * @param width The width of all frames in pixel, with range [2, infinity)
		 * @param height The height of all frames in pixel, with range [2, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param randomNoise Optional the maximal random noise (+/-) that will be added to each channel and pixel, with range [0, 255]
		 * @param randomSeed The random seed value to be used when generating random values, with range [0, infinity)
		 * @tparam tChannels The number of frame channels with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void shrinkMaskRandom8BitPerChannel8Neighbor(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int randomNoise = 0u, const unsigned int randomSeed = RandomI::random32());

		/**
		 * Applies several erosion filter iterations for an 8 bit mask image.
		 * The value of a mask pixel (to be eroded) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param width The width of the mask frame in pixel, with range [4, infinity)
		 * @param height The height of the mask frame in pixel, with range [4, infinity)
		 * @param iterations Number of iterations to be applied, best performance when number of iterations is even, with range [1, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tErosionFilter The type of the erosion filter to be applied
		 * @see filter1Channel8Bit4Neighbor(), filter1Channel8Bit8Neighbor().
		 */
		template <MorphologyFilter tErosionFilter>
		static void filter1Channel8Bit(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int iterations, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Applies one erosion filter iteration in an 8 bit mask image using a 4-neighborhood.
		 * The value of a mask pixel (to be eroded) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void filter1Channel8Bit4Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Applies one erosion filter iteration for an 8 bit mask image using a 8-neighborhood, a 3x3 square kernel.
		 * The value of a mask pixel (to be eroded) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void filter1Channel8Bit8Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

		/**
		* Applies one erosion filter iteration for an 8 bit mask image using a 24-neighborhood, a 5x5 square kernel.
		* The value of a mask pixel (to be eroded) can be defined, every other pixel value is interpreted as a non-mask pixels.
		* @param mask The mask frame to be filtered, must be valid
		* @param target The target frame receiving the filter response, must be valid
		* @param width The width of the mask frame in pixel, with range [4, infinity)
		* @param height The height of the mask frame in pixel, with range [4, infinity)
		* @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		* @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		* @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		* @param worker Optional worker object to distribute the computation
		*/
		static inline void filter1Channel8Bit24Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

	private:

		/**
		 * Applies one erosion filter iteration in a subset of an 8 bit mask image using a 4-neighborhood.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 */
		static void filter1Channel8Bit4NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies one erosion filter iteration for an 8 bit mask image using a 8-neighborhood.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 */
		static void filter1Channel8Bit8NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies one erosion filter iteration for an 8 bit mask image using a 24-neighborhood.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [4, infinity)
		 * @param height The height of the mask frame in pixel, with range [4, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 */
		static void filter1Channel8Bit24NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Returns whether at least one of several pixels in a row is not equal to a specified value.
		 * @param maskPixels The first pixel within the row to check, must be valid
		 * @param maskValue The mask value to check, with range [0, 255]
		 * @return True, if so
		 * @tparam tSize The number of pixels in a row to check, with range [1, infinity)
		 */
		template <unsigned int tSize>
		static OCEAN_FORCE_INLINE bool onePixelNotEqual(const uint8_t* const maskPixels, const uint8_t maskValue);
};

template <unsigned int tChannels, bool tUseRandomNoise>
bool FrameFilterErosion::shrinkMask8BitPerChannel4Neighbor(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int randomNoise, const unsigned int randomSeed)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(randomNoise <= 255u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* maskRow = mask + y * maskStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			ocean_assert(maskRow[x] == 0x00u || maskRow[x] == 0xFFu);
		}
	}
#endif // OCEAN_DEBUG

	RandomGenerator randomGenerator(randomSeed);

	/**
	 *   O
	 * O X O
	 *   O
	 */

	Frame intermediateMask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), mask, Frame::CM_COPY_REMOVE_PADDING_LAYOUT, maskPaddingElements);
	ocean_assert(intermediateMask.isContinuous());

	bool atLeastOnePixel = true;

	while (atLeastOnePixel)
	{
		atLeastOnePixel = false;

		const uint8_t* maskMiddle = mask;
		const uint8_t* maskLower = mask + maskStrideElements;

		uint8_t* frameMiddle = frame;
		const uint8_t* frameLower = frame + frameStrideElements;

		uint8_t* intermediateMaskMiddle = intermediateMask.data<uint8_t>();

		// upper left pixel, if the anchor pixel is inside the hole and at least one of the filter pixels is outside the hole
		if (*maskMiddle == 0x00u && (*(maskMiddle + 1) != 0x00u || *maskLower != 0x00u))
		{
			const unsigned int weight = *(maskMiddle + 1) + *maskLower;
			ocean_assert(weight > 0u && weight <= 0xFFu * 2u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);
					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		frameMiddle += tChannels;
		frameLower += tChannels;
		++maskMiddle;
		++maskLower;
		++intermediateMaskMiddle;


		// upper row
		for (unsigned int i = 1u; i < width - 1u; ++i)
		{
			// if the anchor pixel is inside the hole and at least one of the filter pixels is outside the hole
			if (*maskMiddle == 0x00u && (*(maskMiddle - 1) != 0x00u || *(maskMiddle + 1) != 0x00u || *maskLower != 0x00u))
			{
				const unsigned int weight = *(maskMiddle - 1) + *(maskMiddle + 1) + *maskLower;
				ocean_assert(weight > 0u && weight <= 0xFFu * 3u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);
						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameMiddle += tChannels;
			frameLower += tChannels;
			++maskMiddle;
			++maskLower;
			++intermediateMaskMiddle;
		}


		// upper right pixel, if the anchor pixel is inside the hole and at least one of the filter pixels is outside the hole
		if (*maskMiddle == 0 && (*(maskMiddle - 1) != 0u || *maskLower != 0u))
		{
			const unsigned int weight = *(maskMiddle - 1) + *maskLower;
			ocean_assert(weight > 0u && weight <= 0xFFu * 2u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);
					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *maskLower * *(frameLower + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *maskLower * *(frameLower + n)) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		frameMiddle += tChannels + framePaddingElements;
		frameLower += tChannels + framePaddingElements;
		maskMiddle += 1u + maskPaddingElements;
		maskLower += 1u + maskPaddingElements;
		++intermediateMaskMiddle; // intermediate mask has no padding

		ocean_assert(frameMiddle == frame + frameStrideElements);
		ocean_assert(frameLower == frame + frameStrideElements * 2u);
		ocean_assert(maskMiddle == mask + maskStrideElements);
		ocean_assert(maskLower == mask + maskStrideElements * 2u);
		ocean_assert(intermediateMaskMiddle == intermediateMask.data<uint8_t>() + width);


		// center rows

		const uint8_t* maskUpper = maskMiddle - maskStrideElements;
		const uint8_t* frameUpper = frameMiddle - frameStrideElements;

		const uint8_t* const maskUpperEnd = mask + maskStrideElements * (height - 2u);

		while (maskUpper != maskUpperEnd)
		{
			// left pixel
			if (*maskMiddle == 0x00u && (*maskUpper != 0x00u || *(maskMiddle + 1) != 0x00u || *maskLower != 0x00u))
			{
				const unsigned int weight = *maskUpper + *(maskMiddle + 1) + *maskLower;
				ocean_assert(weight > 0u && weight <= 0xFFu * 3u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);
						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*maskUpper * *(frameUpper + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*maskUpper * *(frameUpper + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameUpper += tChannels;
			frameMiddle += tChannels;
			frameLower += tChannels;
			++maskUpper;
			++maskMiddle;
			++maskLower;
			++intermediateMaskMiddle;


			// center pixels
			for (unsigned int i = 1u; i < width - 1u; ++i)
			{
				if (*maskMiddle == 0x00u && (*maskUpper != 0x00u || *(maskMiddle - 1) != 0x00u || *(maskMiddle + 1) != 0x00u || *maskLower != 0x00u))
				{
					const unsigned int weight = *maskUpper + *(maskMiddle - 1) + *(maskMiddle + 1) + *maskLower;
					ocean_assert(weight > 0u && weight <= 0xFFu * 4u);

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						if constexpr (tUseRandomNoise)
						{
							ocean_assert(randomNoise != 0u);
							*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
						}
						else
						{
							*(frameMiddle + n) = uint8_t((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) + *maskLower * *(frameLower + n)) / weight);
						}
					}

					*intermediateMaskMiddle = 0xFFu;
					atLeastOnePixel = true;
				}

				frameUpper += tChannels;
				frameMiddle += tChannels;
				frameLower += tChannels;
				++maskUpper;
				++maskMiddle;
				++maskLower;
				++intermediateMaskMiddle;
			}


			// right pixel
			if (*maskMiddle == 0x00u && (*maskUpper != 0x00u || *(maskMiddle - 1) != 0x00u || *maskLower != 0x00u))
			{
				const unsigned int weight = *maskUpper + *(maskMiddle - 1) + *maskLower;
				ocean_assert(weight > 0u && weight <= 0xFFu * 3u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);
						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *maskLower * *(frameLower + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *maskLower * *(frameLower + n)) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameUpper += tChannels + framePaddingElements;
			frameMiddle += tChannels + framePaddingElements;
			frameLower += tChannels + framePaddingElements;
			maskUpper += 1u + maskPaddingElements;
			maskMiddle += 1u + maskPaddingElements;
			maskLower += 1u + maskPaddingElements;
			++intermediateMaskMiddle; // intermediate mask has no padding elements
		}

		ocean_assert(frameUpper == frame + frameStrideElements * (height - 2u));
		ocean_assert(frameMiddle == frame + frameStrideElements * (height - 1u));
		ocean_assert(maskUpper == mask + maskStrideElements * (height - 2u));
		ocean_assert(maskMiddle == mask + maskStrideElements * (height - 1u));
		ocean_assert(intermediateMaskMiddle == intermediateMask.data<uint8_t>() + width * (height - 1u));

		// bottom left pixel
		if (*maskMiddle == 0x00u && (*maskUpper != 0x00u || *(maskMiddle + 1) != 0x00u))
		{
			const unsigned int weight = *maskUpper + *(maskMiddle + 1);
			ocean_assert(weight > 0u && weight <= 0xFFu * 2u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);
					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*maskUpper * *(frameUpper + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*maskUpper * *(frameUpper + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n)) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		frameUpper += tChannels;
		frameMiddle += tChannels;
		++maskUpper;
		++maskMiddle;
		++intermediateMaskMiddle;


		// center pixels
		for (unsigned int i = 1u; i < width - 1u; ++i)
		{
			if (*maskMiddle == 0x00u && (*maskUpper != 0x00u || *(maskMiddle - 1) != 0x00u || *(maskMiddle + 1) != 0x00u))
			{
				const unsigned int weight = *maskUpper + *(maskMiddle - 1) + *(maskMiddle + 1);
				ocean_assert(weight > 0u && weight <= 0xFFu * 4u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);
						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n) + *(maskMiddle + 1) * *(frameMiddle + tChannels + n)) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameUpper += tChannels;
			frameMiddle += tChannels;
			++maskUpper;
			++maskMiddle;
			++intermediateMaskMiddle;
		}


		// right pixel
		if (*maskMiddle == 0x00u && (*maskUpper != 0x00u || *(maskMiddle - 1) != 0x00u))
		{
			const unsigned int weight = *maskUpper + *(maskMiddle - 1);
			ocean_assert(weight > 0u && weight <= 0xFFu * 3u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);
					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*maskUpper * *(frameUpper + n) + *(maskMiddle - 1) * *(frameMiddle - tChannels + n)) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		if (atLeastOnePixel)
		{
			if (maskPaddingElements == 0u)
			{
				memcpy(mask, intermediateMask.constdata<uint8_t>(), sizeof(uint8_t) * width * height);
			}
			else
			{
				for (unsigned int y = 0u; y < height; ++y)
				{
					memcpy(mask + y * maskStrideElements, intermediateMask.constrow<uint8_t>(y), sizeof(uint8_t) * width);
				}
			}
		}
	}

	return true;
}

template <unsigned int tChannels, bool tUseRandomNoise>
bool FrameFilterErosion::shrinkMask8BitPerChannel8Neighbor(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int randomNoise, const unsigned int randomSeed)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(width >= 2 && height >= 2u);
	ocean_assert(randomNoise <= 255u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* maskRow = mask + y * maskStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			ocean_assert(maskRow[x] == 0x00u || maskRow[x] == 0xFFu);
		}
	}
#endif // OCEAN_DEBUG

	RandomGenerator randomGenerator(randomSeed);

	/**
	 * O O O
	 * O X O
	 * O O O
	 */

	Frame intermediateMask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), mask, Frame::CM_COPY_REMOVE_PADDING_LAYOUT, maskPaddingElements);
	ocean_assert(intermediateMask.isContinuous());

	bool atLeastOnePixel = true;

	while (atLeastOnePixel)
	{
		atLeastOnePixel = false;

		const uint8_t* maskMiddle = mask;
		const uint8_t* maskLower = mask + maskStrideElements;

		uint8_t* frameMiddle = frame;
		const uint8_t* frameLower = frame + frameStrideElements;

		uint8_t* intermediateMaskMiddle = intermediateMask.data<uint8_t>();

		// upper left pixel, if the anchor pixel is inside the hole and at least one of the filter pixels is outside the hole
		if (*maskMiddle == 0x00u && (*(maskMiddle + 1) != 0x00u || *(maskLower + 0) != 0x00u || *(maskLower + 1) != 0x00u))
		{
			const unsigned int weight = *(maskMiddle + 1) * 2u + *(maskLower + 0) * 2u + *(maskLower + 1);
			ocean_assert(weight > 0u && weight <= 0xFF * 12u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);

					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
											+ *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
											+ *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		frameMiddle += tChannels;
		frameLower += tChannels;
		++maskMiddle;
		++maskLower;
		++intermediateMaskMiddle;


		// upper row
		for (unsigned int i = 1u; i < width - 1u; ++i)
		{
			// if the anchor pixel is inside the hole and at least one of the filter pixels is outside the hole
			if (*maskMiddle == 0x00u && (*(maskMiddle - 1) != 0x00u || *(maskMiddle + 1) != 0x00u || *(maskLower - 1) != 0x00u || *(maskLower + 0) != 0x00u || *(maskLower + 1) != 0x00u))
			{
				const unsigned int weight = *(maskMiddle - 1) * 2u + *(maskMiddle + 1) * 2u + *(maskLower - 1) + *(maskLower + 0) * 2u + *(maskLower + 1);
				ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);

						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
												+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
												+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameMiddle += tChannels;
			frameLower += tChannels;
			++maskMiddle;
			++maskLower;
			++intermediateMaskMiddle;
		}


		// upper right pixel
		if (*maskMiddle == 0x00u && (*(maskMiddle - 1) != 0x00u || *(maskLower - 1) != 0x00u || *(maskLower + 0) != 0x00u))
		{
			const unsigned int weight = *(maskMiddle - 1) * 2u + *(maskLower - 1) + *(maskLower + 0) * 2u;
			ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);

					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u
											+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u
											+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFF;
			atLeastOnePixel = true;
		}

		frameMiddle += tChannels + framePaddingElements;
		frameLower += tChannels + framePaddingElements;
		maskMiddle += 1u + maskPaddingElements;
		maskLower += 1u + maskPaddingElements;
		++intermediateMaskMiddle; // intermediate mask has no padding

		ocean_assert(frameMiddle == frame + frameStrideElements);
		ocean_assert(frameLower == frame + frameStrideElements * 2u);
		ocean_assert(maskMiddle == mask + maskStrideElements);
		ocean_assert(maskLower == mask + maskStrideElements * 2u);
		ocean_assert(intermediateMaskMiddle == intermediateMask.data<uint8_t>() + width);


		// center rows

		const uint8_t* maskUpper = maskMiddle - maskStrideElements;
		const uint8_t* frameUpper = frameMiddle - frameStrideElements;

		const uint8_t* const maskUpperEnd = mask + maskStrideElements * (height - 2u);

		while (maskUpper != maskUpperEnd)
		{
			// left pixel
			if (*maskMiddle == 0x00u && (*(maskUpper + 0) != 0x00u || *(maskUpper + 1) != 0x00u || *(maskMiddle + 1) != 0x00u || *(maskLower + 0) != 0x00u || *(maskLower + 1) != 0x00u))
			{
				const unsigned int weight = *(maskUpper + 0) * 2u + *(maskUpper + 1) + *(maskMiddle + 1) * 2u + *(maskLower + 0) * 2u + *(maskLower + 1);
				ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);

						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
												+ *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
												+ *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
												+ *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
												+ *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameUpper += tChannels;
			frameMiddle += tChannels;
			frameLower += tChannels;
			++maskUpper;
			++maskMiddle;
			++maskLower;
			++intermediateMaskMiddle;


			// center pixels
			for (unsigned int i = 1u; i < width - 1u; ++i)
			{
				if (*maskMiddle == 0x00u && (*(maskUpper - 1) != 0x00u || *(maskUpper + 0) != 0x00u || *(maskUpper + 1) != 0x00u || *(maskMiddle - 1) != 0x00u || *(maskMiddle + 1) != 0x00u || *(maskLower - 1) != 0x00u || *(maskLower + 0) != 0x00u || *(maskLower + 1) != 0x00u))
				{
					const unsigned int weight = *(maskUpper - 1) + *(maskUpper + 0) * 2u + *(maskUpper + 1) + *(maskMiddle - 1) * 2u + *(maskMiddle + 1) * 2u + *(maskLower - 1) + *(maskLower + 0) * 2u + *(maskLower + 1);
					ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						if constexpr (tUseRandomNoise)
						{
							ocean_assert(randomNoise != 0u);

							*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
													+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
													+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
						}
						else
						{
							*(frameMiddle + n) = uint8_t((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
													+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u
													+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u + *(maskLower + 1) * *(frameLower + tChannels + n)) / weight);
						}
					}

					*intermediateMaskMiddle = 0xFFu;
					atLeastOnePixel = true;
				}

				frameUpper += tChannels;
				frameMiddle += tChannels;
				frameLower += tChannels;
				++maskUpper;
				++maskMiddle;
				++maskLower;
				++intermediateMaskMiddle;
			}


			// right pixels
			if (*maskMiddle == 0x00u && (*(maskUpper - 1) != 0x00u || *(maskUpper + 0) != 0x00u || *(maskMiddle - 1) != 0x00u || *(maskLower - 1) != 0x00u || *(maskLower + 0) != 0x00u))
			{
				const unsigned int weight = *(maskUpper - 1) + *(maskUpper + 0) * 2u + *(maskMiddle - 1) * 2u + *(maskLower - 1) + *(maskLower + 0) * 2u;
				ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);

						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u
												+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u
												+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u
												+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u
												+ *(maskLower - 1) * *(frameLower - tChannels + n) + *(maskLower + 0) * *(frameLower + n) * 2u) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameUpper += tChannels + framePaddingElements;
			frameMiddle += tChannels + framePaddingElements;
			frameLower += tChannels + framePaddingElements;
			maskUpper += 1u + maskPaddingElements;
			maskMiddle += 1u + maskPaddingElements;
			maskLower += 1u + maskPaddingElements;
			++intermediateMaskMiddle; // intermediate mask has no padding elements
		}

		ocean_assert(frameUpper == frame + frameStrideElements * (height - 2u));
		ocean_assert(frameMiddle == frame + frameStrideElements * (height - 1u));
		ocean_assert(maskUpper == mask + maskStrideElements * (height - 2u));
		ocean_assert(maskMiddle == mask + maskStrideElements * (height - 1u));
		ocean_assert(intermediateMaskMiddle == intermediateMask.data<uint8_t>() + width * (height - 1u));


		// bottom left pixel
		if (*maskMiddle == 0x00u && (*(maskUpper + 0) != 0x00u || *(maskUpper + 1) != 0x00u || *(maskMiddle + 1) != 0x00u))
		{
			const unsigned int weight = *(maskUpper + 0) * 2u + *(maskUpper + 1) + *(maskMiddle + 1) * 2u;
			ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);

					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
											+ *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
											+ *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		frameUpper += tChannels;
		frameMiddle += tChannels;
		++maskUpper;
		++maskMiddle;
		++intermediateMaskMiddle;


		// center pixels
		for (unsigned int i = 1u; i < width - 1u; ++i)
		{
			if (*maskMiddle == 0x00u && (*(maskUpper - 1) != 0x00u || *(maskUpper + 0) != 0x00u || *(maskUpper + 1) != 0x00u || *(maskMiddle - 1) != 0x00u || *(maskMiddle + 1) != 0x00u))
			{
				const unsigned int weight = *(maskUpper - 1) + *(maskUpper + 0) * 2u + *(maskUpper + 1) + *(maskMiddle - 1) * 2u + *(maskMiddle + 1) * 2u;
				ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					if constexpr (tUseRandomNoise)
					{
						ocean_assert(randomNoise != 0u);

						*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
												+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
					}
					else
					{
						*(frameMiddle + n) = uint8_t((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u + *(maskUpper + 1) * *(frameUpper + tChannels + n)
												+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u + *(maskMiddle + 1) * *(frameMiddle + tChannels + n) * 2u) / weight);
					}
				}

				*intermediateMaskMiddle = 0xFFu;
				atLeastOnePixel = true;
			}

			frameUpper += tChannels;
			frameMiddle += tChannels;
			++maskUpper;
			++maskMiddle;
			++intermediateMaskMiddle;
		}


		// right pixel
		if (*maskMiddle == 0x00u && (*(maskUpper - 1) != 0x00u || *(maskUpper + 0) != 0x00u || *(maskMiddle - 1) != 0x00u))
		{
			const unsigned int weight = *(maskUpper - 1) + *(maskUpper + 0) * 2u + *(maskMiddle - 1) * 2u;
			ocean_assert(weight > 0u && weight <= 0xFFu * 12u);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tUseRandomNoise)
				{
					ocean_assert(randomNoise != 0u);

					*(frameMiddle + n) = uint8_t(minmax<int>(0, int((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u
											+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u) / weight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255));
				}
				else
				{
					*(frameMiddle + n) = uint8_t((*(maskUpper - 1) * *(frameUpper - tChannels + n) + *(maskUpper + 0) * *(frameUpper + n) * 2u
											+ *(maskMiddle - 1) * *(frameMiddle - tChannels + n) * 2u) / weight);
				}
			}

			*intermediateMaskMiddle = 0xFFu;
			atLeastOnePixel = true;
		}

		if (atLeastOnePixel)
		{
			if (maskPaddingElements == 0u)
			{
				memcpy(mask, intermediateMask.constdata<uint8_t>(), sizeof(uint8_t) * width * height);
			}
			else
			{
				for (unsigned int y = 0u; y < height; ++y)
				{
					memcpy(mask + y * maskStrideElements, intermediateMask.constrow<uint8_t>(y), sizeof(uint8_t) * width);
				}
			}
		}
	}

	return true;
}

template <unsigned int tChannels>
void FrameFilterErosion::shrinkMaskRandom8BitPerChannel8Neighbor(uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int randomNoise, const unsigned int randomSeed)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(width >= 2u && height >= 2u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	RandomGenerator randomGenerator(randomSeed);

	/**
	 * O O O
	 * O X O
	 * O O O
	 */

	PixelPositionSet borderPixelSet;
	CV::PixelPositions borderPixels;

	borderPixelSet.reserve(1024);
	borderPixels.reserve(1024);

	constexpr uint8_t nonMaskValue = 0xFFu;

	{
		// find all border pixels

		const uint8_t* maskUpper = mask - maskStrideElements;
		const uint8_t* maskMiddle = mask;
		const uint8_t* maskLower = mask + maskStrideElements;

		for (unsigned int y = 0u; y < height; ++y)
		{
			unsigned int x = 0u;

			if (y == 0u)
			{
				// top left pixel
				if (maskMiddle[x] != nonMaskValue && (maskMiddle[x + 1u] == nonMaskValue || maskLower[x] == nonMaskValue || maskLower[x + 1u] == nonMaskValue))
				{
					borderPixelSet.emplace(x, 0u);
					borderPixels.emplace_back(x, 0u);
				}

				while (++x < width_1)
				{
					if (*(maskMiddle + x) != nonMaskValue && (*(maskMiddle + x - 1u) == nonMaskValue || *(maskMiddle + x + 1u) == nonMaskValue || *(maskLower + x - 1u) == nonMaskValue || *(maskLower + x) == nonMaskValue || *(maskLower + x + 1u) == nonMaskValue))
					{
						borderPixelSet.emplace(x, 0u);
						borderPixels.emplace_back(x, 0u);
					}
				}

				ocean_assert(x == width_1);

				// top right pixel
				if (*(maskMiddle + x) != nonMaskValue && (*(maskMiddle + x - 1u) == nonMaskValue || *(maskLower + x - 1u) == nonMaskValue || *(maskLower + x) == nonMaskValue))
				{
					borderPixelSet.emplace(x, 0u);
					borderPixels.emplace_back(x, 0u);
				}
			}
			else if (y == height_1)
			{
				// bottom left pixel
				if (maskMiddle[x] != nonMaskValue && (maskMiddle[x + 1u] == nonMaskValue || maskUpper[x] == nonMaskValue || maskUpper[x + 1u] == nonMaskValue))
				{
					borderPixelSet.emplace(x, height_1);
					borderPixels.emplace_back(x, height_1);
				}

				while (++x < width_1)
				{
					if (*(maskMiddle + x) != nonMaskValue && (*(maskMiddle + x - 1u) == nonMaskValue || *(maskMiddle + x + 1u) == nonMaskValue || *(maskUpper + x - 1u) == nonMaskValue || *(maskUpper + x) == nonMaskValue || *(maskUpper + x + 1u) == nonMaskValue))
					{
						borderPixelSet.emplace(x, height_1);
						borderPixels.emplace_back(x, height_1);
					}
				}

				ocean_assert(x == width_1);

				// bottom right pixel
				if (*(maskMiddle + x) != nonMaskValue && (*(maskMiddle + x - 1u) == nonMaskValue || *(maskUpper + x - 1u) == nonMaskValue || *(maskUpper + x) == nonMaskValue))
				{
					borderPixelSet.emplace(x, height_1);
					borderPixels.emplace_back(x, height_1);
				}
			}
			else
			{
				ocean_assert(y >= 1u && y < height_1);

				// left pixel
				if (maskMiddle[x] != nonMaskValue && (maskMiddle[x + 1u] == nonMaskValue || maskUpper[x] == nonMaskValue || maskUpper[x + 1u] == nonMaskValue || maskLower[x] == nonMaskValue || maskLower[x + 1u] == nonMaskValue))
				{
					borderPixelSet.emplace(x, y);
					borderPixels.emplace_back(x, y);
				}

				while (++x < width_1)
				{
					if (*(maskMiddle + x) != nonMaskValue && (*(maskMiddle + x - 1u) == nonMaskValue || *(maskMiddle + x + 1u) == nonMaskValue || *(maskUpper + x - 1u) == nonMaskValue || *(maskUpper + x) == nonMaskValue || *(maskUpper + x + 1u) == nonMaskValue || *(maskLower + x - 1u) == nonMaskValue || *(maskLower + x) == nonMaskValue || *(maskLower + x + 1u) == nonMaskValue))
					{
						borderPixelSet.emplace(x, y);
						borderPixels.emplace_back(x, y);
					}
				}

				ocean_assert(x == width_1);

				// right pixel
				if (*(maskMiddle + x) != nonMaskValue && (*(maskMiddle + x - 1u) == nonMaskValue || *(maskUpper + x - 1u) == nonMaskValue || *(maskUpper + x) == nonMaskValue || *(maskLower + x - 1u) == nonMaskValue || *(maskLower + x) == nonMaskValue))
				{
					borderPixelSet.emplace(x, y);
					borderPixels.emplace_back(x, y);
				}
			}

			maskUpper += maskStrideElements;
			maskMiddle += maskStrideElements;
			maskLower += maskStrideElements;
		}
	}

	while (!borderPixels.empty())
	{
		const unsigned int index = Random::random(randomGenerator, (unsigned int)(borderPixels.size()) - 1u);

		const CV::PixelPosition pixelPosition(borderPixels[index]);

		borderPixels[index] = borderPixels.back();
		borderPixels.pop_back();

		borderPixelSet.erase(pixelPosition);

		ocean_assert(pixelPosition.x() < width && pixelPosition.y() < height);

		uint8_t* maskMiddle = mask + pixelPosition.y() * maskStrideElements + pixelPosition.x();
		--maskMiddle; // left pixel of 3x3 square
		const uint8_t* maskUpper = maskMiddle - maskStrideElements;
		const uint8_t* maskLower = maskMiddle + maskStrideElements;

		uint8_t* frameMiddle = frame + pixelPosition.y() * frameStrideElements + pixelPosition.x() * tChannels;
		frameMiddle -= tChannels; // left pixel of 3x3 square
		const uint8_t* const frameUpper = frameMiddle - frameStrideElements;
		const uint8_t* const frameLower = frameMiddle + frameStrideElements;

		if (pixelPosition.x() >= 1u && pixelPosition.y() >= 1u && pixelPosition.x() < width_1 && pixelPosition.y() < height_1)
		{
			// we have a center pixel

			const unsigned int weight = maskUpper[0] + maskUpper[1] * 2u + maskUpper[2] + maskMiddle[0] * 2u + maskMiddle[2] * 2u + maskLower[0] + maskLower[1] * 2u + maskLower[2];

			ocean_assert(weight > 0u && weight <= 0xFF * 12u);
			const unsigned int weight_2 = weight / 2u;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const unsigned int value = frameUpper[n] * maskUpper[0] + frameUpper[tChannels + n] * maskUpper[1] * 2u + frameUpper[tChannels * 2u + n] * maskUpper[2]
												+ frameMiddle[n] * maskMiddle[0] * 2u + frameMiddle[tChannels * 2u + n] * maskMiddle[2] * 2u
												+ frameLower[n] * maskLower[0] + frameLower[tChannels + n] * maskLower[1] * 2u + frameLower[tChannels * 2u + n] * maskLower[2];

				const int noise = randomNoise == 0u ? 0 : RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise));

				frameMiddle[tChannels + n] = uint8_t(minmax<int>(0, int((value + weight_2) / weight) + noise, 255));
			}

			maskMiddle[1] = nonMaskValue;
		}
		else
		{
			// we have a frame border pixel

			unsigned int weight = 0u;
			unsigned int values[tChannels] = {0u};

			if (pixelPosition.y() > 0u)
			{
				if (pixelPosition.x() > 0u)
				{
					weight += maskUpper[0];

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += frameUpper[n] * maskUpper[0];
					}
				}

				weight += maskUpper[1] * 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					values[n] += frameUpper[tChannels + n] * maskUpper[1] * 2u;
				}

				if (pixelPosition.x() < width_1)
				{
					weight += maskUpper[2];

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += frameUpper[tChannels * 2u + n] * maskUpper[2];
					}
				}
			}

			if (pixelPosition.x() > 0u)
			{
				weight += maskMiddle[0] * 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					values[n] += frameMiddle[n] * maskMiddle[0] * 2u;
				}
			}

			if (pixelPosition.x() < width_1)
			{
				weight += maskMiddle[2] * 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					values[n] += frameMiddle[tChannels * 2u + n] * maskMiddle[2] * 2u;
				}
			}

			if (pixelPosition.y() < height_1)
			{
				if (pixelPosition.x() > 0u)
				{
					weight += maskLower[0];

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += frameLower[n] * maskLower[0];
					}
				}

				weight += maskLower[1] * 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					values[n] += frameLower[tChannels + n] * maskLower[1] * 2u;
				}

				if (pixelPosition.x() < width_1)
				{
					weight += maskLower[2];

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						values[n] += frameLower[tChannels * 2u + n] * maskLower[2];
					}
				}
			}

			ocean_assert(weight > 0u && weight <= 0xFF * 12u);
			const unsigned int weight_2 = weight / 2u;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const int noise = randomNoise == 0u ? 0 : RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise));

				frameMiddle[tChannels + n] = uint8_t(minmax<int>(0, int((values[n] + weight_2) / weight) + noise, 255));
			}

			maskMiddle[1] = nonMaskValue;
		}

		for (int yy = -1; yy <= 1; ++yy)
		{
			const unsigned int yCenter = (unsigned int)(int(pixelPosition.y()) + yy);

			if (yCenter < height)
			{
				for (int xx = -1; xx <= 1; ++xx)
				{
					if (yy != 0 || xx != 0)
					{
						const unsigned int xCenter = (unsigned int)(int(pixelPosition.x()) + xx);

						if (xCenter < width)
						{
							maskMiddle = mask + yCenter * maskStrideElements + xCenter;
							--maskMiddle; // left pixel of 3x3 square

							if (maskMiddle[1] != nonMaskValue)
							{
								if (borderPixelSet.find(CV::PixelPosition(xCenter, yCenter)) == borderPixelSet.cend())
								{
									maskUpper = maskMiddle - maskStrideElements;
									maskLower = maskMiddle + maskStrideElements;

									const bool newBorderPixel = (xCenter > 0u && yCenter > 0u && maskUpper[0] == nonMaskValue) // upper pixels
																	|| (yCenter > 0u && maskUpper[1] == nonMaskValue)
																	|| (xCenter < width_1 && yCenter > 0u && maskUpper[2] == nonMaskValue)
																	|| (xCenter > 0u && maskMiddle[0] == nonMaskValue) // middle pixels
																	|| (xCenter < width_1 && maskMiddle[2] == nonMaskValue)
																	|| (xCenter > 0u && yCenter < height_1 && maskLower[0] == nonMaskValue) // lower pixels
																	|| (yCenter < height_1 && maskLower[1] == nonMaskValue)
																	|| (xCenter < width_1 && yCenter < height_1 && maskLower[2] == nonMaskValue);

									if (newBorderPixel)
									{
										borderPixelSet.emplace(xCenter, yCenter);
										borderPixels.emplace_back(xCenter, yCenter);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

template <FrameFilterErosion::MorphologyFilter tErosionFilter>
void FrameFilterErosion::filter1Channel8Bit(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int iterations, const uint8_t maskValue, const unsigned int maskPaddingElements, Worker* worker)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 4u && height >= 4u);
	ocean_assert(iterations >= 1u);

	Frame intermediateTarget(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	switch (tErosionFilter)
	{
		case MF_CROSS_3:
		{
			for (unsigned int n = 0u; n < iterations / 2u; ++n)
			{
				filter1Channel8Bit4Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
				filter1Channel8Bit4Neighbor(intermediateTarget.constdata<uint8_t>(), mask, width, height, maskValue, intermediateTarget.paddingElements(), maskPaddingElements, worker);
			}

			if (iterations % 2u == 1u)
			{
				filter1Channel8Bit4Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
			}

			break;
		}

		case MF_SQUARE_3:
		{
			for (unsigned int n = 0u; n < iterations / 2u; ++n)
			{
				filter1Channel8Bit8Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
				filter1Channel8Bit8Neighbor(intermediateTarget.constdata<uint8_t>(), mask, width, height, maskValue, intermediateTarget.paddingElements(), maskPaddingElements, worker);
			}

			if (iterations % 2u == 1u)
			{
				filter1Channel8Bit8Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
			}

			break;
		}

		case MF_SQUARE_5:
		{
			for (unsigned int n = 0u; n < iterations / 2u; ++n)
			{
				filter1Channel8Bit24Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
				filter1Channel8Bit24Neighbor(intermediateTarget.constdata<uint8_t>(), mask, width, height, maskValue, intermediateTarget.paddingElements(), maskPaddingElements, worker);
			}

			if (iterations % 2u == 1u)
			{
				filter1Channel8Bit24Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid erosion filter!");
	}

	if (iterations % 2u == 1u)
	{
		CV::FrameConverter::subFrame<uint8_t>(intermediateTarget.constdata<uint8_t>(), mask, width, height, width, height, 1u, 0u, 0u, 0u, 0u, width, height, intermediateTarget.paddingElements(), maskPaddingElements);
	}
}

inline void FrameFilterErosion::filter1Channel8Bit4Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(mask && target);
	ocean_assert(width >= 2u && height >= 2u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterErosion::filter1Channel8Bit4NeighborSubset, mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filter1Channel8Bit4NeighborSubset(mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, height);
	}
}

inline void FrameFilterErosion::filter1Channel8Bit8Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(mask && target);
	ocean_assert(width >= 2u && height >= 2u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterErosion::filter1Channel8Bit8NeighborSubset, mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filter1Channel8Bit8NeighborSubset(mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, height);
	}
}

inline void FrameFilterErosion::filter1Channel8Bit24Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(mask && target);
	ocean_assert(width >= 2u && height >= 2u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterErosion::filter1Channel8Bit24NeighborSubset, mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filter1Channel8Bit24NeighborSubset(mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <>
OCEAN_FORCE_INLINE bool FrameFilterErosion::onePixelNotEqual<3u>(const uint8_t* const maskPixels, const uint8_t maskValue)
{
	ocean_assert(maskPixels != nullptr);

	return maskPixels[0] != maskValue || maskPixels[1] != maskValue || maskPixels[2] != maskValue;
}

template <>
OCEAN_FORCE_INLINE bool FrameFilterErosion::onePixelNotEqual<4u>(const uint8_t* const maskPixels, const uint8_t maskValue)
{
	ocean_assert(maskPixels != nullptr);

	return maskPixels[0] != maskValue || maskPixels[1] != maskValue || maskPixels[2] != maskValue || maskPixels[3] != maskValue;
}

template <>
OCEAN_FORCE_INLINE bool FrameFilterErosion::onePixelNotEqual<5u>(const uint8_t* const maskPixels, const uint8_t maskValue)
{
	ocean_assert(maskPixels != nullptr);

	return maskPixels[0] != maskValue || maskPixels[1] != maskValue || maskPixels[2] != maskValue || maskPixels[3] != maskValue || maskPixels[4] != maskValue;
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_EROSION_H
