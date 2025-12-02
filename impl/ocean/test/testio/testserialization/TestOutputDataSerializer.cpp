/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestOutputDataSerializer.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/serialization/DataSample.h"

#include "ocean/math/Random.h"

#include <fstream>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

/**
 * This class implements a simple test data sample for testing purposes.
 */
class SimpleTestDataSample : public IO::Serialization::DataSample
{
	public:

		/**
		 * Creates a new simple test data sample.
		 */
		SimpleTestDataSample() = default;

		/**
		 * Creates a new simple test data sample with data.
		 * @param dataTimestamp The data timestamp
		 * @param payload The payload data
		 * @param sampleCreationTimestamp The sample creation timestamp
		 */
		explicit inline SimpleTestDataSample(const IO::Serialization::DataTimestamp& dataTimestamp, const std::string& payload, const Timestamp sampleCreationTimestamp = Timestamp(true)) :
			DataSample(dataTimestamp, sampleCreationTimestamp),
			payload_(payload)
		{
			// nothing to do here
		}

		/**
		 * Returns the payload.
		 * @return The payload
		 */
		inline const std::string& payload() const
		{
			return payload_;
		}

		/**
		 * Reads the sample from an input bitstream.
		 * @see DataSample::readSample().
		 */
		bool readSample(IO::InputBitstream& inputBitstream) override
		{
			if (!DataSample::readSample(inputBitstream))
			{
				return false;
			}

			return inputBitstream.read<std::string>(payload_);
		}

		/**
		 * Writes the sample to an output bitstream.
		 * @see DataSample::writeSample().
		 */
		bool writeSample(IO::OutputBitstream& outputBitstream) const override
		{
			if (!DataSample::writeSample(outputBitstream))
			{
				return false;
			}

			return outputBitstream.write<std::string>(payload_);
		}

		/**
		 * Returns the type of the sample.
		 * @see DataSample::type().
		 */
		const std::string& type() const override
		{
			static const std::string sampleType = "SimpleTestDataSample";
			return sampleType;
		}

	protected:

		/// The payload data.
		std::string payload_;
};

bool TestOutputDataSerializer::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("OutputDataSerializer test");

	Log::info() << " ";

	if (selector.shouldRun("addchannel"))
	{
		testResult = testAddChannel(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("addsample"))
	{
		testResult = testAddSample(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("startstop"))
	{
		testResult = testStartStop(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("fileoutput"))
	{
		testResult = testFileOutput(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(OutputDataSerializer, AddChannel)
{
	EXPECT_TRUE(TestOutputDataSerializer::testAddChannel(GTEST_TEST_DURATION));
}

TEST(OutputDataSerializer, AddSample)
{
	EXPECT_TRUE(TestOutputDataSerializer::testAddSample(GTEST_TEST_DURATION));
}

TEST(OutputDataSerializer, StartStop)
{
	EXPECT_TRUE(TestOutputDataSerializer::testStartStop(GTEST_TEST_DURATION));
}

TEST(OutputDataSerializer, FileOutput)
{
	EXPECT_TRUE(TestOutputDataSerializer::testFileOutput(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestOutputDataSerializer::testAddChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Add channel test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

		if (!scopedDirectory.exists())
		{
			OCEAN_EXPECT_TRUE(validation, false);
			continue;
		}

		const std::string tempFilename = (scopedDirectory + IO::File("test_output.dat"))();

		IO::Serialization::FileOutputDataSerializer serializer;
		OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

		// Add first channel
		const std::string sampleType1 = "SampleType1";
		const std::string name1 = "Channel1";
		const std::string contentType1 = "Content1";

		const IO::Serialization::DataSerializer::ChannelId channelId1 = serializer.addChannel(sampleType1, name1, contentType1);
		OCEAN_EXPECT_NOT_EQUAL(validation, channelId1, IO::Serialization::DataSerializer::invalidChannelId());

		// Add second channel with different configuration
		const std::string sampleType2 = "SampleType2";
		const std::string name2 = "Channel2";
		const std::string contentType2 = "Content2";

		const IO::Serialization::DataSerializer::ChannelId channelId2 = serializer.addChannel(sampleType2, name2, contentType2);
		OCEAN_EXPECT_NOT_EQUAL(validation, channelId2, IO::Serialization::DataSerializer::invalidChannelId());
		OCEAN_EXPECT_NOT_EQUAL(validation, channelId1, channelId2);

		// Try to add duplicate channel (should fail)
		const IO::Serialization::DataSerializer::ChannelId duplicateChannelId = serializer.addChannel(sampleType1, name1, contentType1);
		OCEAN_EXPECT_EQUAL(validation, duplicateChannelId, IO::Serialization::DataSerializer::invalidChannelId());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOutputDataSerializer::testAddSample(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Add sample test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

		if (!scopedDirectory.exists())
		{
			OCEAN_EXPECT_TRUE(validation, false);
			continue;
		}

		const std::string tempFilename = (scopedDirectory + IO::File("test_output.dat"))();

		IO::Serialization::FileOutputDataSerializer serializer;
		OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

		// Add a channel
		const std::string sampleType = "SimpleTestDataSample";
		const std::string name = "TestChannel";
		const std::string contentType = "TestContent";

		const IO::Serialization::DataSerializer::ChannelId channelId = serializer.addChannel(sampleType, name, contentType);
		OCEAN_EXPECT_NOT_EQUAL(validation, channelId, IO::Serialization::DataSerializer::invalidChannelId());

		if (channelId != IO::Serialization::DataSerializer::invalidChannelId())
		{
			// Start the serializer
			OCEAN_EXPECT_TRUE(validation, serializer.start());

			// Add samples
			const unsigned int numSamples = RandomI::random(randomGenerator, 1u, 10u);

			for (unsigned int i = 0u; i < numSamples; ++i)
			{
				const IO::Serialization::DataTimestamp dataTimestamp{double(i)};
				const std::string payload = "Sample" + String::toAString(i);

				IO::Serialization::UniqueDataSample sample = std::make_unique<SimpleTestDataSample>(dataTimestamp, payload);

				const bool addSuccess = serializer.addSample(channelId, std::move(sample));
				OCEAN_EXPECT_TRUE(validation, addSuccess);
			}

			// Stop the serializer
			OCEAN_EXPECT_TRUE(validation, serializer.stopAndWait(10.0));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOutputDataSerializer::testStartStop(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Start/stop test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

		if (!scopedDirectory.exists())
		{
			OCEAN_EXPECT_TRUE(validation, false);
			continue;
		}

		const std::string tempFilename = (scopedDirectory + IO::File("test_output.dat"))();

		IO::Serialization::FileOutputDataSerializer serializer;
		OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

		// Initially, serializer should not be started
		OCEAN_EXPECT_FALSE(validation, serializer.isStarted());
		OCEAN_EXPECT_FALSE(validation, serializer.hasStopped());

		// Start the serializer
		OCEAN_EXPECT_TRUE(validation, serializer.start());
		OCEAN_EXPECT_TRUE(validation, serializer.isStarted());
		OCEAN_EXPECT_FALSE(validation, serializer.hasStopped());

		// Stop the serializer
		OCEAN_EXPECT_TRUE(validation, serializer.stop());

		// Wait for it to stop
		OCEAN_EXPECT_TRUE(validation, serializer.stopAndWait(10.0));
		OCEAN_EXPECT_FALSE(validation, serializer.isStarted());
		OCEAN_EXPECT_TRUE(validation, serializer.hasStopped());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOutputDataSerializer::testFileOutput(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "File output test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

		if (!scopedDirectory.exists())
		{
			OCEAN_EXPECT_TRUE(validation, false);
			continue;
		}

		const std::string tempFilename = (scopedDirectory + IO::File("test_output.dat"))();

		{
			IO::Serialization::FileOutputDataSerializer serializer;
			OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

			// Add a channel
			const std::string sampleType = "SimpleTestDataSample";
			const std::string name = "TestChannel";
			const std::string contentType = "TestContent";

			const IO::Serialization::DataSerializer::ChannelId channelId = serializer.addChannel(sampleType, name, contentType);
			OCEAN_EXPECT_NOT_EQUAL(validation, channelId, IO::Serialization::DataSerializer::invalidChannelId());

			if (channelId != IO::Serialization::DataSerializer::invalidChannelId())
			{
				// Start the serializer
				OCEAN_EXPECT_TRUE(validation, serializer.start());

				// Add some samples
				const unsigned int numSamples = 5u;

				for (unsigned int i = 0u; i < numSamples; ++i)
				{
					const IO::Serialization::DataTimestamp dataTimestamp{double(i)};
					const std::string payload = "TestData" + String::toAString(i);

					IO::Serialization::UniqueDataSample sample = std::make_unique<SimpleTestDataSample>(dataTimestamp, payload);

					OCEAN_EXPECT_TRUE(validation, serializer.addSample(channelId, std::move(sample)));
				}

				// Stop and wait for serializer to finish
				OCEAN_EXPECT_TRUE(validation, serializer.stopAndWait(10.0));
			}
		}

		// Verify file was created and has content
		OCEAN_EXPECT_TRUE(validation, IO::File(tempFilename).exists());

		std::ifstream testStream(tempFilename.c_str(), std::ios::binary | std::ios::ate);
		const size_t fileSize = size_t(testStream.tellg());

		OCEAN_EXPECT_TRUE(validation, fileSize > 0u);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
