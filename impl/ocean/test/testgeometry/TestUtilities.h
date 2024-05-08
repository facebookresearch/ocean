/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_UTITLITIES_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_UTITLITIES_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements tests for the functions in @c Geometry::Utilities.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestUtilities
{
	public:

		/**
		 * Tests for the functions in @c Geometry::Utilities.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True on success, otherwise false
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the create 3D object points function.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True on success, otherwise false
		 */
		static bool testCreateObjectPoints(const double testDuration);

		/**
		 * Test for the computation of the area of polygons
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True on success, otherwise false
		 */
		static bool testComputePolygonArea(const double testDuration);

		/**
		 * Test for the check if points are inside or outside of a polygon
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True on success, otherwise false
		 */
		static bool testIsInsideConvexPolygon(const double testDuration);

		/**
		 * Tests the computation of a random camera pose for a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomCameraPosePinhole(const double testDuration);

		/**
		 * Tests the computation of a random camera pose for a fisheye camera.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomCameraPoseFisheye(const double testDuration);
};

} // namespace TestGeometry

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_UTITLITIES_H
