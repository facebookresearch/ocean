/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_OPTIMIZER_F_1_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_OPTIMIZER_F_1_H

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/synthesis/MappingF1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

/**
 * This class implements a test for optimizers with one frame.
 * @ingroup testcvsynthesis
 */
class OCEAN_TEST_CV_SYNTHESIS_EXPORT TestOptimizerF1
{
	public:

		/**
		 * Invokes all test functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood high performance optimizer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHighPerformance4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood high performance optimizer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHighPerformance4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood optimizer with a reference frame.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testReferenceFrame4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood optimizer with a reference frame.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testReferenceFrame4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Invokes a high performance optimization with a 4-neighborhood.
		 * @param frame The frame to be used, must be valid
		 * @param mask The mask to be used, must be valid
		 * @param mapping The mapping to be used, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param applyInitialMapping True, to apply the initial mapping; False, to skip the initial mapping
		 * @param radii The number of search radii, with range [1, infinity)
		 * @param iterations The number of optimization iterations, with range [1, infinity)
		 * @param weightFactor The weight factor to be applied, with range [1, infinity)
		 * @param maxSpatialCost The maximal spatial cost to be used, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam tBorderFactor The border factor to be used, with range [1, infinity)
		 */
		template <unsigned int tBorderFactor>
		static bool optimize4Neighborhood(Frame& frame, const Frame& mask, CV::Synthesis::MappingF1& mapping, RandomGenerator& randomGenerator, const bool applyInitialMapping, const unsigned int radii, const unsigned int iterations, const unsigned int weightFactor, const unsigned int maxSpatialCost);

		/**
		 * Invokes an optimization with a 4-neighborhood and a reference frame.
		 * @param frame The frame to be used, must be valid
		 * @param mask The mask to be used, must be valid
		 * @param reference The reference frame to be used, must be valid
		 * @param mapping The mapping to be used, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param applyInitialMapping True, to apply the initial mapping; False, to skip the initial mapping
		 * @param radii The number of search radii, with range [1, infinity)
		 * @param iterations The number of optimization iterations, with range [1, infinity)
		 * @param weightFactor The weight factor to be applied, with range [1, infinity)
		 * @param maxSpatialCost The maximal spatial cost to be used, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam tBorderFactor The border factor to be used, with range [1, infinity)
		 */
		template <unsigned int tBorderFactor>
		static bool optimizeReference4Neighborhood(Frame& frame, const Frame& mask, const Frame& reference, CV::Synthesis::MappingF1& mapping, RandomGenerator& randomGenerator, const bool applyInitialMapping, const unsigned int radii, const unsigned int iterations, const unsigned int weightFactor, const unsigned int maxSpatialCost);

		/**
		 * Determines the cost for a given location.
		 * @param frame The frame to be used, must be valid
		 * @param mask The mask to be used, must be valid
		 * @param mapping The mapping to be used, must be valid
		 * @param targetPosition The target position for which the cost will be determined
		 * @param sourcePosition The source position for which the cost will be determined
		 * @param weightFactor The weight factor to be applied
		 * @param maxSpatialCost The maximal spatial cost to be used
		 * @return The resulting cost composed of spatial and appearance cost
		 * @tparam tBorderFactor The border factor to be used
		 */
		template <unsigned int tBorderFactor>
		static Scalar determineCost(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingF1& mapping, const CV::PixelPosition& targetPosition, const Vector2& sourcePosition, const unsigned int weightFactor, const unsigned int maxSpatialCost);

		/**
		 * Determines the cost for a given location.
		 * @param frame The frame to be used, must be valid
		 * @param mask The mask to be used, must be valid
		 * @param reference The reference frame to be used, must be valid
		 * @param mapping The mapping to be used, must be valid
		 * @param targetPosition The target position for which the cost will be determined
		 * @param sourcePosition The source position for which the cost will be determined
		 * @param weightFactor The weight factor to be applied
		 * @param maxSpatialCost The maximal spatial cost to be used
		 * @return The resulting cost composed of spatial and appearance cost
		 * @tparam tBorderFactor The border factor to be used
		 */
		template <unsigned int tBorderFactor>
		static Scalar determineCost(const Frame& frame, const Frame& mask, const Frame& reference, const CV::Synthesis::MappingF1& mapping, const CV::PixelPosition& targetPosition, const Vector2& sourcePosition, const unsigned int weightFactor, const unsigned int maxSpatialCost);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_OPTIMIZER_F_1_H
