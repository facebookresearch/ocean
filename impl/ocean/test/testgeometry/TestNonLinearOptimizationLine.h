/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_LINE_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_LINE_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/Estimator.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class tests the non linear optimization implementations for lines.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationLine
{
	public:

		/**
		 * Tests the non linear optimization function.
		 * @param testDuration Number of seconds for each test
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker* worker);

	private:

		/**
		 * Tests the non linear optimization function for one 2D line.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationLine(const double testDuration);

		/**
		 * Tests the non linear optimization function for a 2D line with ideal 2D image points.
		 * @param numberPoints Number of image points to be used
		 * @param testDuration Number of seconds for each test
		 * @param type Estimator type to be used
		 * @param outliers Number of outlier correspondences
		 * @return True, if succeeded
		 */
		static bool testOptimizeLineIdeal(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const unsigned int outliers = 0);

		/**
		 * Tests the non linear optimization function for a 2D line with noisy 2D image points.
		 * @param numberPoints Number of image points to be used
		 * @param testDuration Number of seconds for each test
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points, with range [0, infinity)
		 * @param outliers Number of outlier correspondences
		 * @return True, if succeeded
		 */
		static bool testOptimizeLineNoisy(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_H
