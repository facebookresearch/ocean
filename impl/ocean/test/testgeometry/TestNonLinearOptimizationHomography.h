/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_HOMOGRAPHY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_HOMOGRAPHY_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/Estimator.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the tests of the non linear optimization functions for homography optimization.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationHomography
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
		 * Tests the non linear optimization function optimizing a homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationHomography(const double testDuration);

		/**
		 * Tests the non linear optimization function optimizing a similarity transformation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationSimilarity(const double testDuration);

		/**
		 * Tests the non linear optimizations function optimizing a homography with specified correspondences, estimator type and outliers.
		 * @param correspondences Number of point correspondences
		 * @param modelParameter The number of parameters defining the model (of the homography) to be optimize, with range [8, 9]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian distribution for noised image points
		 * @param outliers Number of outlier correspondences
		 * @param useCovariances True, to use covariances
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationHomography(const unsigned int correspondences, const unsigned int modelParameter, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool useCovariances);

		/**
		 * Tests the non linear optimizations function optimizing a similarity transformation with specified correspondences, estimator type and outliers.
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian distribution for noised image points
		 * @param outliers Number of outlier correspondences
		 * @param useCovariances True, to use covariances
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationSimilarity(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool useCovariances);

	protected:

		/**
		 * Determines the average pixel error between set of left points transform to the right points.
		 * @param homography The homography that will be checked transforming left points to right points (right = homography * left), must be valid
		 * @param pointsLeft Image points defined in the left camera frame
		 * @param pointsRight Image points defined int he right camerea frame, each point corresponds the one point in the left camera frame
		 * @param maximalSqrDistance Optional resulting maximal square distance between a right image point and the corresponding transformed left image point
		 * @return The average square distance between the right image points and the transformed left image points in pixel, with range [0, infinity)
		 */
		static Scalar determineHomographyError(const SquareMatrix3& homography, const Vectors2& pointsLeft, const Vectors2& pointsRight, Scalar* maximalSqrDistance = nullptr);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_HOMOGRAPHY_H
