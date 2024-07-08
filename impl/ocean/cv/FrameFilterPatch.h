/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_PATCH_H
#define META_OCEAN_CV_FRAME_FILTER_PATCH_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameFilter.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a patch frame filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterPatch : public FrameFilter
{
	public:

		/**
		 * Filters a given 1 channel 8 bit grayscale frame.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtering result, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param patchSize Size of the patches to be used, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static void filter1Channel8Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int patchSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Filters a subset of a given 1 channel 8 bit grayscale frame.
		 * This functions used the integral image of the original frame to speed up the computation.<br>
		 * The given integral image is expected have have an extra border with size identical to the specified patch size.
		 * @param borderedIntegral Bordered integral image to be used for filtering, must be valid
		 * @param target The target frame receiving the filtering result, must be valid
		 * @param width The width of the original image in pixel, with range [1, infinity)
		 * @param height The height of the original image in pixel, with range [1, infinity)
		 * @param patchSize Size of the patches to be used, with range [1, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be filtered, with range [0, height - 1]
		 * @param numberRows Number of rows to be filtered, with range [1, height - firstRow]
		 * @see IntegralImage
		 */
		static void filter1Channel8BitSubset(const uint32_t* borderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int patchSize, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);
};

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_PATCH_H
