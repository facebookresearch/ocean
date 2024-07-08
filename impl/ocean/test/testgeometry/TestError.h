/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_ERROR_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_ERROR_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the test for the Error class of the geometry library.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestError
{
	public:

		/**
		 * Tests the entire error functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the pose error determination function for separate error values with pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeterminePoseErrorSeparatePinhole(const double testDuration);

		/**
		 * Tests the pose error determination function for separate error values with any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeterminePoseErrorSeparateAnyCamera(const double testDuration);

		/**
		 * Tests the pose error determination function for combined error values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeterminePoseErrorCombinedPinhole(const double testDuration);

		/**
		 * Tests the pose error determination function for combined error values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeterminePoseErrorCombinedAnyCamera(const double testDuration);

		/**
		 * Tests the homography error determination function for separate error values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetermineHomographyErrorSeparate(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_ERROR_H
