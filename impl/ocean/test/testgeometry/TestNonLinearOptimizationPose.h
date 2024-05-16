/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_POSE_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_POSE_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the tests of the non linear optimization functions for camera poses.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationPose
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
		 * Deprecated.
		 *
		 * Tests the non linear optimization function for a 6DOF pose and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPosePinholeCamera(const double testDuration);

		/**
		 * Deprecated.
		 *
		 * Tests the non linear optimizations function for a 6DOF pose and a pinhole camera, with specified correspondences, estimator type and outliers.
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian distribution for noised image points
		 * @param outliers Number of outlier correspondences
		 * @param useCovariances True, to use covariances
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPosePinholeCamera(const PinholeCamera& pinholeCamera, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool useCovariances);

		/**
		 * Tests the non linear optimization function for a 6DOF pose and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPoseAnyCamera(const double testDuration);

		/**
		 * Tests the non linear optimizations function for a 6DOF pose and any camera, with specified correspondences, estimator type and outliers.
		 * @param anyCamera The camera profile to be used, must be valid
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian distribution for noised image points
		 * @param outliers Number of outlier correspondences
		 * @param useCovariances True, to use covariances
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPoseAnyCamera(const AnyCamera& anyCamera, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool useCovariances);

		/**
		 * Tests the non linear optimization function for a 6DOF pose with flexible zoom factor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPoseZoom(const double testDuration);

		/**
		 * Tests the non linear optimizations function for a 6DOF pose with flexible zoom factor, with specified correspondences, estimator type and outliers.
		 * @param pinholeCamera The pinhole camera to be used
		 * @param correspondences Number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian distribution for noised image points
		 * @param outliers Number of outlier correspondences
		 * @param useCovariances True, to use covariances
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPoseZoom(const PinholeCamera& pinholeCamera, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool useCovariances);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_POSE_H
