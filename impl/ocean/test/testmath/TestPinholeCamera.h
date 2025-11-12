/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_PINHOLE_CAMERA_H
#define META_OCEAN_TEST_TESTMATH_PINHOLE_CAMERA_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements tests for the PinholeCamera.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestPinholeCamera
{
	public:

		/**
		 * Tests all pinhole camera functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructors of the camera function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testCameraConstructor(const double testDuration);

		/**
		 * Tests the pattern camera function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testPatternCamera(const double testDuration);

		/**
		 * Tests the sub-frame camera function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testSubFrameCamera(const double testDuration);

		/**
		 * Tests the distortion function.
		 * @param width The width of the camera image in pixel
		 * @param height The height of the camera image in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testDistortion(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the vector function with an undistorted camera.
		 * @param width The width of the camera image in pixel
		 * @param height The height of the camera image in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testVectorDistortionFree(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the vector function with a distorted camera.
		 * @param width The width of the camera image in pixel
		 * @param height The height of the camera image in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testVectorDistorted(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the diagonal field of view function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static bool testFovDiagonal(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_PINHOLE_CAMERA_H
