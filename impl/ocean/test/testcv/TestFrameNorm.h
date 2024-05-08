/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_TEST_TESTCV_TEST_FRAME_NORM_H
#define OCEAN_TEST_TESTCV_TEST_FRAME_NORM_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrameNorm class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameNorm
{
	public:

		/**
		 * Starts all tests of the FrameNorm class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the function determining the L2 norm.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each element, 'float' or 'double'
		 */
		template <typename T>
		static bool testNormL2(const double testDuration);

	protected:

		/**
		 * Calculates the L2 norm for all elements of a given frame (square root of sum of squared elements).
		 * @param frame The frame for which the L2 norm will be calculated, must be valid
		 * @param width The width of the given frame in pixels (in elements as we have a 1 channel frame), with range [1, infinity)
		 * @param height The height of the given frame in pixels, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range  [0, infinity)
		 * @return The resulting L2 norm, with range [0, infinity)
		 * @tparam T The data type of each element of the given frame 'float' or 'double'
		 */
		template <typename T>
		static double calculateNormL2(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements);
};

}

}

}

#endif // OCEAN_TEST_TESTCV_TEST_FRAME_NORM_H
