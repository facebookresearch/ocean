/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_OBJECT_POINT_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_OBJECT_POINT_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/PinholeCamera.h"

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
class OCEAN_TEST_GEOMETRY_EXPORT TestNonLinearOptimizationObjectPoint : protected Geometry::NonLinearOptimizationObjectPoint
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
		 * Tests the non linear optimization function for several 3D object points and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointsPinholeCamera(const double testDuration, Worker* worker);

		/**
		 * Tests the non linear optimization function for object points with a specific camera and a specified number of random point correspondences.
		 * @param patternCamera The camera profile pattern which will be used for the test
		 * @param poses Number of camera poses
		 * @param numberObjectPoints Number of object point points
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointsPinholeCamera(const PinholeCamera& patternCamera, const unsigned int poses, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0u, Worker* worker = nullptr);

		/**
		 * Tests the non linear optimization function for one 3D object point and a fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointFisheyeCamera(const double testDuration);

		/**
		 * Tests the non linear optimization function for one object point with a specific a specific number of random point correspondences.
		 * @param poses Number of camera poses
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointFisheyeCamera(const unsigned int poses, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0u);

		/**
		 * Tests the non linear optimization function for one 3D object point and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointAnyCamera(const double testDuration);

		/**
		 * Tests the non linear optimization function for one object point with a specific a specific number of random point correspondences.
		 * @param anyCamera The camera profile to be used, must be valid
		 * @param poses Number of camera poses, with range [2, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences, with range [0, poses - 1]
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointAnyCamera(const AnyCamera& anyCamera, const unsigned int poses, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0u);

		/**
		 * Tests the non linear optimization function for one 3D object point and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointStereoAnyCamera(const double testDuration);

		/**
		 * Tests the non linear optimization function for one object point with a specific a specific number of random point correspondences.
		 * @param anyCameraA The first camera profile to be used, must be valid
		 * @param anyCameraB The second camera profile to be used, must be valid
		 * @param poses Number of camera poses, with range [2, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences, with range [0, poses - 1]
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointStereoAnyCamera(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const unsigned int poses, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0u);

		/**
		 * Tests the non linear optimization function for one 3D object point and any cameras.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointAnyCameras(const double testDuration);

		/**
		 * Tests the non linear optimization function for one object point with a specific a specific number of random point correspondences.
		 * @param poses Number of camera poses, with range [2, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences, with range [0, poses - 1]
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointAnyCameras(const unsigned int poses, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0u);


		/**
		 * Tests the non linear optimization function for one 3D object point and a stereo fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointStereoFisheyeCamera(const double testDuration);

		/**
		 * Tests the non linear optimization function for one object point with a specific a specific number of random point correspondences.
		 * @param poses Number of camera poses
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationObjectPointStereoFisheyeCamera(const unsigned int poses, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0u);

		/**
		 * Tests the non linear optimization function for one static and one dynamic 6DOF poses and several dynamic 3D object points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationOnePoseObjectPoints(const double testDuration);

		/**
		 * Tests the non linear optimization function for one static and one dynamic camera pose and several dynamic object points.
		 * @param pinholeCamera The pinhole camera to test
		 * @param numberObjectPoints Number of object points
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences
		 * @param useCovariances True, to create and use perfect covariance matrices
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationOnePoseObjectPoints(const PinholeCamera& pinholeCamera, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0, const bool useCovariances = false);

		/**
		 * Tests the non linear optimization function for one static and one dynamic 6DOF poses and several dynamic 3D object points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationTwoPosesObjectPoints(const double testDuration);

		/**
		 * Tests the non linear optimization function for two dynamic camera pose and several dynamic object points.
		 * @param pinholeCamera The pinhole camera to test
		 * @param numberObjectPoints Number of object points
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points
		 * @param outliers Number of outliers correspondences
		 * @param useCovariances True, to create and use perfect covariance matrices
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationTwoPosesObjectPoints(const PinholeCamera& pinholeCamera, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation = 0, const unsigned int outliers = 0, const bool useCovariances = false);

		/**
		 * Tests the non linear optimization function for several 6DOF poses and several 3D object points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPosesObjectPoints(const double testDuration);

		/**
		 * Tests the non linear optimization function for poses, and object points, and a specified number of random point correspondences.
		 * @param numberPoses The number of camera poses, with range [2, infinity)
		 * @param numberObjectPoints Number of object points, with range [4, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type The estimator type to be used
		 * @param noiseStandardDeviation The standard deviation of the Gaussian normal distribution for noised image points, with range [0, infinity)
		 * @param numberOutliers The number of outliers correspondences, with range [0, numberObjectPoints]
		 * @param slowImplementation True, to test the slow implementation; False, to test the fast (highly optimized) implementation
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationPosesObjectPoints(const unsigned int numberPoses, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar noiseStandardDeviation = Scalar(0), const unsigned int numberOutliers = 0u, const bool slowImplementation = false);

		/**
		 * Tests the non linear optimization function for several 6DOF poses (with fixed translations) and several 3D object points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationOrientationalPosesObjectPoints(const double testDuration);

		/**
		 * Tests the non linear optimization function for poses (with fixed translations), and object points, and a specified number of random point correspondences.
		 * @param numberPoses The number of camera poses, with range [2, infinity)
		 * @param numberObjectPoints Number of object points, with range [4, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type The estimator type to be used
		 * @param noiseStandardDeviation The standard deviation of the Gaussian normal distribution for noised image points, with range [0, infinity)
		 * @param numberOutliers The number of outliers correspondences, with range [0, numberObjectPoints]
		 * @return True, if succeeded
		 */
		static bool testNonLinearOptimizationOrientationalPosesObjectPoints(const unsigned int numberPoses, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar noiseStandardDeviation = Scalar(0), const unsigned int numberOutliers = 0u);

		/**
		 * Tests the non linear optimization function for a 3D object point observed in several cameras with rotational motion.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testOptimizeObjectPointRotationalPoses(const double testDuration);

		/**
		 * Tests the non linear optimization function for a 3D object point observed in several cameras with rotational motion with noisy image points optional in combination with a RANSAC algorithm.
		 * @param patternCamera The camera profile pattern which will be used for the test
		 * @param numberObservations Number of observations to be used, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param type Estimator type to be used
		 * @param standardDeviation Optional standard deviation of the Gaussian normal distribution for noised image points, with range [0, infinity)
		 * @param outliers Number of outliers correspondences
		 * @param useRoughObjectPoint True, to use a rough object point as start for the optimization; False, to use a RANSAC approach for a initial object point
		 * @return True, if succeeded
		 */
		static bool testOptimizeObjectPointRotationalPoses(const PinholeCamera& patternCamera, const unsigned int numberObservations, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool useRoughObjectPoint);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_NON_LINEAR_OPTIMIZATION_OBJECT_POINT_H
