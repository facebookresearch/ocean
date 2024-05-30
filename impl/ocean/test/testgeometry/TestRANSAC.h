/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the tests for RANSAC functions.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestRANSAC
{
	protected:

		/**
		 * Definition of individual camera distortion types.
		 */
		enum DistortionType : uint32_t
		{
			/// No distortion.
			DT_NO_DISTORTION = 0u,
			/// The camera has radial distortion.
			DT_RADIAL_DISTORTION = 1u << 0u | DT_NO_DISTORTION,
			/// The camera has full distortion (e.g., radial and tangential).
			DT_FULL_DISTORTION = (1u << 1u) | DT_RADIAL_DISTORTION
		};

	public:

		/**
		 * Invokes all RANSAC tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Optional worker object
		 * @return True, if the entire function test has succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the iterations function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIterations(const double testDuration);

		/**
		 * Tests the perspective pose function p3p for mono cameras.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3P(const double testDuration);

		/**
		 * Tests the perspective pose function p3p for mono cameras.
		 * @param anyCameraType The camera type to be tested
		 * @param correspondences The number of correspondences to use, with range [4, infinity)
		 * @param faultyRate The rate of invalid correspondences, with range [0, 1)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3P(const AnyCameraType anyCameraType, const size_t correspondences, const double faultyRate, const double testDuration);

		/**
		 * Tests the RANSAC implementation of the perspective pose problem for three random points including unknown zoom factor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3PZoom(const double testDuration);

		/**
		 * Tests the determination of a 6-DOF object transformation with any stereo camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testObjectTransformationStereoAnyCamera(const double testDuration);

		/**
		 * Tests the RANSAC-based function determining the homography matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrix(const double testDuration, Worker& worker);

		/**
		 * Tests the RANSAC-based function determining the homography matrix with a specified number of point correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param refine True, to apply an successive non-linear optimization step; False, to keep the internal RANSAC result
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrix(const double testDuration, const bool refine, const bool useSVD, Worker& worker);

		/**
		 * Tests the RANSAC-based function determining the homography matrix for non-bijective correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixForNonBijectiveCorrespondences(const double testDuration, Worker& worker);

		/**
		 * Tests the RANSAC-based function determining the homography matrix with a specified number of non-bijective point correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param refine True, to apply an successive non-linear optimization step; False, to keep the internal RANSAC result
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinear)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testHomographyMatrixForNonBijectiveCorrespondences(const double testDuration, const bool refine, const bool useSVD, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_RANSAC_H
