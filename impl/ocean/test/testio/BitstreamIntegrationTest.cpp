/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Bitstream.h"

#ifdef OCEAN_USE_GTEST

#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>
#include <string>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace
{

// Helper: Populate an output stream with a mixed payload and return the
// resulting byte buffer as a string suitable for feeding InputBitstream.
std::string makeMixedPayload()
{
	std::ostringstream output;
	IO::OutputBitstream out(output);

	EXPECT_TRUE(out.write<unsigned int>(0xDEADBEEFu));
	EXPECT_TRUE(out.write<int>(-42));
	EXPECT_TRUE(out.write<double>(3.1415926535));
	EXPECT_TRUE(out.write<std::string>(std::string("hello world")));
	EXPECT_TRUE(out.write<std::wstring>(std::wstring(L"wchr")));

	return output.str();
}

} // namespace

// InputBitstream <-> OutputBitstream: end-to-end round-trip through a real
// std::stringstream buffer. Exercises typed write + typed read paths across
// the two components with a heterogeneous payload.
TEST(BitstreamIntegrationTest, RoundTripHeterogeneousPayload)
{
	std::stringstream stream;
	IO::OutputBitstream out(stream);
	IO::InputBitstream in(stream);

	ASSERT_TRUE(out.write<unsigned int>(0xDEADBEEFu));
	ASSERT_TRUE(out.write<int>(-42));
	ASSERT_TRUE(out.write<double>(3.1415926535));
	ASSERT_TRUE(out.write<std::string>(std::string("hello world")));
	ASSERT_TRUE(out.write<std::wstring>(std::wstring(L"wchr")));

	unsigned int u = 0u;
	int i = 0;
	double d = 0.0;
	std::string s;
	std::wstring w;

	ASSERT_TRUE(in.read<unsigned int>(u));
	ASSERT_TRUE(in.read<int>(i));
	ASSERT_TRUE(in.read<double>(d));
	ASSERT_TRUE(in.read<std::string>(s));
	ASSERT_TRUE(in.read<std::wstring>(w));

	EXPECT_EQ(u, 0xDEADBEEFu);
	EXPECT_EQ(i, -42);
	EXPECT_DOUBLE_EQ(d, 3.1415926535);
	EXPECT_EQ(s, std::string("hello world"));
	EXPECT_EQ(w, std::wstring(L"wchr"));
}

// InputBitstream + std::istringstream: size(), position(), setPosition(),
// skip() interact with the underlying std::istream state.
TEST(BitstreamIntegrationTest, PositionSizeSkipAndSetPosition)
{
	const std::string payload = makeMixedPayload();
	ASSERT_FALSE(payload.empty());

	std::istringstream input(payload);
	IO::InputBitstream in(input);

	const uint64_t total = in.size();
	EXPECT_EQ(total, static_cast<uint64_t>(payload.size()));

	// Initial position must be zero.
	EXPECT_EQ(in.position(), uint64_t(0));

	// Read the first 4-byte value and verify position advances accordingly.
	unsigned int first = 0u;
	ASSERT_TRUE(in.read<unsigned int>(first));
	EXPECT_EQ(first, 0xDEADBEEFu);
	EXPECT_EQ(in.position(), uint64_t(sizeof(unsigned int)));

	// Skipping zero bytes is a no-op and must succeed.
	EXPECT_TRUE(in.skip(0));
	EXPECT_EQ(in.position(), uint64_t(sizeof(unsigned int)));

	// Skip over the int(-42) so the next read is the double.
	ASSERT_TRUE(in.skip(sizeof(int)));
	double d = 0.0;
	ASSERT_TRUE(in.read<double>(d));
	EXPECT_DOUBLE_EQ(d, 3.1415926535);

	// Rewind and confirm the first value can be re-read.
	ASSERT_TRUE(in.setPosition(0));
	EXPECT_EQ(in.position(), uint64_t(0));
	unsigned int again = 0u;
	ASSERT_TRUE(in.read<unsigned int>(again));
	EXPECT_EQ(again, 0xDEADBEEFu);

	// Seeking beyond end must fail without disturbing readability.
	EXPECT_FALSE(in.setPosition(total + 1));

	// Skipping to exactly end-of-stream must succeed.
	ASSERT_TRUE(in.setPosition(0));
	EXPECT_TRUE(in.skip(total));
	EXPECT_EQ(in.position(), total);
}

// look<T>() (via ScopedInputBitstream) must NOT advance the underlying
// istream, so a subsequent read yields the same value.
TEST(BitstreamIntegrationTest, LookDoesNotConsume)
{
	std::stringstream stream;
	IO::OutputBitstream out(stream);
	ASSERT_TRUE(out.write<int>(12345));
	ASSERT_TRUE(out.write<int>(67890));

	IO::InputBitstream in(stream);

	int peeked = 0;
	ASSERT_TRUE(in.look<int>(peeked));
	EXPECT_EQ(peeked, 12345);
	EXPECT_EQ(in.position(), uint64_t(0));

	int consumed = 0;
	ASSERT_TRUE(in.read<int>(consumed));
	EXPECT_EQ(consumed, 12345);
	EXPECT_EQ(in.position(), uint64_t(sizeof(int)));

	int next = 0;
	ASSERT_TRUE(in.read<int>(next));
	EXPECT_EQ(next, 67890);
}

// readDefault<T>() returns the default value once the stream is exhausted.
// Exercises the failure fallback branch through real stream state.
TEST(BitstreamIntegrationTest, ReadDefaultFallsBackWhenExhausted)
{
	std::stringstream stream;
	IO::OutputBitstream out(stream);
	ASSERT_TRUE(out.write<int>(777));

	IO::InputBitstream in(stream);

	// First readDefault consumes the written int.
	const int firstRead = in.readDefault<int>(-1);
	EXPECT_EQ(firstRead, 777);

	// Second readDefault has no data — the sentinel default must come back.
	const int fallback = in.readDefault<int>(-1);
	EXPECT_EQ(fallback, -1);

	// Reading past EOF must set the eof flag observable via isEndOfFile().
	EXPECT_TRUE(in.isEndOfFile());
}

// reset() must clear EOF state and rewind, allowing subsequent successful
// reads that were previously blocked by the eofbit.
TEST(BitstreamIntegrationTest, ResetClearsEofAndRewinds)
{
	std::stringstream stream;
	IO::OutputBitstream out(stream);
	ASSERT_TRUE(out.write<int>(11));
	ASSERT_TRUE(out.write<int>(22));

	IO::InputBitstream in(stream);

	int a = 0;
	int b = 0;
	ASSERT_TRUE(in.read<int>(a));
	ASSERT_TRUE(in.read<int>(b));
	EXPECT_EQ(a, 11);
	EXPECT_EQ(b, 22);

	// Force EOF by attempting one more read past the end.
	int extra = 0;
	EXPECT_FALSE(in.read<int>(extra));
	EXPECT_TRUE(in.isEndOfFile());

	// After reset(), the stream is usable again and reads return the same
	// initial values in order.
	ASSERT_TRUE(in.reset());
	EXPECT_EQ(in.position(), uint64_t(0));

	int a2 = 0;
	int b2 = 0;
	ASSERT_TRUE(in.read<int>(a2));
	ASSERT_TRUE(in.read<int>(b2));
	EXPECT_EQ(a2, 11);
	EXPECT_EQ(b2, 22);
}

// OutputBitstream::write(void*, size) + InputBitstream::read(void*, size)
// combined round-trip of a raw byte buffer, exercising the non-templated
// overloads.
TEST(BitstreamIntegrationTest, RawBufferRoundTrip)
{
	const unsigned char pattern[8] = {0x01u, 0x23u, 0x45u, 0x67u,
									   0x89u, 0xABu, 0xCDu, 0xEFu};

	std::stringstream stream;
	IO::OutputBitstream out(stream);

	// Writing size 0 with any pointer is a no-op and must succeed.
	EXPECT_TRUE(out.write(pattern, 0));

	ASSERT_TRUE(out.write(pattern, sizeof(pattern)));
	EXPECT_EQ(out.size(), uint64_t(sizeof(pattern)));

	IO::InputBitstream in(stream);
	unsigned char readback[8] = {0u};

	// Reading size 0 is a no-op and must succeed without touching state.
	EXPECT_TRUE(in.read(readback, 0));
	EXPECT_EQ(in.position(), uint64_t(0));

	ASSERT_TRUE(in.read(readback, sizeof(readback)));
	for (size_t n = 0; n < sizeof(pattern); ++n)
	{
		EXPECT_EQ(readback[n], pattern[n]) << "mismatch at index " << n;
	}
}

// Empty std::string round-trips correctly: the length prefix is written and
// read even though there are zero payload bytes.
TEST(BitstreamIntegrationTest, EmptyStringRoundTripPreservesLengthPrefix)
{
	std::stringstream stream;
	IO::OutputBitstream out(stream);

	ASSERT_TRUE(out.write<std::string>(std::string()));
	// A 4-byte length prefix must have been written.
	EXPECT_EQ(out.size(), uint64_t(sizeof(unsigned int)));

	IO::InputBitstream in(stream);
	std::string readback("previous");
	ASSERT_TRUE(in.read<std::string>(readback));
	EXPECT_TRUE(readback.empty());
	EXPECT_EQ(in.position(), uint64_t(sizeof(unsigned int)));
}

} // namespace TestIO

} // namespace Test

} // namespace Ocean

#endif // OCEAN_USE_GTEST
