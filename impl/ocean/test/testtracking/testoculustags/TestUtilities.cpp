/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/testoculustags/TestUtilities.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/test/Validation.h"

#include "ocean/tracking/oculustags/Utilities.h"

using namespace Ocean::Tracking::OculusTags;

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestOculusTags
{


bool TestUtilities::test(const double testDuration, Worker& /*worker*/, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("OculusTag test");
	Log::info() << " ";

	if (selector.shouldRun("serializedeserializeoculustags"))
	{
		testResult = testSerializeDeserializeOculusTags(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("serializedeserializetagsizemap"))
	{
		testResult = testSerializeDeserializeTagSizeMap(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestOculusTags, UtilitiesSerializeDeserializeOculusTags)
{
	Worker worker;
	EXPECT_TRUE(TestUtilities::testSerializeDeserializeOculusTags(GTEST_TEST_DURATION));
}

TEST(TestOculusTags, UtilitiesSerializeDeserializeTagSizeMap)
{
	Worker worker;
	EXPECT_TRUE(TestUtilities::testSerializeDeserializeTagSizeMap(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUtilities::testSerializeDeserializeOculusTags(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Serialize/Deserialize Oculus Tags test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
	 	const unsigned int numberTags = RandomI::random(randomGenerator, 0u, 100u);

	 	OculusTags tags;

		for (unsigned int i = 0u; i < numberTags; ++i)
		{
			const uint32_t tagID = RandomI::random(randomGenerator, 0u, 1023u);
			const OculusTag::ReflectanceType reflectanceType = RandomI::boolean(randomGenerator) ? OculusTag::RT_REFLECTANCE_NORMAL : OculusTag::RT_REFLECTANCE_INVERTED;
			const uint8_t intensityThreshold = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			const HomogenousMatrix4 world_T_tag(Random::vector3(randomGenerator), Random::rotation(randomGenerator));
			const Scalar tagSize = Random::scalar(randomGenerator, Scalar(0.01), Scalar(1));

			tags.emplace_back(tagID, reflectanceType, intensityThreshold, world_T_tag, tagSize);
			ocean_assert(tags.back().isValid());
		}

		std::string buffer;
		if (!Tracking::OculusTags::Utilities::serializeOculusTags(tags, buffer))
		{
			// False is expected if tags are empty
			if (!tags.empty())
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		OculusTags deserializedTags;
		if (!Tracking::OculusTags::Utilities::deserializeOculusTags(buffer, deserializedTags))
		{
			// False is expected if buffer is empty
			if (!buffer.empty())
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		if (tags.size() != deserializedTags.size())
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			for (size_t t = 0; t < deserializedTags.size(); ++t)
			{
				OCEAN_EXPECT_EQUAL(validation, tags[t].tagID(), deserializedTags[t].tagID());

				OCEAN_EXPECT_EQUAL(validation, tags[t].reflectanceType(), deserializedTags[t].reflectanceType());

				OCEAN_EXPECT_EQUAL(validation, tags[t].intensityThreshold(), deserializedTags[t].intensityThreshold());

				const HomogenousMatrix4& world_T_tag = tags[t].world_T_tag();
				const HomogenousMatrix4 deserialized_world_T_tag = deserializedTags[t].world_T_tag();
				for (unsigned int i = 0u; i < 16u; ++i)
				{
					OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(world_T_tag[i], deserialized_world_T_tag[i]));
				}

				OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(tags[t].tagSize(), deserializedTags[t].tagSize()));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Serialize/deserialize Oculus Tags: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testSerializeDeserializeTagSizeMap(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Serialize/Deserialize tag sizes test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar defaultTagSize = Random::scalar(randomGenerator, Scalar(0.01), Scalar(1));

		TagSizeMap tagSizeMap;
		const bool useZeroEntries = RandomI::random(randomGenerator, 0u, 9u) == 0u; // 10% chance of the value 0
		const unsigned int numberEntries = useZeroEntries ? 0u : RandomI::random(randomGenerator, 1u, 100u);
		bool isValidTestData = true;

		for (unsigned int i = 0u; i < numberEntries; ++i)
		{
			const Scalar tagSize = Random::scalar(randomGenerator, Scalar(0.01), Scalar(1));
			uint32_t tagID = 0u;

			do
			{
				tagID = RandomI::random(randomGenerator, 0u, 1023u);

				if (startTimestamp + testDuration > Timestamp(true))
				{
					isValidTestData = false;
					break;
				}
			}
			while (tagSizeMap.find(tagID) != tagSizeMap.cend());

			if (!isValidTestData)
			{
				break;
			}

			bool addedToMap;
			std::tie(std::ignore, addedToMap) = tagSizeMap.emplace(tagID, tagSize);

			if (!addedToMap)
			{
				isValidTestData = false;
				break;
			}
		}

		if (!isValidTestData)
		{
			continue;
		}

		std::string buffer;

		OCEAN_EXPECT_TRUE(validation, Tracking::OculusTags::Utilities::serializeTagSizeMap(tagSizeMap, defaultTagSize, buffer));

		Scalar deserializedDefaultTagSize;
		TagSizeMap deserializedTagSizeMap;

		OCEAN_EXPECT_TRUE(validation, Tracking::OculusTags::Utilities::deserializeTagSizeMap(buffer, deserializedTagSizeMap, deserializedDefaultTagSize));


		// Validation

		OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(defaultTagSize, deserializedDefaultTagSize));

		if (tagSizeMap.size() != deserializedTagSizeMap.size())
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			ocean_assert(tagSizeMap.size() == deserializedTagSizeMap.size());

			for (const TagSizeMap::value_type& tagSizeMapIter : tagSizeMap)
			{
				const TagSizeMap::const_iterator deserializedTagSizeMapIter = deserializedTagSizeMap.find(tagSizeMapIter.first);

				if (deserializedTagSizeMapIter == deserializedTagSizeMap.cend())
				{
					OCEAN_SET_FAILED(validation);
				}
				else
				{
					ocean_assert(tagSizeMapIter.first == deserializedTagSizeMapIter->first);

					OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(tagSizeMapIter.second, deserializedTagSizeMapIter->second));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Serialize/deserialize tag sizes: " << validation;

	return validation.succeeded();
}

} // namespace TestTrackingOculusTag

} // namespace TestTracking

} // namespace Test

} // namespace Ocean
