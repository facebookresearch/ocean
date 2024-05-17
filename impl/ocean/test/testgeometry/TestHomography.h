/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_HOMOGRAPHY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_HOMOGRAPHY_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements homography tests.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestHomography
{
	public:

		/**
		 * Tests all homography functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the rotational homography for one pose.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRotationalHomographyOnePose(const double testDuration);

		/**
		 * Tests the rotational homography for two poses.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRotationalHomographyTwoPoses(const double testDuration);

		/**
		 * Tests the planar homography with one idenity camera pose and one variable camera pose.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPlanarHomographyOnePose(const double testDuration);

		/**
		 * Tests the planar homography with two variable camera poses.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPlanarHomographyTwoPoses(const double testDuration);

		/**
		 * Tests the factorization of the planar homography with one idenity camera pose and one variable camera pose.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFactorizationPlanarHomographyOnePose(const double testDuration);

		/**
		 * Tests the factorization of the planar homography with two variable camera poses.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFactorizationPlanarHomographyTwoPoses(const double testDuration);

		/**
		 * Tests the 3D planar 2D-2D homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultlessPlanarHomography2D(const double testDuration);

		/**
		 * Tests the 3D planar 2D-2D homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultlessNoisedPlanarHomography2D(const double testDuration);

		/**
		 * Tests homography determination.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultlessHomography(const double testDuration);

		/**
		 * Tests homography determination.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFaultlessNoisedHomography(const double testDuration);

		/**
		 * Tests intrinsic camera matrix determination.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntrinsic(const double testDuration);

		/**
		 * Tests the function determining the homothetic matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHomotheticMatrix(const double testDuration);

		/**
		 * Tests the function determining the homothetic matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [3, infinity)
		 * @return True, if succeeded
		 */
		static bool testHomotheticMatrix(const double testDuration, const size_t points);

		/**
		 * Tests the function determining the similarity matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSimilarityMatrix(const double testDuration);

		/**
		 * Tests the function determining the similarity matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [3, infinity)
		 * @return True, if succeeded
		 */
		static bool testSimilarityMatrix(const double testDuration, const size_t points);

		/**
		 * Tests the function determining the affine matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAffineMatrix(const double testDuration);

		/**
		 * Tests the function determining the affine matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [3, infinity)
		 * @return True, if succeeded
		 */
		static bool testAffineMatrix(const double testDuration, const size_t points);

		/**
		 * Tests the function determining the homography matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrix(const double testDuration, const bool useSVD);

		/**
		 * Tests the function determining the homography matrix based on points and lines.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixFromPointsAndLinesSVD(const double testDuration);

		/**
		 * Tests the function determining the homography matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [4, infinity)
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrix(const double testDuration, const size_t points, const bool useSVD);

		/**
		 * Tests the function determining the homography matrix based on points and lines.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param correspondences The number of point or line correspondences to be used, with range [4, infinity)
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixFromPointsAndLinesSVD(const double testDuration, const size_t correspondences);

		/**
		 * Tests the RANSAC-based function determining the homography matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param refine True, to apply an successive non-linear optimization step; False, to keep the internal RANSAC result
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixRANSAC(const double testDuration, const bool refine, const bool useSVD, Worker& worker);

		/**
		 * Tests the RANSAC-based function determining the homography matrix with a specified number of point correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [4, infinity)
		 * @param refine True, to apply an successive non-linear optimization step; False, to keep the internal RANSAC result
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixRANSAC(const double testDuration, const size_t points, const bool refine, const bool useSVD, Worker& worker);

		/**
		 * Tests the RANSAC-based function determining the homography matrix for non-bijective correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param refine True, to apply an successive non-linear optimization step; False, to keep the internal RANSAC result
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixRANSACForNonBijectiveCorrespondences(const double testDuration, const bool refine, const bool useSVD, Worker& worker);

		/**
		 * Tests the RANSAC-based function determining the homography matrix with a specified number of non-bijective point correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [4, infinity)
		 * @param refine True, to apply an successive non-linear optimization step; False, to keep the internal RANSAC result
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixRANSACForNonBijectiveCorrespondences(const double testDuration, const size_t points, const bool refine, const bool useSVD, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_HOMOGRAPHY_H
