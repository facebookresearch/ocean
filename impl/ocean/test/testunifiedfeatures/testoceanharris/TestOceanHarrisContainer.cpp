// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testunifiedfeatures/testoceanharris/TestOceanHarrisContainer.h"

#include "ocean/test/testunifiedfeatures/TestFeatureContainer.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include "ocean/unifiedfeatures/oceanharris/OHFeatureContainer.h"

namespace Ocean
{

namespace Test
{

namespace TestUnifiedFeatures
{

namespace TestOceanHarris
{

using namespace Ocean::UnifiedFeatures;
using namespace Ocean::UnifiedFeatures::OceanHarris;

bool TestOceanHarrisContainer::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Ocean Harris Container test:   ---";
	Log::info() << " ";

	allSucceeded = testSerialization(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Ocean Harris Container test succeeded.";
	}
	else
	{
		Log::info() << "Ocean Harris Container test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestOceanHarrisContainer, Serialization)
{
	EXPECT_TRUE(TestOceanHarrisContainer::testSerialization(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestOceanHarrisContainer::testSerialization(const double testDuration)
{
	return TestFeatureContainer::testSerialization(testDuration, &createOceanHarrisContainer, &validateDeserialization);
}

std::shared_ptr<FeatureContainer> TestOceanHarrisContainer::createOceanHarrisContainer(RandomGenerator& randomGenerator, const size_t numberFeatures)
{
	if (numberFeatures == 0)
	{
		return std::make_shared<OHFeatureContainer>();
	}

	CV::Detector::HarrisCorners harrisCorners = generateRandomHarrisCorners(randomGenerator, numberFeatures);
	ocean_assert(harrisCorners.size() == numberFeatures);

	return std::make_shared<OHFeatureContainer>(std::move(harrisCorners));
}

bool TestOceanHarrisContainer::validateDeserialization(const std::shared_ptr<FeatureContainer>& featureContainer0, const std::shared_ptr<FeatureContainer>& featureContainer1)
{
	ocean_assert(featureContainer0 != featureContainer1);

	if (!featureContainer0 || !featureContainer1)
	{
		return false;
	}

	if (featureContainer0->libraryName() != featureContainer1->libraryName() || featureContainer0->name() != featureContainer1->name() || featureContainer0->objectVersion() != featureContainer1->objectVersion())
	{
		return false;
	}

	const OHFeatureContainer oceanHarrisContainer;
	if (featureContainer0->libraryName() != oceanHarrisContainer.libraryName() || featureContainer0->name() != oceanHarrisContainer.name() || featureContainer0->objectVersion() != oceanHarrisContainer.objectVersion())
	{
		return false;
	}

	const OHFeatureContainer* oceanHarrisContainer0 = dynamic_cast<const OHFeatureContainer*>(featureContainer0.get());
	const OHFeatureContainer* oceanHarrisContainer1 = dynamic_cast<const OHFeatureContainer*>(featureContainer1.get());

	if (oceanHarrisContainer0 == nullptr || oceanHarrisContainer1 == nullptr)
	{
		return false;
	}

	if (oceanHarrisContainer0->size() != oceanHarrisContainer1->size())
	{
		return false;
	}

	const CV::Detector::HarrisCorners& harrisCorners0 = oceanHarrisContainer0->harrisCorners();
	const CV::Detector::HarrisCorners& harrisCorners1 = oceanHarrisContainer1->harrisCorners();

	for (size_t i = 0; i < harrisCorners0.size(); ++i)
	{
		const Vector2& observation = harrisCorners0[i].observation();
		const Vector2& deserializedObservation = harrisCorners1[i].observation();

		// Require weak equality for float (because that's the precision used for the serialized data)
		if (!NumericF::isWeakEqual(float(observation.x()), float(deserializedObservation.x())) || !NumericF::isWeakEqual(float(observation.y()), float(deserializedObservation.y())) || !NumericF::isWeakEqual(float(harrisCorners0[i].strength()), float(harrisCorners1[i].strength())))
		{
			return false;
		}
	}

	return true;
}

CV::Detector::HarrisCorners TestOceanHarrisContainer::generateRandomHarrisCorners(RandomGenerator& randomGenerator, const size_t numberHarrisCorners)
{
	CV::Detector::HarrisCorners harrisCorners;
	harrisCorners.reserve(numberHarrisCorners);

	for (size_t i = 0; i < numberHarrisCorners; ++i)
	{
		const Scalar x = Random::scalar(randomGenerator, 0, Scalar(1920));
		const Scalar y = Random::scalar(randomGenerator, 0, Scalar(1080));
		const Scalar strength = Random::scalar(randomGenerator, 0, Scalar(1024));

		harrisCorners.emplace_back(Vector2(x, y), CV::Detector::PointFeature::DS_UNKNOWN, strength);
	}

	return harrisCorners;
}

} // namespace TestOceaHarris

} // namespace TestUnifiedFeatures

} // namespace Test

} // namespace Ocean
