/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestBitstream.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/io/Bitstream.h"

#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestBitstream::test(const double /*testDuration*/, const TestSelector& selector)
{
	TestResult testResult("Bitstream test");

	if (selector.shouldRun("inputoutputbitstream"))
	{
		testResult = testInputOutputBitstream();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(Bitstream, InputOutputBitstream)
{
	EXPECT_TRUE(TestBitstream::testInputOutputBitstream());
}

#endif // OCEAN_USE_GTEST

bool TestBitstream::testInputOutputBitstream()
{
	Log::info() << "Output stream test:";

	std::ostringstream output;
	IO::OutputBitstream outputBitstream(output);

	Validation validation;

	// bool (1 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<bool>(true));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<bool>(false));
	ocean_assert(validation.succeededSoFar());

	// char (1 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<char>(15));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<char>(-7));
	ocean_assert(validation.succeededSoFar());

	// unsigned char (1 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<unsigned char>(15));
	ocean_assert(validation.succeededSoFar());

	// wchar_t (4 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<wchar_t>(15));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<wchar_t>(wchar_t(-7)));
	ocean_assert(validation.succeededSoFar());

	// short (2 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<short>(13000));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<short>(-1001));
	ocean_assert(validation.succeededSoFar());

	// unsigned short (2 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<unsigned short>(65535u));
	ocean_assert(validation.succeededSoFar());

	// int (4 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<int>(2147483641));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<int>(-2147483641));
	ocean_assert(validation.succeededSoFar());

	// unsigned int (4 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<unsigned int>(4294967295u));
	ocean_assert(validation.succeededSoFar());

	// float (4 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<float>(5.7f));
	ocean_assert(validation.succeededSoFar());

	/// double (8 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<double>(5.71));
	ocean_assert(validation.succeededSoFar());

	// long long (8 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<long long>(9223372036854775807ll));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<long long>(-9223372036854775807ll));
	ocean_assert(validation.succeededSoFar());

	// unsigned long long (8 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<unsigned long long>(18446744073709551615ull));
	ocean_assert(validation.succeededSoFar());

	ocean_assert(outputBitstream.size() == 67);

	// string (4 byte + n * 1 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<std::string>(">< this is a test ><"));
	ocean_assert(validation.succeededSoFar());

	ocean_assert(outputBitstream.size() == 67 + 20 * 1 + 4);

	// wstring (4 byte + n * 4 byte)
	OCEAN_EXPECT_TRUE(validation, outputBitstream.write<std::wstring>(L">< this is a test ><"));
	ocean_assert(validation.succeededSoFar());

	ocean_assert(outputBitstream.size() == 91 + 20 * 4 + 4);

	const unsigned long long streamSize = outputBitstream.size();
	ocean_assert(streamSize == 175ull);

	OCEAN_EXPECT_EQUAL(validation, streamSize, 175ull);

	const std::string outputString(output.str());

	Log::info() << "Input stream test:";

	std::istringstream input(outputString);
	IO::InputBitstream inputStream(input);

	// bool (1 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<bool>(inputStream, true));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, readValue<bool>(inputStream, false));
	ocean_assert(validation.succeededSoFar());

	// char (1 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<char>(inputStream, 15));
	ocean_assert(validation.succeededSoFar());

	// char (1 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<char>(inputStream, -7));
	ocean_assert(validation.succeededSoFar());

	// unsigned char (1 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<unsigned char>(inputStream, 15));
	ocean_assert(validation.succeededSoFar());

	// wchar_t (4 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<wchar_t>(inputStream, 15));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, readValue<wchar_t>(inputStream, wchar_t(-7)));
	ocean_assert(validation.succeededSoFar());

	// short (2 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<short>(inputStream, 13000));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, readValue<short>(inputStream, -1001));
	ocean_assert(validation.succeededSoFar());

	// unsigned short (2 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<unsigned short>(inputStream, 65535u));
	ocean_assert(validation.succeededSoFar());

	// int (4 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<int>(inputStream, 2147483641));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, readValue<int>(inputStream, -2147483641));
	ocean_assert(validation.succeededSoFar());

	// unsigned int (4 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<unsigned int>(inputStream, 4294967295u));
	ocean_assert(validation.succeededSoFar());

	// float (4 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<float>(inputStream, 5.7f));
	ocean_assert(validation.succeededSoFar());

	// double (8 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<double>(inputStream, 5.71));
	ocean_assert(validation.succeededSoFar());

	// long long (8 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<long long>(inputStream, 9223372036854775807ll));
	ocean_assert(validation.succeededSoFar());

	OCEAN_EXPECT_TRUE(validation, readValue<long long>(inputStream, -9223372036854775807ll));
	ocean_assert(validation.succeededSoFar());

	// unsigned long long (8 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<unsigned long long>(inputStream, 18446744073709551615ull));
	ocean_assert(validation.succeededSoFar());

	// string (4 byte + n * 1 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<std::string>(inputStream, ">< this is a test ><"));
	ocean_assert(validation.succeededSoFar());

	// wstring (4 byte + n * 4 byte)
	OCEAN_EXPECT_TRUE(validation, readValue<std::wstring>(inputStream, L">< this is a test ><"));
	ocean_assert(validation.succeededSoFar());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
