/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_MORPHOLOGY_H
#define META_OCEAN_CV_FRAME_FILTER_MORPHOLOGY_H

#include "ocean/cv/CV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements the base class for all morphology frame filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterMorphology
{
	public:

		/**
		 * Definition of individual morphology masks.
		 */
		enum MorphologyFilter
		{
			/// Cross mask filter defined by one center point and 4 (north, south, west, east) filter pixels with an area of 3x3.
			MF_CROSS_3,
			/// Square mask filter defined by one center point and 8 surrounded filter pixels with an area of 3x3.
			MF_SQUARE_3,
			/// Square mask filter defined by one center point and 24 surrounded filter pixels with an area of 5x5.
			MF_SQUARE_5
		};

	public:

		/**
		 * Applies an erosion and dilation filter iteration to the given mask to remove small mask elements or single pixels.
		 * The value of a mask pixel (to be dilated) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask 8 bit binary Mask to be opened by erosion and dilation iterations, must be valid
		 * @param width The width of the mask in pixel, with range [4, infinity)
		 * @param height The height of the mask in pixel, with range [4, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param maskValue The value of a mask pixel to be dilated, pixels with other values will be untouched, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tFilter The filter pattern to be applied, either 'MF_SQUARE_3' or 'MF_SQUARE_5'
		 */
		template <MorphologyFilter tFilter>
		static void openMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Applies a dilation and erosion filter iteration to the given mask to close small gaps between mask pixels.
		 * The value of a mask pixel (to be dilated) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask 8 bit binary Mask to be opened by erosion and dilation iterations, must be valid
		 * @param width The width of the mask in pixel, with range [4, infinity)
		 * @param height The height of the mask in pixel, with range [4, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param maskValue The value of a mask pixel to be dilated, pixels with other values will be untouched, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tFilter The filter pattern to be applied, either 'MF_SQUARE_3' or 'MF_SQUARE_5'
		 */
		template <MorphologyFilter tFilter>
		static void closeMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker = nullptr);
};

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_MORPHOLOGY_H
