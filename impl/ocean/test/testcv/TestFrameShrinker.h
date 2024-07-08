/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_SHRINKER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_SHRINKER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FrameShrinker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the frame downsizing functions.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameShrinker : protected CV::FrameShrinker
{
	public:

		/**
		 * Tests the frame downsize.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the downsampling of three rows to one row.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRowDownsamplingByTwoThreeRows8Bit121(const double testDuration);

		/**
		 * Tests the 8 bit frame downsampling using 11 filtering.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDownsamplingByTwo8Bit11(const double testDuration, Worker& worker);

		/**
		 * Tests the 8 bit frame downsampling using 11 filtering for extreme (small) frame resolutions.
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDownsamplingByTwo8Bit11ExtremeResolutions(Worker& worker);

		/**
		 * Tests the 8 bit frame downsampling using 14641 filtering.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDownsamplingByTwo8Bit14641(const double testDuration, Worker& worker);

		/**
		 * Tests the 8 bit frame downsampling using 14641 filtering for extreme (small) frame resolutions.
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDownsamplingByTwo8Bit14641ExtremeResolutions(Worker& worker);

		/**
		 * Tests the binary frame downsampling using 11 filtering.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testDownsampleBinayMaskByTwo11(const double testDuration, Worker& worker);

		/**
		 * Tests the binary frame downsampling using 11 filtering for extreme (small) frame resolutions.
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testDownsampleBinayMaskByTwo11ExtremeResolutions(Worker& worker);

		/**
		 * Tests the 8 bit pyramid downsampling using 11 filtering.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testPyramidByTwo11(const double testDuration, Worker& worker);

		/**
		 * Tests the 8 bit frame downsampling using 11 filtering.
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDownsamplingByTwo8Bit11(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the binary frame downsampling using 11 filtering.
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testDownsampleBinayMaskByTwo11(const unsigned int sourceWidth, const unsigned int sourceHeight, const double testDuration, Worker& worker);

		/**
		 * Tests the 8 bit frame downsampling using 14641 filtering.
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [sourceWidth / 2, (sourceWidth + 1) / 2]
		 * @param targetHeight Height of the target frame in pixel, with range [sourceHeight / 2, (sourceHeight + 1) / 2]
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDownsamplingByTwo8Bit14641(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the downsampling of a frame using a 11 filtering.
		 * @param source The source frame which has been downsampled, must be valid
		 * @param target The target frame holding the downsampled frame, must be valid
		 * @param averageAbsError Optional resulting average absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param maximalAbsError Optional resulting maximal absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param groundTruth Optional resulting ground truth data, with buffer size (sourceWidth / 2) * (sourceHeight / 2) * channels, otherwise nullptr
		 * @param groundTruthPaddingElements Optional number of padding elements at the end of each ground truth row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool validateDownsamplingByTwo8Bit11(const Frame& source, const Frame& target, double* averageAbsError, double* maximalAbsError, uint8_t* groundTruth = nullptr, const unsigned int groundTruthPaddingElements = 0u);

		/**
		 * Validates the binary downsampling of a frame using a 11 filtering.
		 * @param source The source frame holding an binary image, must be valid
		 * @param target The target frame holding the halved binary image, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param threshold Minimal sum threshold of four pixels to result in a pixel with value 255, with range [1, 255 * 4]
		 * @return True, if succeeded
		 */
		static bool validateDownsampleBinayMaskByTwo11(const uint8_t* source, const uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int threshold);

		/**
		 * Validates the downsampling of a frame using a 14641 filtering.
		 * @param source The source frame holding an 8 bit image per channel, must be valid
		 * @param target The target frame holding the downsampled frame, also 8 bit per channel, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [sourceWidth / 2, (sourceWidth + 1) / 2]
		 * @param targetHeight Height of the target frame in pixel, with range [sourceHeight / 2, (sourceHeight + 1) / 2]
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param averageAbsError Optional resulting average absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param maximalAbsError Optional resulting maximal absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param groundTruth Optional resulting ground truth data, with buffer size (sourceWidth / 2) * (sourceHeight / 2) * channels, otherwise nullptr
		 * @param groundTruthPaddingElements Optional number of padding elements at the end of each ground truth row, in elements, with range [0, infinity)
		 */
		static void validateDownsamplingByTwo8Bit14641(const uint8_t* source, const uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsError, double* maximalAbsError, uint8_t* groundTruth = nullptr, const unsigned int groundTruthPaddingElements = 0u);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_SHRINKER_H
