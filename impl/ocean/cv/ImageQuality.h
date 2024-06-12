/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_IMAGE_QUALITY_H
#define META_OCEAN_CV_IMAGE_QUALITY_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions to measure the quality of images.
 * @ingroup oceancv
 */
class OCEAN_CV_EXPORT ImageQuality
{
	public:

		/**
		 * Determines the structural similarity (SSIM) of two images with identical frame type for images with pixel formats that have 8 bit per channel.
		 * @param imageX The first image to be used, must be valid
		 * @param imageY The second image to be used, e.g., a compressed/noisy version of image x, must be valid
		 * @param width The width of both images in pixel, with range [11, infinity)
		 * @param height The height of both images in pixel, with range [11, infinity)
		 * @param channels The number of data channels both images have, with range [1, infinity)
		 * @param imageXPaddingElements Optional number of padding elements at the end of each first image row, in elements, with range [0, infinity)
		 * @param imageYPaddingElements Optional number of padding elements at the end of each second image row, in elements, with range [0, infinity)
		 * @param meanSSIM The resulting ssim index, with range [0, 1]
		 * @param meanContrast The resulting mean contrast measure, with range [0, 1]
		 * @param worker Optional worker object to distribute the computation
		 * @see multiScaleStructuralSimilarity8BitPerChannel().
		 */
		static bool structuralSimilarity8BitPerChannel(const uint8_t* const imageX, const uint8_t* const imageY, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int imageXPaddingElements, const unsigned int imageYPaddingElements, double& meanSSIM, double& meanContrast, Worker* worker = nullptr);

		/**
		 * Determines the multi-scale structural similarity (MSSSIM) of two images with identical frame type for images with pixel formats that have 8 bit per channel.
		 *
		 * Beware: For backwards-compatibility the implementation of this function does not fully match that in the paper "Multi-Scale Structural Similarity for Image Quality Assessment"
		 *
		 * @param imageX The first image to be used, must be valid
		 * @param imageY The second image to be used, must be valid
		 * @param width The width of both images in pixel, with range [11, infinity)
		 * @param height The height of both images in pixel, with range [11, infinity)
		 * @param channels The number of data channels both images have, with range [1, 4]
		 * @param imageXPaddingElements Optional number of padding elements at the end of each first image row, in elements, with range [0, infinity)
		 * @param imageYPaddingElements Optional number of padding elements at the end of each second image row, in elements, with range [0, infinity)
		 * @param msssim The resulting msssim index, with range [0, 1]
		 * @param worker Optional worker object to distribute the computation
		 * @see structuralSimilarity8BitPerChannel().
		 */
		static bool multiScaleStructuralSimilarity8BitPerChannel(const uint8_t* const imageX, const uint8_t* const imageY, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int imageXPaddingElements, const unsigned int imageYPaddingElements, double& msssim, Worker* worker = nullptr);
};

}

}

#endif // META_OCEAN_CV_IMAGE_QUALITY_H
