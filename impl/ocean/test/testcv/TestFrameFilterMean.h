/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MEAN_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MEAN_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a mean filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterMean
{
	public:

		/**
		 * Tests all mean filter functions.
		 * @param width The width of the test frame in pixel, with range [21, infinity)
		 * @param height The height of the test frame in pixel, with range [21, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the filter with arbitrary size.
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testFilterSizeArbitrary(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the filter result of a frame.
		 * @param source The source frame to which the filter has been applied, must be valid
		 * @param target The target frame with applied filter, must be valid
		 * @param width The width of the source frame, in pixel, with range [1, infinity)
		 * @param height The height of the source frame, in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param filterWidth The width of the filter, in pixel, with range [1, width], must be odd
		 * @param filterHeight The height of the filter, in pixel, with range [1 height], must be ood
		 * @param sourcePaddingElements The number of padding elements at the end of each source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool validationFilter8BitPerChannel(const uint8_t* source, const uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterWidth, const unsigned int filterHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MEAN_H
