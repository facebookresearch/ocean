// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/test/testunifiedfeatures/TestUnifiedFeatures.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

namespace Ocean
{

namespace Test
{

namespace TestUnifiedFeatures
{

/**
 * This class implements the base test for feature containers.
 * @ingroup testunifiedfeatures
 */
class OCEAN_TEST_UNIFIEDFEATURES_EXPORT TestFeatureContainer
{
	public:

		/**
		 * Callback function that generates a specialized instance of a feature container with specified number of (random) features
		 * @param randomGenerator The random generator that should be used in order to generate the random features
		 * @param numberFeatures The number of features that will be created and stored inside the feature container, range: [0, infinity)
		 * @return A pointer feature container that was created
		 */
		typedef std::shared_ptr<UnifiedFeatures::FeatureContainer> (*CreateFeatureContainerCallback)(RandomGenerator& randomGenerator, const size_t numberFeatures);

		/**
		 * Callback function that checks if a pair of feature container are weakly identical
		 * @param featureContainer0 The first feature container that will be checked against the second, must be valid
		 * @param featureContainer1 The second feature container that will be checked against the first, must be valid
		 * @return True if the two feature containers are weakly identical, otherwise false
		 */
		typedef bool (*ValidateDeserializationCallback)(const std::shared_ptr<UnifiedFeatures::FeatureContainer>& featureContainer0, const std::shared_ptr<UnifiedFeatures::FeatureContainer>& featureContainer1);

	 public:

		/**
		 * Generic function to test the serialization of implementations of feature containers
		 * @param testDuration Number of seconds for this test, with range (0, infinity)
		 * @param createFeatureContainerCallback Callback function that generates a specialized instance of a feature container with specified number of (random) features, cf. `TestFeatureContainer::CreateFeatureContainerCallback`, must be valid
		 * @param validateDeserializationCallback Callback function that checks if a pair of feature container are weakly identical, cf. `TestFeatureContainer::ValidateDeserializationCallback`, must be valid
		 * @param numberFeaturesPerformance Optional number of features that should be used to measure the performance of the serialization, range: [1, infinity)
		 * @return True if the serialization test and validation was successful, otherwise false
		 */
		static bool testSerialization(const double testDuration, CreateFeatureContainerCallback createFeatureContainerCallback, ValidateDeserializationCallback validateDeserializationCallback, const size_t numberFeaturesPerformance = 1000);
};

} // namespace TestUnifiedFeatures

} // namespace Test

} // namespace Ocean
