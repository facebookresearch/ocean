/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_CANVAS_H
#define META_OCEAN_TEST_TESTCV_TEST_CANVAS_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the Canvas class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestCanvas
{
	protected:

		/**
		 * Definition of an unordered map mapping distances to pixel values.
		 */
		using DistanceMap = std::unordered_map<CV::PixelPosition, const uint8_t*, CV::PixelPosition>;

	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the color value functions.
		 * @return True, if succeeded
		 */
		static bool testColors();

		/**
		 * Tests the line drawing function with pixel accuracy.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinePixelAccuracy(const double testDuration);

		/**
		 * Tests the point function when using a point location without fraction (perfectly placed in the center of a pixel).
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPointNoFraction(const double testDuration);

		/**
		 * Tests the point function when using a point location with fraction (with arbitrary location).
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPointWithFraction(const double testDuration);

	protected:

		/**
		 * Returns the sum of squared differences between the color values of two pixels.
		 * @param pixel0 The first pixel, must be valid
		 * @param pixel1 The second pixel, must be valid
		 * @param channels The number of channels, with range [1, infinity)
		 */
		static unsigned int ssd(const uint8_t* pixel0, const uint8_t* pixel1, const unsigned int channels);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_CANVAS_H
