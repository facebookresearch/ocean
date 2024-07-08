/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_DELAUNAY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_DELAUNAY_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements a test for the Delaunay triangulation.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestDelaunay
{
	public:

		/**
		 * Tests the Delaunay triangulation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the delaunay triangulation of random points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTriangulation(const double testDuration);

	protected:

		/**
		 * Tests the delaunay triangulation for a specified number of random points.
		 * @param pointNumber The number of points to be used for triangulation, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTriangulation(const unsigned int pointNumber, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_DELAUNAY_H
