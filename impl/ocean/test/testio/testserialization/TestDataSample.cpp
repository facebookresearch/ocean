/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestDataSample.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector3.h"

#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

/**
 * This class implements a test data sample for testing purposes.
 */
class TestDataSampleConcrete : public IO::Serialization::DataSample
{
	public:

		/**
		 * Creates a new test data sample.
		 */
		TestDataSampleConcrete() = default;

		/**
		 * Creates a new test data sample with a data timestamp.
		 * @param dataTimestamp The data timestamp
		 * @param sampleCreationTimestamp The sample creation timestamp
		 */
		explicit inline TestDataSampleConcrete(const IO::Serialization::DataTimestamp& dataTimestamp, const Timestamp sampleCreationTimestamp = Timestamp(true)) :
			DataSample(dataTimestamp, sampleCreationTimestamp)
		{
			// nothing to do here
		}

		/**
		 * Returns the type of the sample.
		 * @see DataSample::type().
		 */
		const std::string& type() const override
		{
			static const std::string sampleType = "TestDataSample";
			return sampleType;
		}
};

bool TestDataSample::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("DataSample test");

	Log::info() << " ";

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("playbacktimestamp"))
	{
		testResult = testPlaybackTimestamp(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("readwrite"))
	{
		testResult = testReadWrite(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(DataSample, Constructor)
{
	EXPECT_TRUE(TestDataSample::testConstructor(GTEST_TEST_DURATION));
}

TEST(DataSample, PlaybackTimestamp)
{
	EXPECT_TRUE(TestDataSample::testPlaybackTimestamp(GTEST_TEST_DURATION));
}

TEST(DataSample, ReadWrite)
{
	EXPECT_TRUE(TestDataSample::testReadWrite(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDataSample::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructor test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test default constructor
		TestDataSampleConcrete defaultSample;

		OCEAN_EXPECT_EQUAL(validation, defaultSample.type(), std::string("TestDataSample"));

		// Test constructor with double data timestamp
		const double doubleTimestampValue = RandomD::scalar(randomGenerator, 0.0, 1000.0);
		const IO::Serialization::DataTimestamp doubleDataTimestamp(doubleTimestampValue);

		const Timestamp creationTimestamp(true);
		const TestDataSampleConcrete doubleSample(doubleDataTimestamp, creationTimestamp);

		OCEAN_EXPECT_EQUAL(validation, doubleSample.type(), std::string("TestDataSample"));

		const IO::Serialization::DataTimestamp& retrievedDoubleTimestamp = doubleSample.dataTimestamp();
		OCEAN_EXPECT_TRUE(validation, retrievedDoubleTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, retrievedDoubleTimestamp.isDouble());
		OCEAN_EXPECT_EQUAL(validation, retrievedDoubleTimestamp.asDouble(), doubleTimestampValue);

		// Test constructor with int64 data timestamp
		const int64_t intTimestampValue = int64_t(RandomI::random32(randomGenerator));
		const IO::Serialization::DataTimestamp intDataTimestamp(intTimestampValue);

		const TestDataSampleConcrete intSample(intDataTimestamp);

		OCEAN_EXPECT_EQUAL(validation, intSample.type(), std::string("TestDataSample"));

		const IO::Serialization::DataTimestamp& retrievedIntTimestamp = intSample.dataTimestamp();
		OCEAN_EXPECT_TRUE(validation, retrievedIntTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, retrievedIntTimestamp.isInt());
		OCEAN_EXPECT_EQUAL(validation, retrievedIntTimestamp.asInt(), intTimestampValue);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataSample::testPlaybackTimestamp(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Playback timestamp test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const IO::Serialization::DataTimestamp dataTimestamp(RandomD::scalar(randomGenerator, 0.0, 1000.0));

		const Timestamp serializationStartTimestamp(true);

		// Wait a small amount of time
		Thread::sleep(1u);

		const Timestamp sampleCreationTimestamp(true);

		TestDataSampleConcrete sample(dataTimestamp, sampleCreationTimestamp);

		// Before configuration, playback timestamp should be minimum
		OCEAN_EXPECT_EQUAL(validation, sample.playbackTimestamp(), NumericD::minValue());

		// Configure the playback timestamp
		sample.configurePlaybackTimestamp(serializationStartTimestamp);

		const double playbackTimestamp = sample.playbackTimestamp();

		// Playback timestamp should be the time difference
		const double expectedPlaybackTimestamp = double(sampleCreationTimestamp - serializationStartTimestamp);

		OCEAN_EXPECT_TRUE(validation, NumericD::isEqual(playbackTimestamp, expectedPlaybackTimestamp, 0.001));
		OCEAN_EXPECT_TRUE(validation, playbackTimestamp >= 0.0);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataSample::testReadWrite(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Read/write test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Timestamp serializationStartTimestamp(true);

		Thread::sleep(50u);

		// Create an original sample with double data timestamp
		const double doubleTimestampValue = RandomD::scalar(randomGenerator, 0.0, 1000.0);
		const IO::Serialization::DataTimestamp doubleDataTimestamp(doubleTimestampValue);

		TestDataSampleConcrete originalSample(doubleDataTimestamp);
		originalSample.configurePlaybackTimestamp(serializationStartTimestamp);

		const double originalPlaybackTimestamp = originalSample.playbackTimestamp();

		// Write the sample
		std::ostringstream outputStream;
		IO::OutputBitstream outputBitstream(outputStream);

		const bool writeSuccess = originalSample.writeSample(outputBitstream);
		OCEAN_EXPECT_TRUE(validation, writeSuccess);

		if (writeSuccess)
		{
			// Read the sample back
			const std::string data = outputStream.str();
			std::istringstream inputStream(data);
			IO::InputBitstream inputBitstream(inputStream);

			TestDataSampleConcrete readSample;

			const bool readSuccess = readSample.readSample(inputBitstream);
			OCEAN_EXPECT_TRUE(validation, readSuccess);

			if (readSuccess)
			{
				// Verify the playback timestamp was preserved
				OCEAN_EXPECT_EQUAL(validation, readSample.playbackTimestamp(), originalPlaybackTimestamp);

				// Verify the data timestamp was preserved
				const IO::Serialization::DataTimestamp& readDataTimestamp = readSample.dataTimestamp();
				OCEAN_EXPECT_TRUE(validation, readDataTimestamp.isValid());
				OCEAN_EXPECT_TRUE(validation, readDataTimestamp.isDouble());
				OCEAN_EXPECT_EQUAL(validation, readDataTimestamp.asDouble(), doubleTimestampValue);
			}
		}

		// Test with int64 data timestamp
		const int64_t intTimestampValue = int64_t(RandomI::random32(randomGenerator));
		const IO::Serialization::DataTimestamp intDataTimestamp(intTimestampValue);

		TestDataSampleConcrete originalIntSample(intDataTimestamp);
		originalIntSample.configurePlaybackTimestamp(serializationStartTimestamp);

		std::ostringstream outputStreamInt;
		IO::OutputBitstream outputBitstreamInt(outputStreamInt);

		OCEAN_EXPECT_TRUE(validation, originalIntSample.writeSample(outputBitstreamInt));

		const std::string dataInt = outputStreamInt.str();
		std::istringstream inputStreamInt(dataInt);
		IO::InputBitstream inputBitstreamInt(inputStreamInt);

		TestDataSampleConcrete readIntSample;
		OCEAN_EXPECT_TRUE(validation, readIntSample.readSample(inputBitstreamInt));

		const IO::Serialization::DataTimestamp& readIntDataTimestamp = readIntSample.dataTimestamp();
		OCEAN_EXPECT_TRUE(validation, readIntDataTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, readIntDataTimestamp.isInt());
		OCEAN_EXPECT_EQUAL(validation, readIntDataTimestamp.asInt(), intTimestampValue);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
