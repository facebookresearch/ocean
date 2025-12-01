/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestDataSerializer.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/Random.h"

#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

bool TestDataSerializer::test(const double /*testDuration*/, const TestSelector& selector)
{
	TestResult testResult("DataSerializer test");

	Log::info() << " ";

	if (selector.shouldRun("channelconfiguration"))
	{
		testResult = testChannelConfiguration();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("channel"))
	{
		testResult = testChannel();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(DataSerializer, ChannelConfiguration)
{
	EXPECT_TRUE(TestDataSerializer::testChannelConfiguration());
}

TEST(DataSerializer, Channel)
{
	EXPECT_TRUE(TestDataSerializer::testChannel());
}

#endif // OCEAN_USE_GTEST

bool TestDataSerializer::testChannelConfiguration()
{
	Log::info() << "ChannelConfiguration test:";

	Validation validation;

	// Test default constructor
	const IO::Serialization::DataSerializer::ChannelConfiguration defaultConfig;

	OCEAN_EXPECT_FALSE(validation, defaultConfig.isValid());
	OCEAN_EXPECT_TRUE(validation, defaultConfig.sampleType().empty());
	OCEAN_EXPECT_TRUE(validation, defaultConfig.name().empty());
	OCEAN_EXPECT_TRUE(validation, defaultConfig.contentType().empty());

	// Test constructor with valid parameters
	const std::string sampleType = "TestSampleType";
	const std::string name = "TestChannel";
	const std::string contentType = "TestContent";

	const IO::Serialization::DataSerializer::ChannelConfiguration validConfig(sampleType, name, contentType);

	OCEAN_EXPECT_TRUE(validation, validConfig.isValid());
	OCEAN_EXPECT_EQUAL(validation, validConfig.sampleType(), sampleType);
	OCEAN_EXPECT_EQUAL(validation, validConfig.name(), name);
	OCEAN_EXPECT_EQUAL(validation, validConfig.contentType(), contentType);

	// Test equality operator
	const IO::Serialization::DataSerializer::ChannelConfiguration sameConfig(sampleType, name, contentType);
	OCEAN_EXPECT_TRUE(validation, validConfig == sameConfig);

	// Test inequality with different sample type
	const IO::Serialization::DataSerializer::ChannelConfiguration differentSampleType("DifferentType", name, contentType);
	OCEAN_EXPECT_FALSE(validation, validConfig == differentSampleType);

	// Test inequality with different name
	const IO::Serialization::DataSerializer::ChannelConfiguration differentName(sampleType, "DifferentName", contentType);
	OCEAN_EXPECT_FALSE(validation, validConfig == differentName);

	// Test inequality with different content type
	const IO::Serialization::DataSerializer::ChannelConfiguration differentContentType(sampleType, name, "DifferentContent");
	OCEAN_EXPECT_FALSE(validation, validConfig == differentContentType);

	// Test hash function
	const IO::Serialization::DataSerializer::ChannelConfiguration::Hash hashFunction;

	const size_t hash1 = hashFunction(validConfig);
	const size_t hash2 = hashFunction(sameConfig);
	const size_t hash3 = hashFunction(differentSampleType);

	OCEAN_EXPECT_EQUAL(validation, hash1, hash2);
	OCEAN_EXPECT_NOT_EQUAL(validation, hash1, hash3);

	// Test validation with empty fields
	const IO::Serialization::DataSerializer::ChannelConfiguration emptySampleType("", name, contentType);
	OCEAN_EXPECT_FALSE(validation, emptySampleType.isValid());

	const IO::Serialization::DataSerializer::ChannelConfiguration emptyName(sampleType, "", contentType);
	OCEAN_EXPECT_FALSE(validation, emptyName.isValid());

	const IO::Serialization::DataSerializer::ChannelConfiguration emptyContentType(sampleType, name, "");
	OCEAN_EXPECT_FALSE(validation, emptyContentType.isValid());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataSerializer::testChannel()
{
	Log::info() << "Channel test:";

	Validation validation;

	// Test default constructor
	const IO::Serialization::DataSerializer::Channel defaultChannel;

	OCEAN_EXPECT_FALSE(validation, defaultChannel.isValid());
	OCEAN_EXPECT_EQUAL(validation, defaultChannel.channelId(), IO::Serialization::DataSerializer::invalidChannelId());

	// Test constructor with configuration and channel ID
	const std::string sampleType = "TestSampleType";
	const std::string name = "TestChannel";
	const std::string contentType = "TestContent";

	const IO::Serialization::DataSerializer::ChannelConfiguration config(sampleType, name, contentType);
	const IO::Serialization::DataSerializer::ChannelId channelId = 42u;

	const IO::Serialization::DataSerializer::Channel channel(config, channelId);

	OCEAN_EXPECT_TRUE(validation, channel.isValid());
	OCEAN_EXPECT_EQUAL(validation, channel.channelId(), channelId);
	OCEAN_EXPECT_EQUAL(validation, channel.sampleType(), sampleType);
	OCEAN_EXPECT_EQUAL(validation, channel.name(), name);
	OCEAN_EXPECT_EQUAL(validation, channel.contentType(), contentType);

	// Test equality operator
	const IO::Serialization::DataSerializer::Channel sameChannel(config, channelId);
	OCEAN_EXPECT_TRUE(validation, channel == sameChannel);

	// Test inequality with different channel ID
	const IO::Serialization::DataSerializer::ChannelId differentChannelId = channelId + 1;
	const IO::Serialization::DataSerializer::Channel differentIdChannel(config, differentChannelId);
	OCEAN_EXPECT_FALSE(validation, channel == differentIdChannel);

	// Test inequality with different configuration
	const IO::Serialization::DataSerializer::ChannelConfiguration differentConfig("DifferentType", name, contentType);
	const IO::Serialization::DataSerializer::Channel differentConfigChannel(differentConfig, channelId);
	OCEAN_EXPECT_FALSE(validation, channel == differentConfigChannel);

	// Test hash function
	const IO::Serialization::DataSerializer::Channel::Hash hashFunction;

	const size_t hash1 = hashFunction(channel);
	const size_t hash2 = hashFunction(sameChannel);
	const size_t hash3 = hashFunction(differentIdChannel);

	OCEAN_EXPECT_EQUAL(validation, hash1, hash2);
	OCEAN_EXPECT_NOT_EQUAL(validation, hash1, hash3);

	// Test validation with invalid configuration
	const IO::Serialization::DataSerializer::ChannelConfiguration invalidConfig;
	const IO::Serialization::DataSerializer::Channel invalidConfigChannel(invalidConfig, channelId);
	OCEAN_EXPECT_FALSE(validation, invalidConfigChannel.isValid());

	// Test validation with invalid channel ID
	const IO::Serialization::DataSerializer::Channel invalidIdChannel(config, IO::Serialization::DataSerializer::invalidChannelId());
	OCEAN_EXPECT_FALSE(validation, invalidIdChannel.isValid());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
