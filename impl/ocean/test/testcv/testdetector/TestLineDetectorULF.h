/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_DETECTOR_ULF_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_DETECTOR_ULF_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/LineDetectorULF.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements line detector tests.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestLineDetectorULF : protected CV::Detector::LineDetectorULF
{
	public:

		/**
		 * Invokes all test for the line detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the sliding window sum function for single rows calculating the sums of pixel intensities only.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRowSums(const double testDuration);

		/**
		 * Tests the sliding window sum functions for single rows calculating the sums of pixel intensities and the sums of squared pixel intensities.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRowSqrSums(const double testDuration);

		/**
		 * Tests the mean square residual bar edge detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRMSBarEdgeDetector(const double testDuration);

		/**
		 * Tests the horizontal RMS Bar edge detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHorizontalRMSBarEdgeDetector(const double testDuration);

		/**
		 * Tests the mean square residual bar line detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRMSBarLineDetector(const double testDuration);

		/**
		 * Tests the mean square residual step edge detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRMSStepEdgeDetector(const double testDuration);

		/**
		 * Tests the horizontal RMS Step edge detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHorizontalRMSStepEdgeDetector(const double testDuration);

		/**
		 * Tests the mean square residual step line detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRMSStepLineDetector(const double testDuration);

		/**
		 * Tests the sum difference edge detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSDStepEdgeDetector(const double testDuration);

		/**
		 * Tests the horizontal SD Step edge detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHorizontalSDStepEdgeDetector(const double testDuration);

	protected:

		/**
		 * Tests the horizontal edge detector of a specific detector.
		 * @param edgeDetector The edge detector to be tested, must support a horizontal detection function
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHorizontalEdgeDetector(const EdgeDetector& edgeDetector, const double testDuration);

		/**
		 * Determines the horizontal RMS-based bar edge response for one pixel with floating point precision.
		 * The response applies non-maximum suppression within a 3-neighborhood.
		 * @param yFrame The 8 bit grayscale frame for which the response will be calculated, must be valid, with dimension [window * 2 + 3, infinity)x[1, infinity)
		 * @param x The horizontal location within the frame, with range [0, width - 1]
		 * @param y The vertical location within the frame, with range [0, height - 1]
		 * @param windowSize The size of the window to be used, in pixel, with range [1, infinity)
		 * @param minimalDelta The minimal intensity delta between average and center pixel, with range [0, 255]
		 * @return The response at the specified location
		 */
		static double rmsBarEdgeResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int windowSize, const double minimalDelta = 5.0);

		/**
		 * Determines the horizontal RMS-based step edge response for one pixel with floating point precision.
		 * The response applies non-maximum suppression within a 3-neighborhood.
		 * @param yFrame The 8 bit grayscale frame for which the response will be calculated, must be valid, with dimension [window * 2 + 3, infinity)x[1, infinity)
		 * @param x The horizontal location within the frame, with range [0, width - 1]
		 * @param y The vertical location within the frame, with range [0, height - 1]
		 * @param windowSize The size of the window to be used, in pixel, with range [1, infinity)
		 * @param decisionDelta Optional resulting minimal delta between center response and neighbor response
		 * @return The response at the specified location
		 * @tparam tSeparateResidual True, to apply a separate residual for left and right window; False, to apply a common residual
		 * @tparam tSignedSquaredResponse True, to apply the squared (but signed) response values; False, to determine the non-squared responses
		 */
		template <bool tSeparateResidual, bool tSignedSquaredResponse>
		static double rmsStepEdgeResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int windowSize, double* decisionDelta = nullptr);

		/**
		 * Determines the horizontal RMS-based step edge response for one pixel with floating point precision.
		 * The response does not apply non-maximum suppression within a 3-neighborhood.
		 * @param yFrame The 8 bit grayscale frame for which the response will be calculated, must be valid, with dimension [window * 2 + 3, infinity)x[1, infinity)
		 * @param x The horizontal location within the frame, with range [0, width - 1]
		 * @param y The vertical location within the frame, with range [0, height - 1]
		 * @param windowSize The size of the window to be used, in pixel, with range [1, infinity)
		 * @return The response at the specified location
		 * @tparam tSeparateResidual True, to apply a separate residual for left and right window; False, to apply a common residual
		 */
		template <bool tSeparateResidual>
		static double rmsStepEdgeResponseWithoutNonMaximumSuppression(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int windowSize);

		/**
		 * Determines the horizontal sum difference step edge detector response for a single pixel.
		 * The response applies non-maximum suppression.
		 * @param yFrame The 8 bit grayscale frame for which the response will be calculated, must be valid, with dimension [window * 2 + 3, infinity)x[1, infinity)
		 * @param x The horizontal location within the frame, with range [0, width - 1]
		 * @param y The vertical location within the frame, with range [0, height - 1]
		 * @param stepSize The number of pixels between both windows sums, "should be odd", "should" have range [1, infinity)
		 * @param windowSize The size of the window to be used, in pixel, with range [1, infinity)
		 * @return The response at the specified location
		 */
		static double sdStepEdgeResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int stepSize, const unsigned int windowSize);

		/**
		 * Determines the horizontal sum difference step edge detector response for a single pixel.
		 * The response does not apply non-maximum suppression.
		 * @param yFrame The 8 bit grayscale frame for which the response will be calculated, must be valid, with dimension [window * 2 + 3, infinity)x[1, infinity)
		 * @param x The horizontal location within the frame, with range [0, width - 1]
		 * @param y The vertical location within the frame, with range [0, height - 1]
		 * @param stepSize The number of pixels between both windows sums, "should be odd", "should" have range [1, infinity)
		 * @param windowSize The size of the window to be used, in pixel, with range [1, infinity)
		 * @return The response at the specified location
		 */
		static double sdStepEdgeResponseWithoutNonMaximumSuppression(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int stepSize, const unsigned int windowSize);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_DETECTOR_ULF_H
