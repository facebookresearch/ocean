/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_OPTIMIZER_I_1_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_OPTIMIZER_I_1_H

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/synthesis/Constraint.h"
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
 * This class implements a test for optimizers with one frame.
 * @ingroup testcvsynthesis
 */
class OCEAN_TEST_CV_SYNTHESIS_EXPORT TestOptimizerI1
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
		 * Tests the 4-neighborhood optimizer with constrained area.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAreaConstrained4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood optimizer with constrained area.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAreaConstrained4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

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
		 * Tests the 4-neighborhood high performance optimizer skipping if spatial cost is already optimal.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHighPerformance4NeighborhoodSkipping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood high performance optimizer skipping if spatial cost is already optimal.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHighPerformance4NeighborhoodSkipping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood high performance optimizer with skipping mask.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHighPerformance4NeighborhoodSkippingByCostMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood high performance optimizer with skipping mask.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHighPerformance4NeighborhoodSkippingByCostMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood optimizer with structural constrains.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testStructuralConstrained4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood optimizer with structural constrains.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testStructuralConstrained4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Invokes a high performance optimization with a 4-neighborhood.
		 * @param frame The frame to be used, must be valid
		 * @param mask The mask to be used, must be valid
		 * @param filterMask Optional filter mask to constrained the optimization; invalid to avoid and additional filter
		 * @param skippingMask Optional mask defining pixels to be skipped; invalid to avoid this additional option
		 * @param constraints Optional constraints to be applied during optimization, nullptr to avoid using constraints
		 * @param mapping The mapping to be used, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param applyInitialMapping True, to apply the initial mapping; False, to skip the initial mapping
		 * @param radii The number of search radii, with range [1, infinity)
		 * @param iterations The number of optimization iterations, with range [1, infinity)
		 * @param weightFactor The weight factor to be applied, with range [1, infinity)
		 * @param maxSpatialCost The maximal spatial cost to be used, with range [0, infinity)
		 * @param spatialSkipping True, to apply spatial skipping
		 * @return True, if succeeded
		 * @tparam tBorderFactor The border factor to be used, with range [1, infinity)
		 */
		template <unsigned int tBorderFactor>
		static bool optimize4Neighborhood(Frame& frame, const Frame& mask, const Frame& filterMask, const Frame& skippingMask, const CV::Synthesis::Constraints* constraints, CV::Synthesis::MappingI1& mapping, RandomGenerator& randomGenerator, const bool applyInitialMapping, const unsigned int radii, const unsigned int iterations, const unsigned int weightFactor, const unsigned int maxSpatialCost, const bool spatialSkipping);

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
		static uint64_t determineCost(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingI1& mapping, const CV::PixelPosition& targetPosition, const CV::PixelPosition& sourcePosition, const unsigned int weightFactor, const unsigned int maxSpatialCost);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_OPTIMIZER_I_1_H
