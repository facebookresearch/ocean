/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_PLANE_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_PLANE_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/Estimator.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class tests the non linear optimization plane implementations.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationPlane
{
	public:

		/**
		 * Tests the non linear optimization plane function.
		 * @param testDuration Number of seconds for each test
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker* worker);

	private:

		/**
		 * Tests the non linear optimization function for a plane.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testOptimizePlane(const double testDuration);

		/**
		 * Tests the non linear optimization function for two cameras capturing 3D object points lying on a plane.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOptimizeOnePoseOnePlane(const double testDuration);

		/**
		 * Tests the non linear optimization function for several cameras capturing 3D object points lying on a plane.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOptimizePosesOnePlane(const double testDuration);

		/**
		 * Tests the non linear optimization function for a 3D plane with ideal 3D object points.
		 * @param numberPoints Number of image points to be used
		 * @param testDuration Number of seconds for each test
		 * @param type Estimator type to be used
		 * @param outliers Number of outlier correspondences
		 * @return True, if succeeded
		 */
		static bool testOptimizePlaneIdeal(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const unsigned int outliers = 0);

		/**
		 * Tests the non linear optimization function for a 3D plane with noisy 3D object points in combination with a RANSAC algorithm.
		 * @param numberPoints Number of image points to be used
		 * @param testDuration Number of seconds for each test
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points, with range [0, infinity)
		 * @param outliers Number of outlier correspondences
		 * @return True, if succeeded
		 */
		static bool testOptimizePlaneNoisy(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0);

		/**
		 * Tests the non linear optimization function for two cameras capturing 3D object points lying on a plane.
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @return True, if succeeded
		 */
		static bool testOptimizeOnePoseOnePlane(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type);

		/**
		 * Tests the non linear optimization function for several cameras capturing 3D object points lying on a plane.
		 * @param numberPoses Number of poses to be used
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @return True, if succeeded
		 */
		static bool testOptimizePosesOnePlane(const unsigned int numberPoses, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_PLANE_H
