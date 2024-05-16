/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the tests for RANSAC functions.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestRANSAC
{
	public:

		/**
		 * Invokes all RANSAC tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker* worker);

		/**
		 * Tests the perspective pose function p3p for mono cameras.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3P(const double testDuration);

		/**
		 * Tests the perspective pose function p3p for mono cameras.
		 * @param anyCameraType The camera type to be tested
		 * @param correspondences The number of correspondences to use, with range [4, infinity)
		 * @param faultyRate The rate of invalid correspondences, with range [0, 1)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3P(const AnyCameraType anyCameraType, const size_t correspondences, const double faultyRate, const double testDuration);

		/**
		 * Tests the determination of a 6-DOF object transformation with any stereo camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testObjectTransformationStereoAnyCamera(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H
