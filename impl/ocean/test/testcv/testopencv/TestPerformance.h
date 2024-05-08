/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_PERFORMANCE_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_PERFORMANCE_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class tests the performance of the frame interpolation function of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestPerformance
{
	public:
		/**
		 * Runs a series of performance comparisons between Ocean and OpenCV algorithms
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

	private:

		static bool performancePerspectiveWarp(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_PERFORMANCE_H
