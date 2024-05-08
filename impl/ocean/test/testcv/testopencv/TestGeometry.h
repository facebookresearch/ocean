/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_GEOMETRY_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_GEOMETRY_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class tests the performance of several OpenCV functions from the module calib3d (aka geometry in Ocean).
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestGeometry
{
	public:

		/**
		 * Tests all functions of relevant feature detectors in OpenCV.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Returns the name of this test.
		 * @return The name of this test
		 */
		static const std::string& testName();

	protected:

		/**
		 * Test for the OpenCV PnP functionality
		 * @param testDuration Duration in seconds that each of the sub-tests is run
		 * @param pinholeCamera The pinhole camera model; all necessary information for OpenCV is extracted from this model
		 * @param numberOfPoints  Number of (random) image-object point pairs to be used for the test (default: 3, must be >= 3)
		 * @return True if all included test succeeded, otherwise false
		 */
		static bool testPnP(const double testDuration, const PinholeCamera& pinholeCamera, const size_t numberOfPoints = 3);

		/**
		 * Test for the OpenCV PnP functionality (including RANSAC)
		 * @param testDuration Duration in seconds that each of the sub-tests is run
		 * @param pinholeCamera The pinhole camera model; all necessary information for OpenCV is extracted from this model
		 * @param numberOfPoints Number of (random) image-object point pairs to be used for the test (default: 3, must be >= 3)
		 * @return True if all included test succeeded, otherwise false
		 */
		static bool testPnPRANSAC(const double testDuration, const PinholeCamera& pinholeCamera, const size_t numberOfPoints = 3);
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_GEOMETRY_H
