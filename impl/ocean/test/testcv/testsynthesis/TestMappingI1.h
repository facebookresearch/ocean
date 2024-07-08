/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_MAPPING_I_1_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_MAPPING_I_1_H

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/synthesis/MappingI1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

/**
 * This class implements a synthesis pixel mapping test.
 * @ingroup testcvsynthesis
 */
class OCEAN_TEST_CV_SYNTHESIS_EXPORT TestMappingI1 : protected CV::Synthesis::MappingI1
{
	public:

		/**
		 * Tests all mapping functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the applyMapping function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testApplyMapping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the applyMapping function for a specific pixel format.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels to be used, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testApplyMapping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the sum of squared differences calculation using a mask and skipping the center pixel.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSumSquaredDifference5x5MaskNoCenter(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum of squared differences calculation using a mask and skipping the center pixel.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels to be used, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testSumSquaredDifference5x5MaskNoCenter(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the 5x5 appearance cost function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAppearanceCost5x5(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the 5x5 appearance cost function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels to be used, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testAppearanceCost5x5(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the spatial cost with 4-neighborhood.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSpatialCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the spatial cost with 4-neighborhood.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels to be used, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testSpatialCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the spatial cost with 8-neighborhood.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSpatialCost8Neighborhood(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the spatial cost with 8-neighborhood.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels to be used, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testSpatialCost8Neighborhood(const unsigned int width, const unsigned int height, const double testDuration);

	private:

		/**
		 * Validates the mapping results for a frame and corresponding mapping.
		 * @param frame The result frame, must be valid
		 * @param mask The mask frame, must be valid
		 * @param mapping The mapping which has been used
		 * @param boundingBox The bounding box defining the area of interest, must be valid
		 * @return True, if succeeded
		 */
		static bool validateMapping(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingI1& mapping, const CV::PixelBoundingBox& boundingBox);

		/**
		 * Validates the 5x5 sum of squared differences skipping the center pixel.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, must be valid
		 * @param mask0 The mask for the first frame, must be valid
		 * @param topLeftPositions0 The top left positions in the first frame
		 * @param topLeftPositions1 The top left positions in the second frame
		 * @param results The SSD results to verify, one for each position
		 * @param borderFactor The border factor to be used
		 * @return True, if succeeded
		 */
		static bool validateSumSquaredDifference5x5MaskNoCenter(const Frame& frame0, const Frame& frame1, const Frame& mask0, const CV::PixelPositions& topLeftPositions0, const CV::PixelPositions& topLeftPositions1, const Indices32& results, const unsigned int borderFactor);

		/**
		 * Determines the appearance cost.
		 * @param frame The frame for which the cost will be determined, must be valid
		 * @param mask The mask associated with the frame, defining valid and invalid frame pixels, must be valid
		 * @param source The source location within the frame, with range [0, frame.width()-1]x[0, frame.height()-1]
		 * @param target The target location within the frame, with range [0, frame.width()-1]x[0, frame.height()-1]
		 * @param patchSize The size of the image patch, in pixels, with range [1, infinity), must be odd
		 * @param borderFactor The factor which will be applied to target pixels outside of the mask, with range [1, infinity)
		 * @param normalizationFactor The normalization factor to be applied, with range [1, infinity)
		 * @return The resulting cost
		 */
		static uint64_t determineAppearanceCost(const Frame& frame, const Frame& mask, const CV::PixelPosition& source, const CV::PixelPosition& target, const unsigned int patchSize, const unsigned int borderFactor, const unsigned int normalizationFactor);

		/**
		 * Determines the spatial cost in a 4-neighborhood.
		 * @param mapping The mapping to be used, must be valid
		 * @param mask The mask associated with the frame, defining valid and invalid frame pixels, must be valid
		 * @param source The source location within the frame, with range [0, frame.width()-1]x[0, frame.height()-1]
		 * @param target The target location within the frame, with range [0, frame.width()-1]x[0, frame.height()-1]
		 * @param maxCost The maximal cost, with range [0, infinity)
		 * @param normalizationFactor The normalization factor to be applied, with range [1, infinity)
		 * @return The resulting cost
		 */
		static uint64_t determineSpatialCost4Neighborhood(const CV::Synthesis::MappingI1& mapping, const Frame& mask, const CV::PixelPosition& source, const CV::PixelPosition& target, const uint32_t maxCost, const unsigned int normalizationFactor);

		/**
		 * Determines the spatial cost in a 8-neighborhood.
		 * @param mapping The mapping to be used, must be valid
		 * @param mask The mask associated with the frame, defining valid and invalid frame pixels, must be valid
		 * @param source The source location within the frame, with range [0, frame.width()-1]x[0, frame.height()-1]
		 * @param target The target location within the frame, with range [0, frame.width()-1]x[0, frame.height()-1]
		 * @param maxCost The maximal cost, with range [0, infinity)
		 * @param normalizationFactor The normalization factor to be applied, with range [1, infinity)
		 * @return The resulting cost
		 */
		static uint64_t determineSpatialCost8Neighborhood(const CV::Synthesis::MappingI1& mapping, const Frame& mask, const CV::PixelPosition& source, const CV::PixelPosition& target, const uint32_t maxCost, const unsigned int normalizationFactor);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_MAPPING_I_1_H
