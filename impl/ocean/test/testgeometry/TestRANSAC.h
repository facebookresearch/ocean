// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H

#include "ocean/test/testgeometry/TestGeometry.h"

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
		 * Tests the determination of a 6-DOF object transformation with any stereo camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testObjectTransformationStereoAnyCamera(const double testDuration);

		/**
		 * Tests the determination of a 6-DOF object transformation with stereo fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testObjectTransformationStereo(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H
