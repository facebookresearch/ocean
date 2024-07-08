/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_HISTOGRAM_H
#define META_OCEAN_TEST_TESTCV_TEST_HISTOGRAM_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/Histogram.h"

namespace Ocean
{

/// Forward-declaration of class RandomGenerator
class RandomGenerator;

namespace Test
{

namespace TestCV
{

/**
 * This class implements a pixel bounding box test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestHistogram : protected CV::ContrastLimitedAdaptiveHistogram
{
	public:

		/**
		 * Tests the histogram function.
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if all tests of this class passed, otherwise false
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Performance and validation test for the computation of histograms
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 */
		static bool testDetermineHistogram8BitPerChannel(const double testDuration, Worker& worker);

		/**
		 * Performance and validation test for the computation of histograms on sub-frames/-regions of an image
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 */
		static bool testDetermineHistogram8BitPerChannelSubFrame(const double testDuration, Worker& worker);

		/**
		 * Performance and validation test for the computation of the CLAHE lookup tables
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 */
		static bool testContrastLimitedAdaptiveHistogramTileLookupTables(const double testDuration, Worker& worker);

		/**
		 * Validation test for the computation of the CLAHE horizontal bilinear interpolation parameters
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @return True if validation was successful, otherwise false
		 */
		static bool testContrastLimitedAdaptiveBilinearInterpolationParameters(const double testDuration);

		/**
		 * Test the Contrast-Limited Histogram Equalization (CLAHE)
		 * @param testDuration Number of seconds that this test will run, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if the test passed, otherwise false
		 */
		static bool testContrastLimitedHistogramEqualization(const double testDuration, Worker& worker);

		/**
		 * Validation function for the computation of the CLAHE horizontal (or vertical) bilinear interpolation parameters
		 * @param lowBins Left (or top) bins for each pixel in a column (or row) that will be validated, must be initialized, size identical to `imageEdgeLength`
		 * @param lowFactors_fixed7 Left (or top) interpolation factors for each pixel in a column (or row) that will be validated, must be initialized as fixed-point, 7-bit precision numbers, size identical to `imageEdgeLength`
		 * @param imageEdgeLength Image edge length, either width or height, range: [1, infinity)
		 * @param tilesCount Number of horizontal (or vertical) tiles, range: [1, infinity)
		 * @return True if the validation was successful, otherwise false
		 */
		static bool validateBilinearInterpolationParameters(const Indices32& lowBins, const std::vector<unsigned char>& lowFactors_fixed7, const unsigned int imageEdgeLength, const unsigned int tilesCount);

		/**
		 * Validation function for the computation of the CLAHE tile lookup tables
		 * @param source Pointer to the source image, must be initialized, expected size: `width` x `height`
		 * @param width The width of the source image
		 * @param height The height of the source image
		 * @param validationLookupTables Values that will be validated, must be initialized, expected size: `width` x `height`
		 * @param horizontalTiles Number of horizontal tiles, range: [1, infinity)
		 * @param verticalTiles Number of vertical tiles, range: [1, infinity)
		 * @param clipLimit CLAHE clip limit, range: [1, infinity)
		 * @param maxAbsError Will hold the value of the largest absolute error measured during the validation, otherwise this will be ignored, range: [0, 255]
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0u, infinity), default: 0
		 * @param maxAbsErrorThresholdMultiple Maximum allowed absolute error value if the both, image width and height, are a multiples of the horizontal and vertical number of tiles tiles, range: [0, infinity), default: 1.0
		 * @param maxAbsErrorThreshold Maximum allowed absolute error value if the image is not a multiple of the number of tiles, range: [0, infinity), default: 25.0
		 * @return Number of tiles for which the validation has failed. Will be zero if all tile validations were successful, range: [0, horizontalTiles * verticalTiles]
		 */
		static unsigned int validateTileLookupTables(const unsigned char* source, const unsigned int width, const unsigned int height, const std::vector<unsigned char>& validationLookupTables, const unsigned int horizontalTiles, const unsigned int verticalTiles, const Scalar clipLimit, double& maxAbsError, const unsigned int sourcePaddingElements = 0u, const double maxAbsErrorThresholdMultiple = 1.0, const double maxAbsErrorThreshold = 25.0);

		/**
		 * Validation function for CLAHE
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param width The width of the source and target frames, range: [horizontalTiles, infinity)
		 * @param height The height of the source and target frames, range: [verticalTiles, infinity)
		 * @param validationTarget Equalization result that will be validated, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`) and have the same size as the source
		 * @param clipLimit Global scaling factor to determine the tile clip limit, `tileClipLimit = clipLimit * N`, where `N = (tileSize / histogramSize)` is the number of pixels per bin if all pixels are distributed evenly over the histogram (average), range: [0, infinity)
		 * @param horizontalTiles Number of tiles the source image will be split horizontally, range: [1, infinity)
		 * @param verticalTiles Number of tiles the source image will be split vertically, range: [1, infinity)
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param validationTargetPaddingElements Number of padding elements in the target data, range: [0, infinity), default: 0
		 * @param maxError If specified, it will hold the maximum absolute error value that has been found during the validation, otherwise this parameter will be ignored
		 * @param groundtruth If specified, it will contain the groundtruth used in the validation; if specified, it must be initialized to the same size of source (no padding allowed)
		 * @return True if the validation was successful, otherwise false
		 */
		static bool validateEqualization8BitPerChannel(const unsigned char* const source, const unsigned int width, const unsigned height, const unsigned char* const validationTarget, const Scalar clipLimit = Scalar(40), const unsigned int horizontalTiles = 8u, const unsigned int verticalTiles = 8u, const unsigned int sourcePaddingElements = 0u, const unsigned int validationTargetPaddingElements = 0u, double* maxError = nullptr, unsigned char* groundtruth = nullptr);

		/**
		 * Performance and validation test for the computation of the CLAHE lookup tables
		 * @param width The width of the images that will be tested, if 0u the width will be selected randomly (max. 2000), range: [16 * horizontalTiles, infinity)
		 * @param height The height of the images that will be tested, if 0u the height will be selected randomly (max. 2000), range: [16 * verticalTiles, infinity)
		 * @param clipLimit Clip limit to used for testing, if <= 0 this parameter will be selected randomly (between 1 and 100), range: [1, infinity)
		 * @param horizontalTiles Number of horizontal tiles to be used for testing, if 0 this parameter will be selected randomly (between 2 and 16), range: [2, infinity)
		 * @param verticalTiles Number of vertical tiles to be used for testing, if 0 this parameter will be selected randomly (between 2 and 16), range: [2, infinity)
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 */
		static bool testContrastLimitedAdaptiveHistogramTileLookupTables(const unsigned int width, const unsigned int height, const Scalar clipLimit, const unsigned int horizontalTiles, const unsigned int verticalTiles, const double testDuration, Worker& worker);

		/**
		 * Test the Contrast-Limited Histogram Equalization (CLAHE)
		 * @param width The width of the images that will be tested, if 0u the width will be selected randomly (max. 2000), range: [16 * horizontalTiles, infinity)
		 * @param height The height of the images that will be tested, if 0u the height will be selected randomly (max. 2000), range: [16 * verticalTiles, infinity)
		 * @param clipLimit Clip limit to used for testing, if <= 0 this parameter will be selected randomly (between 1 and 100), range: [1, infinity)
		 * @param horizontalTiles Number of horizontal tiles to be used for testing, if 0 this parameter will be selected randomly (between 2 and 16), range: [2, infinity)
		 * @param verticalTiles Number of vertical tiles to be used for testing, if 0 this parameter will be selected randomly (between 2 and 16), range: [2, infinity)
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 */
		static bool testContrastLimitedHistogramEqualization(const unsigned int width, const unsigned int height, const Scalar clipLimit, const unsigned int horizontalTiles, const unsigned int verticalTiles, const double testDuration, Worker& worker);

		/**
		 * Performance and validation test for the computation of histograms
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param width The width of the images that will be tested, if 0u the width will be selected randomly (max. 2000), range: [1, infinity)
		 * @param height The height of the images that will be tested, if 0u the height will be selected randomly (max. 2000), range: [1, infinity)
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 * @tparam tChannels Number of channels to use for this test, range: [1, 4]
		 */
		template <unsigned int tChannels>
		static bool testDetermineHistogram8BitPerChannel(const double testDuration, const unsigned int width, const unsigned int height, RandomGenerator& randomGenerator, Worker& worker);

		/**
		 * Performance and validation test for the computation of histograms for sub-frames/-regions of images
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @param width The width of the images that will be tested, if 0u the width will be selected randomly (max. 2000), range: [1, infinity)
		 * @param height The height of the images that will be tested, if 0u the height will be selected randomly (max. 2000), range: [1, infinity)
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @param worker A worker instance for parallel execution of the tested function (performance)
		 * @return True if validation was successful, otherwise false
		 * @tparam tChannels Number of channels to use for this test, range: [1, 4]
		 */
		template <unsigned int tChannels>
		static bool testDetermineHistogram8BitPerChannelSubFrame(const double testDuration, const unsigned int width, const unsigned int height, RandomGenerator& randomGenerator, Worker& worker);

	protected:

		/**
		 * Validate the computation of image histograms
		 * @param frame Valid pointer to the data of the original input image
		 * @param width The width of the images that will be tested, if 0u the width will be selected randomly (max. 2000), range: [1, infinity)
		 * @param height The height of the images that will be tested, if 0u the height will be selected randomly (max. 2000), range: [1, infinity)
		 * @param framePaddingElements Number of padding elements of the input image, range: [0, infinity)
		 * @param subFrameX Horizontal coordinate of the top-left corner of the sub-frame for which the histogram will be validated, range: [0, width)
		 * @param subFrameY Vertical coordinate of the top-left corner of the sub-frame for which the histogram will be validated, range: [0, height)
		 * @param subFrameWidth Width of the sub-frame that will be validated, range: [1, width) such that `subFrameX + subFrameWidth <= width`
		 * @param subFrameHeight Height of the sub-frame that will be validated, range: [1, height) such that `subFrameY + subFrameHeight <= height`
		 * @param histogramToValidate The histogram that will be validated given the input image `frame`.
		 * @return True if validation was successful, otherwise false
		 * @tparam tChannels Number of channels of the input image `frame`, range: [1, 4]
		 */
		template <unsigned int tChannels>
		static bool validateDetermineHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int subFrameX, const unsigned int subFrameY, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const CV::Histogram::Histogram8BitPerChannel<tChannels>& histogramToValidate);
};

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_HISTOGRAM_H
