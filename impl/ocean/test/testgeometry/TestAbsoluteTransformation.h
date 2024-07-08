/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_ABSOLUTE_TRANSFORMATION_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_ABSOLUTE_TRANSFORMATION_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements an absolute transformation test.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestAbsoluteTransformation
{
	public:

		/**
		 * Tests the absolute transformation function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the absolute transformation function which is based on 3D points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAbsoluteTransformationBasedOnPoints(const double testDuration);

		/**
		 * Tests the absolute transformation function which is based on 6-DOF transformations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAbsoluteTransformationBasedOnTransformations(const double testDuration);

		/**
		 * Tests the absolute transformation function which is based on 6-DOF transformations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAbsoluteTransformationBasedOnTransformationsWithOutliers(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_ABSOLUTE_TRANSFORMATION_H
