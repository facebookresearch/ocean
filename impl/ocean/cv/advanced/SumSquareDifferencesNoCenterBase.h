/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_BASE_H
#define META_OCEAN_CV_ADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_BASE_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/cv/SumSquareDifferences.h"
#include "ocean/cv/SumSquareDifferencesBase.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements functions calculating the sum of square differences and omits center pixel.
 * @ingroup cvadvanced
 */
class SumSquareDifferencesNoCenterBase
{
	protected:

		/**
		 * Internal helper class providing access to CV::SumSquareDifferencesBase
		 */
		class SumSquareDifferencesBase : public CV::SumSquareDifferencesBase
		{
			// nothing to do here
		};

	public:

		/**
		 * Returns the sum of square differences between two square patches while skipping the center pixel.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize / 2, width1 - tPatchSize / 2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize / 2, height1 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static OCEAN_FORCE_INLINE uint32_t patch8BitPerChannelTemplate(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two square patches.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [patchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [patchSize, infinity)
		 * @param channels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param centerX0 Horizontal center position of the (patchSize x patchSize) block in the first frame, with range [patchSize / 2, width0 - patchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (patchSize x patchSize) block in the first frame, with range [patchSize / 2, height0 - patchSize / 2 - 1]
		 * @param centerX1 Horizontal center position of the (patchSize x patchSize) block in the second frame, with range [patchSize / 2, width1 - patchSize / 2 - 1]
		 * @param centerY1 Vertical center position of the (patchSize x patchSize) block in the second frame, with range [patchSize / 2, height1 - patchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 */
		static OCEAN_FORCE_INLINE uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferencesNoCenterBase::patch8BitPerChannelTemplate(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	return SumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements)
				- SumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, 1u>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

uint32_t SumSquareDifferencesNoCenterBase::patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	return SumSquareDifferencesBase::patch8BitPerChannel(image0, image1, channels, patchSize, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements)
				- SumSquareDifferencesBase::patch8BitPerChannel(image0, image1, channels, 1u, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_BASE_H
