/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_MASK_ANALYZER_H
#define META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_MASK_ANALYZER_H

#include "ocean/test/testcv/testsegmentation/TestCVSegmentation.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSegmentation
{

/**
 * This class implements a mask analyzer test.
 * @ingroup testcvsegmentation
 */
class OCEAN_TEST_CV_SEGMENTATION_EXPORT TestMaskAnalyzer : public CV::Segmentation::MaskAnalyzer
{
	protected:

		/// Function pointer for distance transform the functions `CV::Segmentation::MaskAnalyzer::compute{C,L1,L2}DistanceTransform8Bit()`
		template <typename TDistanceType>
		using ComputeDistanceTransformFunctionPointer = bool (*) (const uint8_t*, const uint32_t, const uint32_t, TDistanceType*, uint32_t*, const uint8_t, const uint32_t, const uint32_t);

		/**
		 * Definition of an unordered set holding bounding boxes.
		 */
		typedef std::unordered_set<CV::PixelBoundingBox, CV::PixelBoundingBox> BoundingBoxSet;

		/**
		 * Definition of an unordered set holding pixel positions boxes.
		 */
		typedef std::unordered_set<CV::PixelPosition, CV::PixelPosition> PixelPositionSet;

	public:

		/**
		 * Tests all mask analyzer functions.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the has mask neighbor-4 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor4(const double testDuration);

		/**
		 * Tests the has mask neighbor-4 center function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor4Center(const double testDuration);

		/**
		 * Tests the has mask neighbor-5 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor5(const double testDuration);

		/**
		 * Tests the has mask neighbor-5 center function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor5Center(const double testDuration);

		/**
		 * Tests the has mask neighbor-8 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor8(const double testDuration);

		/**
		 * Tests the has mask neighbor-8 center function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor8Center(const double testDuration);

		/**
		 * Tests the has mask neighbor-9 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor9(const double testDuration);

		/**
		 * Tests the has mask neighbor-9 center function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasMaskNeighbor9Center(const double testDuration);

		/**
		 * Tests the distance to border function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetermineDistancesToBorder(const double testDuration, Worker& worker);

		/**
		 * Tests the distance to border function.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param maximalDistance The maximal distance to be used, with range [1, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetermineDistancesToBorder(const unsigned int width, const unsigned int height, const double testDuration, const unsigned int maximalDistance, Worker& worker);

		/**
		 * Tests the find border pixels in a 4-neighborhood function.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testFindBorderPixels4(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the find border pixels in an 8-neighborhood function.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testFindBorderPixels8(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the non-unique pixels in a 4-neighborhood function.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testFindNonUniquePixels4(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the non-unique pixels in an 8-neighborhood function.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testFindNonUniquePixels8(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the outline pixels in a 4-neighborhood function.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFindOutlinePixels4(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Test the distance transformations that uses the chessboard distance
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComputeChessboardDistanceTransform8Bit(const uint32_t width, const uint32_t height, const double testDuration);

		/**
		 * Test the distance transformations that uses the L1 distance
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComputeL1DistanceTransform8Bit(const uint32_t width, const uint32_t height, const double testDuration);

		/**
		 * Test the distance transformations that uses the approximated L2 distance
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComputeL2DistanceTransform8Bit(const uint32_t width, const uint32_t height, const double testDuration);

		/**
		 * Test the determine bounding boxes function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetectBoundingBoxes(const double testDuration);

		/**
		 * Tests the count mask pixels functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCountMaskPixels(const double testDuration);

		/**
		 * Generates test data for the mask analyzer tests
		 * This will generate a white mask frame with a black cross in the center, with 20 pixel width
		 * @param randomGenerator The random generator to be used
		 * @param width The width of the frame that will be generated, range: [1, infinity)
		 * @param height The height of the frame that will be generated, range: [1, infinity)
		 * @param maskValue The value of mask pixels, with range [0, 255]
		 * @param nonMaskValue The value of non-mask pixels, with range [0, 255] without 'maskValue'
		 * @return The generated test frame
		 */
		static Frame generateTestMask(RandomGenerator& randomGenerator, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00u, const uint8_t nonMaskValue = 0xFFu);

	protected:

		/**
		 * Test for distance transform functions
		 * @note The function that can be tested with this function must have an interface as defined by `TestDistanceTransform::ComputeDistanceTransformFunctionPointer`
		 * @param width The width of the images that are used for the performance tests, range: [1, infinity)
		 * @param height The height of the image that are used for the performance tests, range: [1, infinity)
		 * @param computeDistanceTransformPtr A pointer to distance transformation function in `CV::DistanceTransform` that will be tested, must be valid
		 * @param distanceVerticalHorizontal The cost of a vertical/horizontal step; must match the cost of the function that is tested, range: (0, infinity)
		 * @param distanceDiagonal The cost of diagonal step; must match the cost of the function that is tested, range: (0, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True if the validation was successful, otherwise false
		 * @tparam TDistanceType The data type of the resulting distance transform, must be integral or floating-point
		 */
		template <typename TDistanceType>
		static bool testComputeDistanceTransform8Bit(const uint32_t width, const uint32_t height, ComputeDistanceTransformFunctionPointer<TDistanceType> computeDistanceTransformPtr, const TDistanceType distanceVerticalHorizontal, const TDistanceType distanceDiagonal, const double testDuration);

		/**
		 * Validates the distance to border function.
		 * @param mask The original mask frame, must be valid
		 * @param distance The corresponding distance frame to validate, must be valid
		 * @param maximalDistance Maximal distance to be determined
		 * @param assignFinal True, to assign each mask pixel a minimal distance after the final iteration
		 * @return True, if succeeded
		 */
		static bool validateDetermineDistancesToBorder(const Frame& mask, const Frame& distance, const unsigned int maximalDistance, const bool assignFinal);

		/**
		 * Validates the find border pixels in a 4-neighborhood function.
		 * @param mask The mask to verify, must be valid
		 * @param boundingBox The bounding box which has been used, invalid to use the entire mask
		 * @param borderPixels The border pixels to verify
		 * @return True, if succeeded
		 */
		static bool validateFindBorderPixels4(const Frame& mask, const CV::PixelBoundingBox& boundingBox, const CV::PixelPositions& borderPixels);

		/**
		 * Validates the find border pixels in an 8-neighborhood function.
		 * @param mask The mask to verify, must be valid
		 * @param boundingBox The bounding box which has been used, invalid to use the entire mask
		 * @param borderPixels The border pixels to verify
		 * @return True, if succeeded
		 */
		static bool validateFindBorderPixels8(const Frame& mask, const CV::PixelBoundingBox& boundingBox, const CV::PixelPositions& borderPixels);

		/**
		 * Validates the find non-unique pixels in a 4-neighborhood function.
		 * @param mask The mask to verify, must be valid
		 * @param boundingBox The bounding box which has been used, invalid to use the entire mask
		 * @param nonUniquePixels The on unique pixels to verify
		 * @return True, if succeeded
		 */
		static bool validateFindNonUniquePixels4(const Frame& mask, const CV::PixelBoundingBox& boundingBox, const CV::PixelPositions& nonUniquePixels);

		/**
		 * Validates the find non-unique pixels in an 8-neighborhood function.
		 * @param mask The mask to verify, must be valid
		 * @param boundingBox The bounding box which has been used, invalid to use the entire mask
		 * @param nonUniquePixels The on unique pixels to verify
		 * @return True, if succeeded
		 */
		static bool validateFindNonUniquePixels8(const Frame& mask, const CV::PixelBoundingBox& boundingBox, const CV::PixelPositions& nonUniquePixels);

		/**
		 * Validates the find outline pixels in a 4-neighborhood function.
		 * @param mask The mask to verify, must be valid
		 * @param boundingBox The bounding box which has been used, invalid to use the entire mask
		 * @param outlinePixels The outline pixels to verify
		 * @param nonMaskValue The value of non-mask pixels, with range [0, 255] without 'maskValue'
		 * @return True, if succeeded
		 */
		static bool validateFindOutlinePixels4(const Frame& mask, const CV::PixelBoundingBox& boundingBox, const CV::PixelPositions& outlinePixels, const uint8_t nonMaskValue);

		/**
		 * Validation function for the distance transform function
		 * @param source A pointer to the source frame data, must be valid
		 * @param width The width of the source frame, range: [1, infinity)
		 * @param height The height of the source frame, range: [1, infinity)
		 * @param validationTarget A pointer to the result of a distance transform that will be validated, must be valid and have the same size as the source frame
		 * @param validationTargetStatus The return value of the distance transform function that will be validated
		 * @param distanceVerticalHorizontal e cost of a vertical/horizontal step; must match the cost of the function that is tested, range: (0, infinity)
		 * @param distanceDiagonal The cost of diagonal step; must match the cost of the function that is tested, range: (0, infinity)
		 * @param referenceValue Distance values pixels will be computed to the closest pixel with this reference value, range: [0, 255] and must match that of the distance transform that is validated
		 * @param sourcePaddingElements Optional number of padding elements of the source frame, range: [0, infinity)
		 * @param validationTargetPaddingElements Optional number padding elements of the target frame, range: [0, infinity)
		 * @return True if the validation was successful, otherwise false
		 * @tparam TDistanceType The data type of the distance transform that will be validated, this must be an integral or floating-point type
		 */
		template <typename TDistanceType>
		static bool validateComputeDistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, const TDistanceType* validationTarget, const bool validationTargetStatus, const TDistanceType distanceVerticalHorizontal, const TDistanceType distanceDiagonal, const uint8_t referenceValue, const uint32_t sourcePaddingElements = 0u, const uint32_t validationTargetPaddingElements = 0u);
};

} // namespace TestSegmentation

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_MASK_ANALYZER_H
