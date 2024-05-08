/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_EPIPOLAR_GEOMETRY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_EPIPOLAR_GEOMETRY_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include <set>

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements an epipolar geometry test.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestEpipolarGeometry
{
	public:

		/**
		 * Tests all epipolar geometry functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the calculation of the fundamental matrix.
		 * All point coordinates are faultless.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultlessFundamentalMatrix(const double testDuration);

		/**
		 * Tests the calculation of the fundamental matrix.
		 * All point coordinates are faultless but have an Gaussian noise.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultlessNoisedFundamentalMatrix(const double testDuration);

		/**
		 * Tests the calculation of the fundamental matrix.
		 * Some point coordinates will be faulty.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultyFundamentalMatrix(const double testDuration);

		/**
		 * Tests the triangulation of image points
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTriangulateImagePoints(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_EPIPOLAR_GEOMETRY_H
