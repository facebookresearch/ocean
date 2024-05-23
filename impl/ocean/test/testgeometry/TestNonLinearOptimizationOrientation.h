/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_ORIENTATION_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_ORIENTATION_H

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
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationOrientation
{
	public:

		/**
		 * Tests the non linear optimization orientation function.
		 * @param testDuration Number of seconds for each test
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker* worker);

		/**
		 * Tests the non linear optimization function for a 3DOF orientation.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testOptimizeOrientation(const double testDuration);

		/**
		 * Tests the non linear optimization function for a 3DOF orientation with noisy image points in combination with a RANSAC algorithm.
		 * @param numberPoints Number of image points to be used
		 * @param testDuration Number of seconds for each test
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points, with range [0, infinity)
		 * @param numberOutliers Number of outlier correspondences, with range [0, numberPoints]
		 * @param useRoughOrientation True, to use a rough orientation as start for the optimization; False, to use a RANSAC approach for a initial orientation
		 * @return True, if succeeded
		 */
		static bool testOptimizeOrientation(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useRoughOrientation);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_ORIENTATION_H
