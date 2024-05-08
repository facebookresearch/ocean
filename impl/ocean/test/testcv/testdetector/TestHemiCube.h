/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HEMI_CUBE_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HEMI_CUBE_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/FiniteLine2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements tests for the Hemi cube.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestHemiCube
{
	public:
		/**
		 * Invokes all test for the Hemi cube.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the add lines to the Hemi cube.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAdd(const double testDuration);

		/**
		 * Test merging of two line segments into a new line segment
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLineFusion(const double testDuration);

		/**
		 * Test merging of two line segments into a new line segment
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMergeGreedyBruteForce(const double testDuration);

		/**
		 * Test merging of two line segments into a new line segment
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMerge(const double testDuration);

	protected:

		/**
		 * Generate a random 2D line segment inside the boundaries of an image/rectangle.
		 * @param randomGenerator The random generator to be used for the generation of the test data
		 * @param imageWidth Width of the image
		 * @param imageHeight Height of the image
		 * @return A line segment within the image boundaries
		 */
		static FiniteLine2 generateRandomFiniteLine2(RandomGenerator& randomGenerator, const unsigned int imageWidth, const unsigned int imageHeight);

		/**
		 * Generate a random 2D line segment inside the boundaries of an image/rectangle that is orthogonal to a reference line segment.
		 * @param randomGenerator The random generator to be used for the generation of the test data
		 * @param line Line for which a orthogonal line will be generated randomly
		 * @param imageWidth Width of the image
		 * @param imageHeight Height of the image
		 * @param minLineLength Minimum length of the random line segments that are generated, default: 0.5, range: (0, infinity)
		 * @return A line segment within the image boundaries
		 */
		static FiniteLine2 generateRandomOrthogonalFiniteLine2(RandomGenerator& randomGenerator, const FiniteLine2& line, const unsigned int imageWidth, const unsigned int imageHeight, const Scalar minLineLength = Scalar(0.5));

		/**
		 * Generate a random 2D line segment inside the boundaries of an image/rectangle that is collinear with a reference line segment.
		 * @param randomGenerator The random generator to be used for the generation of the test data
		 * @param line Line for which a collinear line will be generated randomly
		 * @param imageWidth Width of the image
		 * @param imageHeight Height of the image
		 * @param minLineLength Minimum length of the random line segments that are generated, default: 0.5, range: (0, infinity)
		 * @return A line segment within the image boundaries
		 */
		static FiniteLine2 generateRandomCollinearFiniteLine2(RandomGenerator& randomGenerator, const FiniteLine2& line, const unsigned int imageWidth, const unsigned int imageHeight, const Scalar minLineLength = Scalar(0.5));

		/**
		 * Validation for line merging function
		 * @param testLine Result of merging parameter `lines` with `HemiCube::fuse()`.
		 * @param lines Lines to be merged.
		 * @return True if the result of this function is sufficiently close to `testLine`, otherwise false.
		 */
		static bool validateLineFusion(const FiniteLine2& testLine, const FiniteLines2& lines);
};

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HEMI_CUBE_H
