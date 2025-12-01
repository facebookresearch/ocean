/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestInputDataSerializer.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/serialization/DataSample.h"
#include "ocean/io/serialization/OutputDataSerializer.h"

#include "ocean/math/Random.h"

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
class SimpleTestDataSampleInput : public IO::Serialization::DataSample
{
	public:

		/**
		 * Creates a new simple test data sample.
		 */
		SimpleTestDataSampleInput() = default;

		/**
		 * Creates a new simple test data sample with data.
		 * @param dataTimestamp The data timestamp
		 * @param payload The payload data
		 * @param sampleCreationTimestamp The sample creation timestamp
		 */
		explicit inline SimpleTestDataSampleInput(const IO::Serialization::DataTimestamp& dataTimestamp, const std::string& payload, const Timestamp sampleCreationTimestamp = Timestamp(true)) :
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
			static const std::string sampleType = "SimpleTestDataSampleInput";
			return sampleType;
		}

	protected:

		/// The payload data.
		std::string payload_;
};

bool TestInputDataSerializer::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("InputDataSerializer test");

	Log::info() << " ";

	if (selector.shouldRun("factoryfunction"))
	{
		testResult = testFactoryFunction(testDuration);

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

	if (selector.shouldRun("sample"))
	{
		testResult = testSample(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(InputDataSerializer, FactoryFunction)
{
	EXPECT_TRUE(TestInputDataSerializer::testFactoryFunction(GTEST_TEST_DURATION));
}

TEST(InputDataSerializer, StartStop)
{
	EXPECT_TRUE(TestInputDataSerializer::testStartStop(GTEST_TEST_DURATION));
}

TEST(InputDataSerializer, Sample)
{
	EXPECT_TRUE(TestInputDataSerializer::testSample(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestInputDataSerializer::testFactoryFunction(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Factory function test:";

	Validation validation;

	const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

	if (!scopedDirectory.exists())
	{
		OCEAN_EXPECT_TRUE(validation, false);
	}
	else
	{
		const std::string tempFilename = (scopedDirectory + IO::File("test_input.dat"))();

		IO::Serialization::FileInputDataSerializer serializer;
		OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

		// Register factory function
		const std::string sampleType = "SimpleTestDataSampleInput";

		const IO::Serialization::InputDataSerializer::FactoryFunction factoryFunction = [](const std::string&)
		{
			return std::make_unique<SimpleTestDataSampleInput>();
		};

		const bool registerSuccess = serializer.registerFactoryFunction(sampleType, factoryFunction);
		OCEAN_EXPECT_TRUE(validation, registerSuccess);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestInputDataSerializer::testStartStop(const double testDuration)
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

		const std::string tempFilename = (scopedDirectory + IO::File("test_input.dat"))();

		// Create a temporary file with data first
		{
			IO::Serialization::FileOutputDataSerializer outputSerializer;
			OCEAN_EXPECT_TRUE(validation, outputSerializer.setFilename(tempFilename));

			const IO::Serialization::DataSerializer::ChannelId channelId = outputSerializer.addChannel("SimpleTestDataSampleInput", "TestChannel", "TestContent");

			OCEAN_EXPECT_TRUE(validation, outputSerializer.start());

			const IO::Serialization::DataTimestamp dataTimestamp(0.0);
			IO::Serialization::UniqueDataSample sample = std::make_unique<SimpleTestDataSampleInput>(dataTimestamp, "TestData");

			outputSerializer.addSample(channelId, std::move(sample));

			OCEAN_EXPECT_TRUE(validation, outputSerializer.stopAndWait(10.0));
		}

		// Now test the input serializer
		IO::Serialization::FileInputDataSerializer serializer;
		OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

		const IO::Serialization::InputDataSerializer::FactoryFunction factoryFunction = [](const std::string&)
		{
			return std::make_unique<SimpleTestDataSampleInput>();
		};

		OCEAN_EXPECT_TRUE(validation, serializer.registerFactoryFunction("SimpleTestDataSampleInput", factoryFunction));

		// Initialize the serializer
		OCEAN_EXPECT_TRUE(validation, serializer.initialize());

		// After initialize(), serializer should not be started yet
		OCEAN_EXPECT_FALSE(validation, serializer.isStarted());
		OCEAN_EXPECT_TRUE(validation, serializer.hasStopped());

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

bool TestInputDataSerializer::testSample(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sample retrieval test:";

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

		const std::string tempFilename = (scopedDirectory + IO::File("test_sample.dat"))();

		// Create a temporary file with data first
		const unsigned int numSamples = RandomI::random(randomGenerator, 3u, 10u);
		std::vector<std::string> expectedPayloads;
		expectedPayloads.reserve(numSamples);

		{
			IO::Serialization::FileOutputDataSerializer outputSerializer;
			OCEAN_EXPECT_TRUE(validation, outputSerializer.setFilename(tempFilename));

			const IO::Serialization::DataSerializer::ChannelId channelId = outputSerializer.addChannel("SimpleTestDataSampleInput", "TestChannel", "TestContent");
			OCEAN_EXPECT_NOT_EQUAL(validation, channelId, IO::Serialization::DataSerializer::invalidChannelId());

			OCEAN_EXPECT_TRUE(validation, outputSerializer.start());

			for (unsigned int i = 0u; i < numSamples; ++i)
			{
				const IO::Serialization::DataTimestamp dataTimestamp{double(i) * 0.1};
				const std::string payload = "Sample_" + String::toAString(i);
				expectedPayloads.push_back(payload);

				IO::Serialization::UniqueDataSample sample = std::make_unique<SimpleTestDataSampleInput>(dataTimestamp, payload);

				outputSerializer.addSample(channelId, std::move(sample));
			}

			OCEAN_EXPECT_TRUE(validation, outputSerializer.stopAndWait(10.0));
		}

		// Now test reading samples with InputDataSerializer
		IO::Serialization::FileInputDataSerializer serializer;
		OCEAN_EXPECT_TRUE(validation, serializer.setFilename(tempFilename));

		const IO::Serialization::InputDataSerializer::FactoryFunction factoryFunction = [](const std::string&)
		{
			return std::make_unique<SimpleTestDataSampleInput>();
		};

		OCEAN_EXPECT_TRUE(validation, serializer.registerFactoryFunction("SimpleTestDataSampleInput", factoryFunction));

		// Initialize and start the serializer
		OCEAN_EXPECT_TRUE(validation, serializer.initialize());
		OCEAN_EXPECT_TRUE(validation, serializer.start());

		// Wait a bit for samples to be read into the queue
		Thread::sleep(100u);

		// Retrieve samples with speed = 0.0 (no timing, should return immediately)
		for (unsigned int i = 0u; i < numSamples; ++i)
		{
			IO::Serialization::DataSerializer::ChannelId retrievedChannelId = IO::Serialization::DataSerializer::invalidChannelId();
			IO::Serialization::UniqueDataSample retrievedSample = serializer.sample(retrievedChannelId, 0.0);

			OCEAN_EXPECT_TRUE(validation, retrievedSample != nullptr);
			OCEAN_EXPECT_NOT_EQUAL(validation, retrievedChannelId, IO::Serialization::DataSerializer::invalidChannelId());

			if (retrievedSample)
			{
				// Downcast to our test sample type
				SimpleTestDataSampleInput* testSample = dynamic_cast<SimpleTestDataSampleInput*>(retrievedSample.get());
				OCEAN_EXPECT_TRUE(validation, testSample != nullptr);

				if (testSample)
				{
					OCEAN_EXPECT_EQUAL(validation, testSample->payload(), expectedPayloads[i]);
					OCEAN_EXPECT_EQUAL(validation, testSample->dataTimestamp().asDouble(), double(i) * 0.1);
				}
			}
		}

		// After retrieving all samples, the queue should be empty
		IO::Serialization::DataSerializer::ChannelId dummyChannelId = IO::Serialization::DataSerializer::invalidChannelId();
		IO::Serialization::UniqueDataSample emptySample = serializer.sample(dummyChannelId, 0.0);
		OCEAN_EXPECT_TRUE(validation, emptySample == nullptr);

		// Stop the serializer (may already be stopped if thread finished reading the file)
		const bool stopResult = serializer.stop();
		if (!stopResult)
		{
			// If stop() returned false, verify that the serializer has already stopped
			OCEAN_EXPECT_TRUE(validation, serializer.hasStopped());
		}
		else
		{
			OCEAN_EXPECT_TRUE(validation, serializer.stopAndWait(10.0));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
