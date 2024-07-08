/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_TRANSFORMATION_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_TRANSFORMATION_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include <opencv2/core.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class tests transformation determination functions of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestTransformation
{
	public:

		/**
		 * Tests all transformation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTransformation(const double testDuration);

	private:

		/**
		 * Tests the function determining the similarity matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testSimilarityMatrix(const double testDuration);

		/**
		 * Tests the function determining the similarity matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [3, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar used for the test
		 */
		template <typename T>
		static bool testSimilarityMatrix(const double testDuration, const size_t points);

		/**
		 * Tests the function determining the affine matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testAffineMatrix(const double testDuration);

		/**
		 * Tests the function determining the affine matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [3, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar used for the test
		 */
		template <typename T>
		static bool testAffineMatrix(const double testDuration, const size_t points);

		/**
		 * Tests the function determining the similarity matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testHomographyMatrix(const double testDuration);

		/**
		 * Tests the function determining the similarity matrix with a specified number of point correspondences
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param points The number of point correspondences to be used, with range [3, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar used for the test
		 */
		template <typename T>
		static bool testHomographyMatrix(const double testDuration, const size_t points);

		/**
		 * Converts 2D vectors defined by the Ocean framework to 2D vectors defined by OpenCV.
		 * @param vectors The vectors to convert
		 * @return The resulting 2D vectors
		 * @tparam T The data type of each element the resulting 2D vectors
		 */
		template <typename T>
		static std::vector<cv::Point_<T>> ocean2opencv(const Vectors2& vectors);

		/**
		 * Converts a 3x3 square matrix defined by OpenCV to a 3x3 square matrix defined by Ocean.
		 * @param matrix The matrix to convert
		 * @return The resulting matrix
		 * @tparam T The data type of each element the resulting matrix
		 */
		template <typename T>
		static SquareMatrix3 opencv2ocean(const cv::Mat& matrix);

		/**
		 * Converts a 3x3 square matrix defined by OpenCV to a 3x3 square matrix defined by Ocean in homography.
		 * @param matrix The matrix to convert
		 * @return The resulting matrix
		 * @tparam T The data type of each element the resulting matrix
		 */
		template <typename T>
		static SquareMatrix3 opencv2ocean_homography(const cv::Mat& matrix);

		/**
		 * Converts a 3x3 square matrix defined by OpenCV to a 3x3 square matrix defined by Ocean in affine transform.
		 * @param matrix The matrix to convert
		 * @return The resulting matrix
		 * @tparam T The data type of each element the resulting matrix
		 */
		template <typename T>
		static SquareMatrix3 opencv2ocean_affine(const cv::Mat& matrix);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_TRANSFORMATION_H
