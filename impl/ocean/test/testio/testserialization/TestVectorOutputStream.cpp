/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestVectorOutputStream.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include <cstring>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

bool TestVectorOutputStream::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("VectorOutputStream test");

	Log::info() << " ";

	if (selector.shouldRun("basicwrite"))
	{
		testResult = testBasicWrite(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("tellposition"))
	{
		testResult = testTellPosition(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("seekabsolute"))
	{
		testResult = testSeekAbsolute(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("seekrelative"))
	{
		testResult = testSeekRelative(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("writeandseek"))
	{
		testResult = testWriteAndSeek(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("overwrite"))
	{
		testResult = testOverwrite(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(VectorOutputStream, BasicWrite)
{
	EXPECT_TRUE(TestVectorOutputStream::testBasicWrite(GTEST_TEST_DURATION));
}

TEST(VectorOutputStream, TellPosition)
{
	EXPECT_TRUE(TestVectorOutputStream::testTellPosition(GTEST_TEST_DURATION));
}

TEST(VectorOutputStream, SeekAbsolute)
{
	EXPECT_TRUE(TestVectorOutputStream::testSeekAbsolute(GTEST_TEST_DURATION));
}

TEST(VectorOutputStream, SeekRelative)
{
	EXPECT_TRUE(TestVectorOutputStream::testSeekRelative(GTEST_TEST_DURATION));
}

TEST(VectorOutputStream, WriteAndSeek)
{
	EXPECT_TRUE(TestVectorOutputStream::testWriteAndSeek(GTEST_TEST_DURATION));
}

TEST(VectorOutputStream, Overwrite)
{
	EXPECT_TRUE(TestVectorOutputStream::testOverwrite(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestVectorOutputStream::testBasicWrite(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Basic write test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Serialization::VectorOutputStream stream;

		// Test writing individual bytes
		const uint8_t byteValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		stream << byteValue;

		OCEAN_EXPECT_EQUAL(validation, stream.size(), size_t(1));

		// Test writing strings
		const std::string testString = "Hello World!";
		stream << testString;

		OCEAN_EXPECT_GREATER(validation, stream.size(), testString.size());

		// Test writing integers
		const int32_t intValue = int32_t(RandomI::random32(randomGenerator));
		stream << intValue;

		const size_t finalSize = stream.size();
		OCEAN_EXPECT_GREATER(validation, finalSize, size_t(0));

		// Verify data is accessible
		const void* data = stream.data();
		OCEAN_EXPECT_TRUE(validation, data != nullptr);

		// Test clear
		stream.clear();
		OCEAN_EXPECT_EQUAL(validation, stream.size(), size_t(0));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestVectorOutputStream::testTellPosition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "tellp() position tracking test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Serialization::VectorOutputStream stream;

		// Initial position should be 0
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(0));

		// Write some data and check position
		const std::string testData = "Test";
		stream << testData;

		const std::streampos posAfterWrite = stream.tellp();
		OCEAN_EXPECT_GREATER(validation, posAfterWrite, std::streampos(0));

		// Write more data
		const int value = 42;
		stream << value;

		const std::streampos posAfterSecondWrite = stream.tellp();
		OCEAN_EXPECT_GREATER(validation, posAfterSecondWrite, posAfterWrite);

		// Clear and check position
		stream.clear();
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(0));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestVectorOutputStream::testSeekAbsolute(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Absolute seek test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Serialization::VectorOutputStream stream;

		// Write initial data
		const std::string initialData = "0123456789ABCDEF";
		stream.write(initialData.c_str(), initialData.size());

		const size_t dataSize = stream.size();
		OCEAN_EXPECT_EQUAL(validation, dataSize, initialData.size());

		// Seek to beginning
		stream.seekp(0);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(0));

		// Seek to middle
		const size_t middlePosition = dataSize / 2;
		stream.seekp(middlePosition);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(middlePosition));

		// Seek to end
		stream.seekp(dataSize);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(dataSize));

		// Seek beyond current size (should work)
		const size_t beyondSize = dataSize + 10;
		stream.seekp(beyondSize);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(beyondSize));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestVectorOutputStream::testSeekRelative(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Relative seek test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Serialization::VectorOutputStream stream;

		// Write test data
		const std::string testData = "0123456789";
		stream.write(testData.c_str(), testData.size());

		const size_t dataSize = stream.size();

		// Test seeking from beginning
		stream.seekp(5, std::ios::beg);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(5));

		// Test seeking from current position (forward)
		stream.seekp(2, std::ios::cur);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(7));

		// Test seeking from current position (backward)
		stream.seekp(-3, std::ios::cur);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(4));

		// Test seeking from end
		stream.seekp(0, std::ios::end);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(dataSize));

		stream.seekp(-5, std::ios::end);
		OCEAN_EXPECT_EQUAL(validation, stream.tellp(), std::streampos(dataSize - 5));

		// Test seeking before beginning (should fail)
		stream.seekp(0, std::ios::beg);
		stream.seekp(-1, std::ios::cur);

		// After an invalid seek, the stream position should remain at 0
		// and the stream should be in a fail state
		const bool streamOk = stream.good();
		OCEAN_EXPECT_FALSE(validation, streamOk);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestVectorOutputStream::testWriteAndSeek(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Combined write and seek test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Serialization::VectorOutputStream stream;

		// Write initial data
		const std::string part1 = "AAAA";
		stream.write(part1.c_str(), part1.size());

		const std::streampos pos1 = stream.tellp();
		OCEAN_EXPECT_EQUAL(validation, pos1, std::streampos(4));

		// Write more data
		const std::string part2 = "BBBB";
		stream.write(part2.c_str(), part2.size());

		const std::streampos pos2 = stream.tellp();
		OCEAN_EXPECT_EQUAL(validation, pos2, std::streampos(8));

		// Seek back and write
		stream.seekp(2);
		const std::string part3 = "CC";
		stream.write(part3.c_str(), part3.size());

		const std::streampos pos3 = stream.tellp();
		OCEAN_EXPECT_EQUAL(validation, pos3, std::streampos(4));

		// Seek to end and write
		stream.seekp(0, std::ios::end);
		const std::string part4 = "DD";
		stream.write(part4.c_str(), part4.size());

		const size_t finalSize = stream.size();
		OCEAN_EXPECT_EQUAL(validation, finalSize, size_t(10));

		// Verify content: should be "AACCBBBBDD"
		const char* data = static_cast<const char*>(stream.data());
		OCEAN_EXPECT_TRUE(validation, data != nullptr);

		if (data != nullptr)
		{
			const std::string result(data, finalSize);
			OCEAN_EXPECT_EQUAL(validation, result, std::string("AACCBBBBDD"));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestVectorOutputStream::testOverwrite(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Overwrite test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Serialization::VectorOutputStream stream;

		// Write initial data
		const std::string original = "XXXXXXXXXXXX";
		stream.write(original.c_str(), original.size());

		const size_t originalSize = stream.size();
		OCEAN_EXPECT_EQUAL(validation, originalSize, original.size());

		// Overwrite at beginning
		stream.seekp(0);
		const std::string overwrite1 = "AAA";
		stream.write(overwrite1.c_str(), overwrite1.size());

		OCEAN_EXPECT_EQUAL(validation, stream.size(), originalSize);

		// Overwrite in middle
		stream.seekp(6);
		const std::string overwrite2 = "BBB";
		stream.write(overwrite2.c_str(), overwrite2.size());

		OCEAN_EXPECT_EQUAL(validation, stream.size(), originalSize);

		// Verify final content: "AAAXXXBBBXXX"
		const char* data = static_cast<const char*>(stream.data());
		OCEAN_EXPECT_TRUE(validation, data != nullptr);

		if (data != nullptr)
		{
			const std::string result(data, originalSize);
			OCEAN_EXPECT_EQUAL(validation, result, std::string("AAAXXXBBBXXX"));
		}

		// Test overwriting beyond original size
		stream.seekp(originalSize + 5);
		const std::string overwrite3 = "CCC";
		stream.write(overwrite3.c_str(), overwrite3.size());

		const size_t newSize = stream.size();
		OCEAN_EXPECT_EQUAL(validation, newSize, originalSize + 5 + 3);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
