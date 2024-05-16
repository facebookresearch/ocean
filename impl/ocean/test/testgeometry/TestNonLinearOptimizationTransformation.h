/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_TRANSFORMATION_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_TRANSFORMATION_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the tests of the non linear optimization functions for transformations.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationTransformation
{
	public:

		/**
		 * Tests the non linear optimization function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker* worker);

		/**
		 * Tests the non linear optimization function for a 6-DOF object transformation with fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectTransformation(const double testDuration);

		/**
		 * Tests the non linear optimizations function for a 6-DOF transformation with fisheye camera, with specified poses and correspondences.
		 * @param camera The camera model to be used, must be valid
		 * @param poses The number of camera poses, with range [2, infinity)
		 * @param correspondences The number of point correspondences, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectTransformation(const AnyCamera& camera, const unsigned int poses, const unsigned int correspondences, const double testDuration);

		/**
		 * Tests the non linear optimization function for a 6-DOF object transformation with stereo fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectTransformationStereo(const double testDuration);

		/**
		 * Tests the non linear optimizations function for a 6-DOF transformation with stereo fisheye camera, with specified poses and correspondences.
		 * @param cameraA The first camera model to be used, must be valid
		 * @param cameraB The second camera model to be used, must be valid
		 * @param poses The number of camera poses, with range [2, infinity)
		 * @param correspondences The number of point correspondences, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectTransformationStereo(const AnyCamera& cameraA, const AnyCamera& cameraB, const unsigned int poses, const unsigned int correspondences, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_TRANSFORMATION_H
