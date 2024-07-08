/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_MULTIPLE_VIEW_GEOMETRY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_MULTIPLE_VIEW_GEOMETRY_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements an epipolar geometry test.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestMultipleViewGeometry
{
	public:

		/**
		 * Tests all multiple view geometry functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMultipleViewGeometry(const double testDuration);

	private:

		/**
		 * Tests the calculation of the trifocal tensor matrices.
		 * @param addGaussianNoise Gaussian noise will be added
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTrifocalTensorMatrix(bool addGaussianNoise, const double testDuration);

		/**
		 * Tests the 3-views case of projective reconstruction algorithm.
		 * @param addGaussianNoise Gaussian noise will be added
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testProjectiveReconstructionFrom3Views(bool addGaussianNoise, const double testDuration);

		/**
		 * Tests the multiple views case of projective reconstruction algorithm.
		 * @param views Number of views, with range [3, infinity)
		 * @param addGaussianNoise If set, Gaussian noise is added
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testProjectiveReconstruction(const unsigned int views, bool addGaussianNoise, const double testDuration);

		/**
		 * Tests the multiple views case of projective reconstruction algorithm.
		 * Faulty image point correspondences are used.
		 * @param views Number of views, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultyProjectiveReconstruction(const unsigned int views, const double testDuration);

		/**
		 * Generate groups of corresponding image points for individual camera poses.
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param points Number of point correspondences, with range [1, infinity)
		 * @param views Number of views, with range [2, infinity)
		 * @param imagePointsPerPose Resulting groups of corresponding image points, one group for each camera pose
		 * @param gaussSigma Additional Gaussian noise will be added [0, infinity), 0 disable noise
		 * @param objectPoints Optional resulting 3d object points
		 * @return True, if succeeded
		 */
		static bool generatedImagePointGroups(const PinholeCamera& pinholeCamera, const size_t points, const unsigned int views, std::vector<Vectors2>& imagePointsPerPose, Scalar gaussSigma = Scalar(0), Vectors3* objectPoints = nullptr);

		/**
		 * Evaluates the (squared) re-projection error for given camera intrinsics and camera poses.<br>
		 * Therefore, 3d objects point are determined from camera poses and then projected to image plane.
		 * @param imagePointsPerPose Group of corresponding image points, one group for each camera pose
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param posesIF The camera poses (inverted flipped)
		 * @return The squared re-projection error between image points given and estimated image points.
		 */
		static Scalar evaluateReprojectionError(const std::vector<Vectors2>& imagePointsPerPose, const PinholeCamera& pinholeCamera, const HomogenousMatrices4& posesIF);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_MULTIPLE_VIEW_GEOMETRY_H
