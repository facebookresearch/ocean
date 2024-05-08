/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_CAMERA_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_CAMERA_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/NonLinearOptimizationCamera.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class tests the non linear optimization implementations for camera profiles.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationCamera
{
	public:

		/**
		 * Tests the non linear optimization function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker* worker);

	private:

		/**
		 * Tests the non linear optimization function for camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationCamera(const double testDuration);

		/**
		 * Tests the non linear optimization function for one camera profile and several poses.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationCameraPoses(const double testDuration);

		/**
		 * Tests the non linear optimization function for camera parameters with a defined number of correspondences.
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian distribution for noised image points
		 * @param outliers Number of outlier correspondences
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationCamera(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers);

		/**
		 * Tests the non linear optimization function with one camera profile and a specified number of poses.
		 * @param poses Number of poses to be used
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationCameraPoses(const unsigned int poses, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_H
