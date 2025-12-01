/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestDataTimestamp.h"

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

bool TestDataTimestamp::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("DataTimestamp test");

	Log::info() << " ";

	if (selector.shouldRun("constructordouble"))
	{
		testResult = testConstructorDouble(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("constructorint64"))
	{
		testResult = testConstructorInt64(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("typechecking"))
	{
		testResult = testTypeChecking(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("valueretrieval"))
	{
		testResult = testValueRetrieval(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("serialization"))
	{
		testResult = testSerialization(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("equality"))
	{
		testResult = testEquality(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(DataTimestamp, ConstructorDouble)
{
	EXPECT_TRUE(TestDataTimestamp::testConstructorDouble(GTEST_TEST_DURATION));
}

TEST(DataTimestamp, ConstructorInt64)
{
	EXPECT_TRUE(TestDataTimestamp::testConstructorInt64(GTEST_TEST_DURATION));
}

TEST(DataTimestamp, TypeChecking)
{
	EXPECT_TRUE(TestDataTimestamp::testTypeChecking(GTEST_TEST_DURATION));
}

TEST(DataTimestamp, ValueRetrieval)
{
	EXPECT_TRUE(TestDataTimestamp::testValueRetrieval(GTEST_TEST_DURATION));
}

TEST(DataTimestamp, Serialization)
{
	EXPECT_TRUE(TestDataTimestamp::testSerialization(GTEST_TEST_DURATION));
}

TEST(DataTimestamp, Equality)
{
	EXPECT_TRUE(TestDataTimestamp::testEquality(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDataTimestamp::testConstructorDouble(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructor with double value test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test with positive double value
		const double positiveValue = RandomD::scalar(randomGenerator, 0.0, 1000000.0);
		const IO::Serialization::DataTimestamp positiveTimestamp(positiveValue);

		OCEAN_EXPECT_TRUE(validation, positiveTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, positiveTimestamp.isDouble());
		OCEAN_EXPECT_FALSE(validation, positiveTimestamp.isInt());
		OCEAN_EXPECT_EQUAL(validation, positiveTimestamp.asDouble(), positiveValue);

		// Test with negative double value
		const double negativeValue = RandomD::scalar(randomGenerator, -1000000.0, 0.0);
		const IO::Serialization::DataTimestamp negativeTimestamp(negativeValue);

		OCEAN_EXPECT_TRUE(validation, negativeTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, negativeTimestamp.isDouble());
		OCEAN_EXPECT_FALSE(validation, negativeTimestamp.isInt());
		OCEAN_EXPECT_EQUAL(validation, negativeTimestamp.asDouble(), negativeValue);

		// Test with zero
		const IO::Serialization::DataTimestamp zeroTimestamp(0.0);

		OCEAN_EXPECT_TRUE(validation, zeroTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, zeroTimestamp.isDouble());
		OCEAN_EXPECT_EQUAL(validation, zeroTimestamp.asDouble(), 0.0);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataTimestamp::testConstructorInt64(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructor with int64 value test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test with positive int64 value
		const int64_t positiveValue = int64_t(RandomI::random64(randomGenerator) & 0x7FFFFFFFFFFFFFFFLL);
		const IO::Serialization::DataTimestamp positiveTimestamp(positiveValue);

		OCEAN_EXPECT_TRUE(validation, positiveTimestamp.isValid());
		OCEAN_EXPECT_FALSE(validation, positiveTimestamp.isDouble());
		OCEAN_EXPECT_TRUE(validation, positiveTimestamp.isInt());
		OCEAN_EXPECT_EQUAL(validation, positiveTimestamp.asInt(), positiveValue);

		// Test with negative int64 value
		const int64_t negativeValue = -int64_t(RandomI::random64(randomGenerator) & 0x7FFFFFFFFFFFFFFFLL);
		const IO::Serialization::DataTimestamp negativeTimestamp(negativeValue);

		OCEAN_EXPECT_TRUE(validation, negativeTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, negativeTimestamp.isInt());
		OCEAN_EXPECT_FALSE(validation, negativeTimestamp.isDouble());
		OCEAN_EXPECT_EQUAL(validation, negativeTimestamp.asInt(), negativeValue);

		// Test with zero
		const IO::Serialization::DataTimestamp zeroTimestamp(int64_t(0));

		OCEAN_EXPECT_TRUE(validation, zeroTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, zeroTimestamp.isInt());
		OCEAN_EXPECT_EQUAL(validation, zeroTimestamp.asInt(), int64_t(0));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataTimestamp::testTypeChecking(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Type checking test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test invalid timestamp (default constructed)
		IO::Serialization::DataTimestamp invalidTimestamp;

		OCEAN_EXPECT_FALSE(validation, invalidTimestamp.isValid());
		OCEAN_EXPECT_FALSE(validation, invalidTimestamp.isDouble());
		OCEAN_EXPECT_FALSE(validation, invalidTimestamp.isInt());

		// Test double timestamp
		const double doubleValue = RandomD::scalar(randomGenerator, -1000.0, 1000.0);
		const IO::Serialization::DataTimestamp doubleTimestamp(doubleValue);

		OCEAN_EXPECT_TRUE(validation, doubleTimestamp.isValid());
		OCEAN_EXPECT_TRUE(validation, doubleTimestamp.isDouble());
		OCEAN_EXPECT_FALSE(validation, doubleTimestamp.isInt());

		// Test int64 timestamp
		const int64_t intValue = int64_t(RandomI::random32(randomGenerator));
		const IO::Serialization::DataTimestamp intTimestamp(intValue);

		OCEAN_EXPECT_TRUE(validation, intTimestamp.isValid());
		OCEAN_EXPECT_FALSE(validation, intTimestamp.isDouble());
		OCEAN_EXPECT_TRUE(validation, intTimestamp.isInt());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataTimestamp::testValueRetrieval(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Value retrieval test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test asDouble() with double timestamp
		const double doubleValue = RandomD::scalar(randomGenerator, -1000.0, 1000.0);
		const IO::Serialization::DataTimestamp doubleTimestamp(doubleValue);

		OCEAN_EXPECT_EQUAL(validation, doubleTimestamp.asDouble(), doubleValue);
		OCEAN_EXPECT_EQUAL(validation, doubleTimestamp.forceDouble(), doubleValue);

		// Test asInt() with int64 timestamp
		const int64_t intValue = int64_t(RandomI::random32(randomGenerator));
		const IO::Serialization::DataTimestamp intTimestamp(intValue);

		OCEAN_EXPECT_EQUAL(validation, intTimestamp.asInt(), intValue);
		OCEAN_EXPECT_EQUAL(validation, intTimestamp.forceDouble(), double(intValue));

		// Test forceDouble() conversion from int64
		const int64_t largeIntValue = int64_t(RandomI::random64(randomGenerator) & 0x7FFFFFFFFFFFFFFFLL);
		const IO::Serialization::DataTimestamp largeIntTimestamp(largeIntValue);

		const double forcedDouble = largeIntTimestamp.forceDouble();
		OCEAN_EXPECT_EQUAL(validation, forcedDouble, double(largeIntValue));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataTimestamp::testSerialization(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Serialization test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test serialization with double timestamp
		{
			const double originalValue = RandomD::scalar(randomGenerator, -1000.0, 1000.0);
			const IO::Serialization::DataTimestamp originalTimestamp(originalValue);

			std::ostringstream outputStream;
			IO::OutputBitstream outputBitstream(outputStream);

			const bool writeSuccess = IO::Serialization::DataTimestamp::write(outputBitstream, originalTimestamp);
			OCEAN_EXPECT_TRUE(validation, writeSuccess);

			if (writeSuccess)
			{
				const std::string data = outputStream.str();
				std::istringstream inputStream(data);
				IO::InputBitstream inputBitstream(inputStream);

				IO::Serialization::DataTimestamp readTimestamp;

				const bool readSuccess = IO::Serialization::DataTimestamp::read(inputBitstream, readTimestamp);
				OCEAN_EXPECT_TRUE(validation, readSuccess);

				if (readSuccess)
				{
					OCEAN_EXPECT_TRUE(validation, readTimestamp.isValid());
					OCEAN_EXPECT_TRUE(validation, readTimestamp.isDouble());
					OCEAN_EXPECT_EQUAL(validation, readTimestamp.asDouble(), originalValue);
					OCEAN_EXPECT_TRUE(validation, readTimestamp == originalTimestamp);
				}
			}
		}

		// Test serialization with int64 timestamp
		{
			const int64_t originalValue = int64_t(RandomI::random32(randomGenerator));
			const IO::Serialization::DataTimestamp originalTimestamp(originalValue);

			std::ostringstream outputStream;
			IO::OutputBitstream outputBitstream(outputStream);

			const bool writeSuccess = IO::Serialization::DataTimestamp::write(outputBitstream, originalTimestamp);
			OCEAN_EXPECT_TRUE(validation, writeSuccess);

			if (writeSuccess)
			{
				const std::string data = outputStream.str();
				std::istringstream inputStream(data);
				IO::InputBitstream inputBitstream(inputStream);

				IO::Serialization::DataTimestamp readTimestamp;

				const bool readSuccess = IO::Serialization::DataTimestamp::read(inputBitstream, readTimestamp);
				OCEAN_EXPECT_TRUE(validation, readSuccess);

				if (readSuccess)
				{
					OCEAN_EXPECT_TRUE(validation, readTimestamp.isValid());
					OCEAN_EXPECT_TRUE(validation, readTimestamp.isInt());
					OCEAN_EXPECT_EQUAL(validation, readTimestamp.asInt(), originalValue);
					OCEAN_EXPECT_TRUE(validation, readTimestamp == originalTimestamp);
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataTimestamp::testEquality(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Equality test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test equality with same double values
		const double doubleValue = RandomD::scalar(randomGenerator, -1000.0, 1000.0);
		const IO::Serialization::DataTimestamp doubleTimestamp1(doubleValue);
		const IO::Serialization::DataTimestamp doubleTimestamp2(doubleValue);

		OCEAN_EXPECT_TRUE(validation, doubleTimestamp1 == doubleTimestamp2);

		// Test inequality with different double values
		const double differentDoubleValue = doubleValue + 1.0;
		const IO::Serialization::DataTimestamp differentDoubleTimestamp(differentDoubleValue);

		OCEAN_EXPECT_FALSE(validation, doubleTimestamp1 == differentDoubleTimestamp);

		// Test equality with same int64 values
		const int64_t intValue = int64_t(RandomI::random32(randomGenerator));
		const IO::Serialization::DataTimestamp intTimestamp1(intValue);
		const IO::Serialization::DataTimestamp intTimestamp2(intValue);

		OCEAN_EXPECT_TRUE(validation, intTimestamp1 == intTimestamp2);

		// Test inequality with different int64 values
		const int64_t differentIntValue = intValue + 1;
		const IO::Serialization::DataTimestamp differentIntTimestamp(differentIntValue);

		OCEAN_EXPECT_FALSE(validation, intTimestamp1 == differentIntTimestamp);

		// Test inequality between double and int64 timestamps (even if numerically equal)
		const double numericValue = 42.0;
		const int64_t sameNumericValue = 42;

		const IO::Serialization::DataTimestamp doubleFortyTwo(numericValue);
		const IO::Serialization::DataTimestamp intFortyTwo(sameNumericValue);

		OCEAN_EXPECT_FALSE(validation, doubleFortyTwo == intFortyTwo);

		// Test invalid timestamps
		IO::Serialization::DataTimestamp invalidTimestamp1;
		IO::Serialization::DataTimestamp invalidTimestamp2;

		OCEAN_EXPECT_TRUE(validation, invalidTimestamp1 == invalidTimestamp2);
		OCEAN_EXPECT_FALSE(validation, invalidTimestamp1 == doubleTimestamp1);
		OCEAN_EXPECT_FALSE(validation, invalidTimestamp1 == intTimestamp1);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
