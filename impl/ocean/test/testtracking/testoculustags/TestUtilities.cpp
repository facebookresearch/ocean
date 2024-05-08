/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/testoculustags/TestUtilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/testgeometry/Utilities.h"

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


bool TestUtilities::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   OculusTag test:   ---";
	Log::info() << " ";

	allSucceeded = testSerializeDeserializeOculusTags(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSerializeDeserializeTagSizeMap(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Utilities succeeded.";
	}
	else
	{
		Log::info() << "Utilities FAILED!";
	}

	return allSucceeded;
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
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
	 	const unsigned int numberTags = RandomI::random(randomGenerator, 0u, 100u);

	 	OculusTags tags;

		for (unsigned int i = 0u; i < numberTags; ++i)
		{
			const uint32_t tagID = RandomI::random(randomGenerator, 0u, 1023u);
			const OculusTag::ReflectanceType reflectanceType = RandomI::random(randomGenerator, 1u) == 0u ? OculusTag::RT_REFLECTANCE_NORMAL : OculusTag::RT_REFLECTANCE_INVERTED;
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
				allSucceeded = false;
			}
		}

		OculusTags deserializedTags;
		if (!Tracking::OculusTags::Utilities::deserializeOculusTags(buffer, deserializedTags))
		{
			// False is expected if buffer is empty
			if (!buffer.empty())
			{
				allSucceeded = false;
			}
		}

		if (tags.size() != deserializedTags.size())
		{
			allSucceeded = false;
		}
		else
		{
			for (size_t t = 0; t < deserializedTags.size(); ++t)
			{
				if (tags[t].tagID() != deserializedTags[t].tagID())
				{
					allSucceeded = false;
				}

				if (tags[t].reflectanceType() != deserializedTags[t].reflectanceType())
				{
					allSucceeded = false;
				}

				if (tags[t].intensityThreshold() != deserializedTags[t].intensityThreshold())
				{
					allSucceeded = false;
				}

				const HomogenousMatrix4& world_T_tag = tags[t].world_T_tag();
				const HomogenousMatrix4 deserialized_world_T_tag = deserializedTags[t].world_T_tag();
				for (unsigned int i = 0u; i < 16u; ++i)
				{
					if (!Numeric::isWeakEqual(world_T_tag[i], deserialized_world_T_tag[i]))
					{
						allSucceeded = false;
					}
				}

				if (!Numeric::isWeakEqual(tags[t].tagSize(), deserializedTags[t].tagSize()))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Serialize/deserialize Oculus Tags: succeeded.";
	}
	else
	{
		Log::info() << "Serialize/deserialize Oculus Tags: FAILED!";
	}

	return allSucceeded;
}

bool TestUtilities::testSerializeDeserializeTagSizeMap(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Serialize/Deserialize tag sizes test:";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar defaultTagSize = Random::scalar(randomGenerator, Scalar(0.01), Scalar(1));

		TagSizeMap tagSizeMap;
		const unsigned int numberEntries = RandomI::random(randomGenerator, 0u, 9u) == 0u ? 0u : RandomI::random(randomGenerator, 1u, 100u); // 10% chance of the value 0
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

		if (!Tracking::OculusTags::Utilities::serializeTagSizeMap(tagSizeMap, defaultTagSize, buffer))
		{
			allSucceeded = false;
		}

		Scalar deserializedDefaultTagSize;
		TagSizeMap deserializedTagSizeMap;

		if (!Tracking::OculusTags::Utilities::deserializeTagSizeMap(buffer, deserializedTagSizeMap, deserializedDefaultTagSize))
		{
			allSucceeded = false;
		}


		// Validation

		if (!Numeric::isWeakEqual(defaultTagSize, deserializedDefaultTagSize))
		{
			allSucceeded = false;
		}

		if (tagSizeMap.size() != deserializedTagSizeMap.size())
		{
			allSucceeded = false;
		}
		else
		{
			ocean_assert(tagSizeMap.size() == deserializedTagSizeMap.size());

			for (const TagSizeMap::value_type& tagSizeMapIter : tagSizeMap)
			{
				const TagSizeMap::const_iterator deserializedTagSizeMapIter = deserializedTagSizeMap.find(tagSizeMapIter.first);

				if (deserializedTagSizeMapIter == deserializedTagSizeMap.cend())
				{
					allSucceeded = false;
				}
				else
				{
					ocean_assert(tagSizeMapIter.first == deserializedTagSizeMapIter->first);

					if (!Numeric::isWeakEqual(tagSizeMapIter.second, deserializedTagSizeMapIter->second))
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Serialize/deserialize tag sizes: succeeded.";
	}
	else
	{
		Log::info() << "Serialize/deserialize tag sizes: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestTrackingOculusTag

} // namespace TestTracking

} // namespace Test

} // namespace Ocean
