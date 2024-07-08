/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_NEON_H
#define META_OCEAN_CV_ADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_NEON_H

#include "ocean/cv/advanced/Advanced.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#include "ocean/cv/SumSquareDifferencesNEON.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements functions calculating the sum of square differences and omit center pixel.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT SumSquareDifferencesNoCenterNEON
{
	protected:

		/**
		 * Internal helper class providing access to CV::SumSquareDifferencesSSE
		 */
		class SumSquareDifferencesNEON : public CV::SumSquareDifferencesNEON
		{
			// nothing to do here
		};

	public:

		/**
		 * Returns the sum of square differences between two square patches while skipping the center pixel.
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
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferencesNoCenterNEON:: patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

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

	return SumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements)
			- SumSquareDifferences::buffer8BitPerChannel<tChannels, 1u>(patch0 + tPatchSize_2 * image0StrideElements + tPatchSize_2 * tChannels, patch1 + tPatchSize_2 * image1StrideElements + tPatchSize_2 * tChannels);
}

}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_CV_ADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_NEON_H
