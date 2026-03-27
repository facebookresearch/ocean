/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_NON_MAXIMUM_SUPPRESSION_H
#define META_OCEAN_TEST_TESTCV_TEST_NON_MAXIMUM_SUPPRESSION_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/test/TestSelector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/NonMaximumSuppression.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class tests the implementation of the NonMaximumSuppression class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestNonMaximumSuppression
{
	protected:

		/**
		 * Definition of a location combining a strength parameter.
		 */
		using StrengthPosition = CV::NonMaximumSuppressionT<uint8_t>::StrengthPosition<int, uint8_t>;

		/**
		 * Definition of a vector holding locations.
		 */
		using StrengthPositions = std::vector<StrengthPosition>;

		/**
		 * Definition of a set holding locations.
		 */
		using StrengthPositionSet = std::set<StrengthPosition>;

	public:

		/**
		 * Tests the entire functionality.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param selector Test selector for filtering sub-tests; default runs all tests
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector = TestSelector());

		/**
		 * Tests the non maximum suppression within a frame.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param subFrameWidth The width of the actual area of application, with range [2, width]
		 * @param subFrameHeight The height of the actual area of application, with range [2, height]
		 * @param strictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testSuppressionInFrame(const unsigned int width, const unsigned int height, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const bool strictMaximum, const double testDuration, Worker& worker);

		/**
		 * Tests the non minimum suppression within a frame (finding local minima).
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param strictMaximum True, to search for a strict minimum (smaller than all eight neighbors); False, to allow equal values in the lower right neighborhood
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testSuppressionInFrameMinimum(const unsigned int width, const unsigned int height, const bool strictMaximum, const double testDuration, Worker& worker);

		/**
		 * Tests the non maximum suppression within a dataset of strength positions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSuppressionInStrengthPositions(const double testDuration);

		/**
		 * Tests the non maximum suppression within a dataset of strength positions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 */
		template <typename TCoordinate, typename TStrength>
		static bool testSuppressionInStrengthPositions(const double testDuration);

		/**
		 * Tests the 1D precise peak location function.
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testDeterminePrecisePeakLocation1();

		/**
		 * Tests the 2D precise peak location function.
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testDeterminePrecisePeakLocation2();

		/**
		 * Tests the determinePrecisePeakLocationNxN function.
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testDeterminePrecisePeakLocationNxN();

		/**
		 * Tests the iterative precise peak location function.
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testDeterminePrecisePeakLocationIterativeNxN();

		/**
		 * Tests the candidate lookup function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCandidate(const double testDuration);

	protected:

		/**
		 * Creates a test frame with artificial feature points.
		 * @param yFrame The frame to which the feature points will be added, must have pixel format FORMAT_Y8, must be valid
		 * @param features The number of feature points to create, with range [0, infinity)
		 * @param featurePointStrength The strength of the feature points to create, with range [1, 255]
		 */
		static void createFeaturePoints(Frame& yFrame, const unsigned int features, const uint8_t featurePointStrength = 255u);

		/**
		 * Determines the locations of the extrema by a standard implementation.
		 * @param yFrame The frame providing the feature points, with pixel format FORMAT_Y8, must be valid
		 * @param subRegionLeft The left location of the upper left corner of the sub-region in which the points will be determined, with range [0, width - 1]
		 * @param subRegionTop The top location of the upper left corner of the sub-region in which the points will be determined, with range [0, height - 1]
		 * @param subRegionWidth The width of the sub-region in which the points will be determined, with range [width - subRegionLeft]
		 * @param subRegionHeight The height of the sub-region in which the points will be determined, with range [height - subRegionTop]
		 * @param minimalThreshold The minimal value a pixel must have to count as feature candidate
		 * @param strictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting locations
		 */
		static inline StrengthPositions determineFeaturePoints(const Frame& yFrame, const unsigned int subRegionLeft, const unsigned int subRegionTop, const unsigned int subRegionWidth, const unsigned int subRegionHeight, const uint8_t minimalThreshold, const bool strictMaximum, Worker* worker = nullptr);

		/**
		 * Determines the locations of the extrema by a standard implementation.
		 * @param yFrame The frame providing the feature points, with pixel format FORMAT_Y8, must be valid
		 * @param minimalThreshold The minimal value a pixel must have to count as feature candidate
		 * @param strictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 * @param lock Optional lock which must be defined if this function is executed on several threads in parallel
		 * @param locations The resulting locations in pixel coordinates
		 * @param firstColumn The first column to be handled, with range [1, width - 2]
		 * @param numberColumns The number of columns to be handled, with range [1, width - firstColumn - 1]
		 * @param firstRow The first row to be handled, with range [1, height - 2]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow - 1]
		 */
		static void determineFeaturePointsSubset(const Frame* yFrame, const uint8_t minimalThreshold, const bool strictMaximum, Lock* lock, StrengthPositions* locations, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Creates a Y8 frame containing a Gaussian blob at a sub-pixel position.
		 * The frame is filled with a 2D Gaussian centered at `(centerX, centerY)` with the given standard deviation.
		 * When `darkBlob` is false the result is a bright blob on a dark background (pixel = 255 * gauss);
		 * when `darkBlob` is true the result is a dark blob on a bright background (pixel = 255 * (1 - gauss)).
		 * @param frameWidth The width of the resulting frame in pixels, with range [1, infinity)
		 * @param frameHeight The height of the resulting frame in pixels, with range [1, infinity)
		 * @param centerX The horizontal sub-pixel center of the Gaussian blob
		 * @param centerY The vertical sub-pixel center of the Gaussian blob
		 * @param sigma The standard deviation of the Gaussian, with range (0, infinity)
		 * @param darkBlob True, to create a dark blob on a bright background; False, to create a bright blob on a dark background
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static Frame createGaussianFrame(const unsigned int frameWidth, const unsigned int frameHeight, const T centerX, const T centerY, const T sigma, const bool darkBlob);

		/**
		 * Compares two IndexPair32 positions by y-coordinate first, then by x-coordinate.
		 * @param a The first position (x, y)
		 * @param b The second position (x, y)
		 * @return True, if a comes before b
		 */
		static inline bool comparePositionYX(const IndexPair32& a, const IndexPair32& b);
};

inline TestNonMaximumSuppression::StrengthPositions TestNonMaximumSuppression::determineFeaturePoints(const Frame& yFrame, const unsigned int subRegionLeft, const unsigned int subRegionTop, const unsigned int subRegionWidth, const unsigned int subRegionHeight, const uint8_t minimalThreshold, const bool strictMaximum, Worker* worker)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(subRegionLeft + subRegionWidth <= yFrame.width() && subRegionTop + subRegionHeight <= yFrame.height());

	const unsigned int firstColumn = std::max(1u, subRegionLeft);
	const unsigned int firstRow = std::max(1u, subRegionTop);

	const unsigned int endColumn = std::min(subRegionLeft + subRegionWidth, yFrame.width() - 1u);
	const unsigned int endRow = std::min(subRegionTop + subRegionHeight, yFrame.height() - 1u);

	const unsigned int numberColumns = endColumn - firstColumn;
	const unsigned int numberRows = endRow - firstRow;

	StrengthPositions result;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&determineFeaturePointsSubset, &yFrame, minimalThreshold, strictMaximum, &lock, &result, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows);
	}
	else
	{
		determineFeaturePointsSubset(&yFrame, minimalThreshold, strictMaximum, nullptr, &result, firstColumn, numberColumns, firstRow, numberRows);
	}

	return result;
}

inline bool TestNonMaximumSuppression::comparePositionYX(const IndexPair32& a, const IndexPair32& b)
{
	return a.second < b.second || (a.second == b.second && a.first < b.first);
}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_NON_MAXIMUM_SUPPRESSION_H
