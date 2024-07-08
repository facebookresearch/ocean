/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestBitstream.h"

#include "ocean/io/Bitstream.h"

#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestBitstream::test(const double /*testDuration*/)
{
	Log::info() << "Bitstream test:";

	bool allSucceeded = true;

	allSucceeded = testInputOutputBitstream() && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Bitstream test suceeded.";
	}
	else
	{
		Log::info() << "Bitstream test FAILED!";
	}

	return allSucceeded;
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

	bool result = true;

	// bool (1 byte)
	result = outputBitstream.write<bool>(true) && result;
	ocean_assert(result);

	result = outputBitstream.write<bool>(false) && result;
	ocean_assert(result);

	// char (1 byte)
	result = outputBitstream.write<char>(15) && result;
	ocean_assert(result);

	result = outputBitstream.write<char>(-7) && result;
	ocean_assert(result);

	// unsigned char (1 byte)
	result = outputBitstream.write<unsigned char>(15) && result;
	ocean_assert(result);

	// wchar_t (4 byte)
	result = outputBitstream.write<wchar_t>(15) && result;
	ocean_assert(result);

	result = outputBitstream.write<wchar_t>(wchar_t(-7)) && result;
	ocean_assert(result);

	// short (2 byte)
	result = outputBitstream.write<short>(13000) && result;
	ocean_assert(result);

	result = outputBitstream.write<short>(-1001) && result;
	ocean_assert(result);

	// unsigned short (2 byte)
	result = outputBitstream.write<unsigned short>(65535u) && result;
	ocean_assert(result);

	// int (4 byte)
	result = outputBitstream.write<int>(2147483641) && result;
	ocean_assert(result);

	result = outputBitstream.write<int>(-2147483641) && result;
	ocean_assert(result);

	// unsigned int (4 byte)
	result = outputBitstream.write<unsigned int>(4294967295u) && result;
	ocean_assert(result);

	// float (4 byte)
	result = outputBitstream.write<float>(5.7f) && result;
	ocean_assert(result);

	/// double (8 byte)
	result = outputBitstream.write<double>(5.71) && result;
	ocean_assert(result);

	// long long (8 byte)
	result = outputBitstream.write<long long>(9223372036854775807ll) && result;
	ocean_assert(result);

	result = outputBitstream.write<long long>(-9223372036854775807ll) && result;
	ocean_assert(result);

	// unsigned long long (8 byte)
	result = outputBitstream.write<unsigned long long>(18446744073709551615ull) && result;
	ocean_assert(result);

	ocean_assert(outputBitstream.size() == 67);

	// string (4 byte + n * 1 byte)
	result = outputBitstream.write<std::string>(">< this is a test ><") && result;
	ocean_assert(result);

	ocean_assert(outputBitstream.size() == 67 + 20 * 1 + 4);

	// wstring (4 byte + n * 4 byte)
	result = outputBitstream.write<std::wstring>(L">< this is a test ><") && result;
	ocean_assert(result);

	ocean_assert(outputBitstream.size() == 91 + 20 * 4 + 4);

	const unsigned long long streamSize = outputBitstream.size();
	ocean_assert(streamSize == 175ull);

	if (streamSize != 175ull)
	{
		result = false;
	}

	const std::string outputString(output.str());

	Log::info() << "Input stream test:";

	std::istringstream input(outputString);
	IO::InputBitstream inputStream(input);

	// bool (1 byte)
	result = readValue<bool>(inputStream, true) && result;
	ocean_assert(result);

	result = readValue<bool>(inputStream, false) && result;
	ocean_assert(result);

	// char (1 byte)
	result = readValue<char>(inputStream, 15) && result;
	ocean_assert(result);

	// char (1 byte)
	result = readValue<char>(inputStream, -7) && result;
	ocean_assert(result);

	// unsigned char (1 byte)
	result = readValue<unsigned char>(inputStream, 15) && result;
	ocean_assert(result);

	// wchar_t (4 byte)
	result = readValue<wchar_t>(inputStream, 15) && result;
	ocean_assert(result);

	result = readValue<wchar_t>(inputStream, wchar_t(-7)) && result;
	ocean_assert(result);

	// short (2 byte)
	result = readValue<short>(inputStream, 13000) && result;
	ocean_assert(result);

	result = readValue<short>(inputStream, -1001) && result;
	ocean_assert(result);

	// unsigned short (2 byte)
	result = readValue<unsigned short>(inputStream, 65535u) && result;
	ocean_assert(result);

	// int (4 byte)
	result = readValue<int>(inputStream, 2147483641) && result;
	ocean_assert(result);

	result = readValue<int>(inputStream, -2147483641) && result;
	ocean_assert(result);

	// unsigned int (4 byte)
	result = readValue<unsigned int>(inputStream, 4294967295u) && result;
	ocean_assert(result);

	// float (4 byte)
	result = readValue<float>(inputStream, 5.7f) && result;
	ocean_assert(result);

	// double (8 byte)
	result = readValue<double>(inputStream, 5.71) && result;
	ocean_assert(result);

	// long long (8 byte)
	result = readValue<long long>(inputStream, 9223372036854775807ll) && result;
	ocean_assert(result);

	result = readValue<long long>(inputStream, -9223372036854775807ll) && result;
	ocean_assert(result);

	// unsigned long long (8 byte)
	result = readValue<unsigned long long>(inputStream, 18446744073709551615ull) && result;
	ocean_assert(result);

	// string (4 byte + n * 1 byte)
	result = readValue<std::string>(inputStream, ">< this is a test ><") && result;
	ocean_assert(result);

	// wstring (4 byte + n * 4 byte)
	result = readValue<std::wstring>(inputStream, L">< this is a test ><") && result;
	ocean_assert(result);

	if (result)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return result;
}

}

}

}
