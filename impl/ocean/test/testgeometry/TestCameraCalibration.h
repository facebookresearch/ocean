/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_CAMERA_CALIBRATION_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_CAMERA_CALIBRATION_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements camera calibration test functions.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestCameraCalibration
{
	public:

		/**
		 * Tests the camera calibration functionality.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCameraCalibration(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_CAMERA_CALIBRATION_H
