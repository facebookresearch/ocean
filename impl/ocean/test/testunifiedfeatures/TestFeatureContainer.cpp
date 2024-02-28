// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testunifiedfeatures/TestFeatureContainer.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include <thrift/lib/cpp2/protocol/Serializer.h>

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace Test
{

namespace TestUnifiedFeatures
{

using namespace Ocean::UnifiedFeatures;

bool TestFeatureContainer::testSerialization(const double testDuration, CreateFeatureContainerCallback createFeatureContainerCallback, ValidateDeserializationCallback validateDeserializationCallback, const size_t numberFeaturesPerformance)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberFeaturesPerformance != 0);

	Log::info() << "Test serialization: ";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	const Timestamp startTimestamp(true);

	HighPerformanceStatistic performanceSerialization;
	HighPerformanceStatistic performanceDeserialization;
	HighPerformanceStatistic performanceDummy;

	do
	{
		for (const bool measurePerformance : {true, false})
		{
			const size_t numberFeatures = measurePerformance ? numberFeaturesPerformance : size_t(Random::random(randomGenerator, 2048u));

			const std::shared_ptr<FeatureContainer> featureContainer = createFeatureContainerCallback(randomGenerator, numberFeatures);
			ocean_assert(featureContainer && featureContainer->size() == numberFeatures);

			// Serialization
			Thrift::FeatureContainer thriftFeatureContainer;
			{
				HighPerformanceStatistic::ScopedStatistic scopedPerformance(measurePerformance ? performanceSerialization : performanceDummy);
				if (!featureContainer->toThrift(thriftFeatureContainer))
				{
					allSucceeded = false;
				}
			}

			const std::string buffer = apache::thrift::CompactSerializer::serialize<std::string>(thriftFeatureContainer);

			// Deserialization
			const Thrift::FeatureContainer deserializedThriftFeatureContainer = apache::thrift::CompactSerializer::deserialize<Thrift::FeatureContainer>(buffer);

			std::shared_ptr<FeatureContainer> deserializedFeatureContainer = createFeatureContainerCallback(randomGenerator, 0u);
			ocean_assert(deserializedFeatureContainer);
			{
				HighPerformanceStatistic::ScopedStatistic scopedPerformance(measurePerformance ? performanceDeserialization : performanceDummy);
				if (!deserializedFeatureContainer->fromThrift(deserializedThriftFeatureContainer))
				{
					allSucceeded = false;
				}
			}

			if (!validateDeserializationCallback(featureContainer, deserializedFeatureContainer))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance using " << numberFeaturesPerformance << " Harris corners (worst / best / average / median in ms)";
	if (performanceSerialization.measurements() != 0)
	{
		Log::info() << "  Serialization: " << String::toAString(performanceSerialization.worstMseconds(), 2u) << " / " << String::toAString(performanceSerialization.bestMseconds(), 2u) << " / " << String::toAString(performanceSerialization.averageMseconds(), 2u) << " / " << String::toAString(performanceSerialization.medianMseconds(), 2u);
	}
	else
	{
		Log::info() << "  Serialization: not enough data";
	}

	if (performanceDeserialization.measurements() != 0)
	{
		Log::info() << "  Deserialization: " << String::toAString(performanceDeserialization.worstMseconds(), 2u) << " / " << String::toAString(performanceDeserialization.bestMseconds(), 2u) << " / " << String::toAString(performanceDeserialization.averageMseconds(), 2u) << " / " << String::toAString(performanceDeserialization.medianMseconds(), 2u);
	}
	else
	{
		Log::info() << "  Deserialization: not enough data";
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestUnifiedFeatures

} // namespace Test

} // namespace Ocean
