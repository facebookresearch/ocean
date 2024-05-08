/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_PNP_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_PNP_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements a PnP test.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestPnP
{
	public:

		/**
		 * Invokes the tests of for the perspective pose problem (with at least five points).
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the perspective pose problem for at least five random points.
		 * @param testDuration Number of seconds for each test with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPose(const double testDuration);

		/**
		 * Tests the perspective pose problem for at least five random points.
		 * @param numberPoints Number of points used for pose determination, with range [5, infinity)
		 * @param testDuration Number of seconds for each test with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPose(const unsigned int numberPoints, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_PNP_H
