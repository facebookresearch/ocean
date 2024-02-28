// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/test/testunifiedfeatures/testoceanharris/TestOceanHarris.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/HarrisCorner.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

namespace Ocean
{

namespace Test
{

namespace TestUnifiedFeatures
{

namespace TestOceanHarris
{
/**
 * This class implements a test for the unified Ocean Harris detector.
 * @ingroup testunifiedfeaturesoceanharris
 */
class OCEAN_TEST_UNIFIEDFEATURES_EXPORT TestOceanHarrisContainer
{
	public:

		/**
		 * Starts all tests
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the serialization
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSerialization(const double testDuration);

	protected:

		/**
		 * Cf. TestFeatureContainer::CreateFeatureContainerCallback
		 */
		static std::shared_ptr<UnifiedFeatures::FeatureContainer> createOceanHarrisContainer(RandomGenerator& randomGenerator, const size_t numberFeatures);

		/**
		 * Cf. TestFeatureContainer::ValidateDeserializationCallback
		 */
		static bool validateDeserialization(const std::shared_ptr<UnifiedFeatures::FeatureContainer>& featureContainer0, const std::shared_ptr<UnifiedFeatures::FeatureContainer>& featureContainer1);

		/**
		 * Generates random Harris corners
		 * @param randomGenerator The random generator that will be used to generate the random Harris corners
		 * @param numberHarrisCorners The number of random Harris corners that will be created, range: [0, infinity)
		 * @return The randomized Harris corners
		 */
		static CV::Detector::HarrisCorners generateRandomHarrisCorners(RandomGenerator& randomGenerator, const size_t numberHarrisCorners);
};

} // namespace TestOceanHarris

} // namespace TestUnifiedFeatures

} // namespace Test

} // namespace Ocean
