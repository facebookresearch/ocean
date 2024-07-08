/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_STEREOSCOPIC_GEOMETRY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_STEREOSCOPIC_GEOMETRY_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements a test for functions in StereoscopicGeometry.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestStereoscopicGeometry
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the function to determine the transformation between two cameras.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCameraPose(const double testDuration);

		/**
		 * Tests the function to determine the transformation between two cameras.
		 * @param numberCorrespondences The number of 2D image point correspondences to be used during the test, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tPureRotational True, to test the pose estimation with only a pure rotational camera motion; False, to test the full 6-DOF pose estimation
		 */
		template <bool tPureRotation>
		static bool testCameraPose(const unsigned int numberCorrespondences, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_STEREOSCOPIC_GEOMETRY_H
