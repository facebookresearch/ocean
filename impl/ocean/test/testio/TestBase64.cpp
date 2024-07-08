/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestBase64.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Base64.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestBase64::test(const double testDuration)
{
	Log::info() << "Base64 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testEncodingDecoding(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire base64 test succeeded.";
	}
	else
	{
		Log::info() << "Base64 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBase64, EncodingDecoding)
{
	EXPECT_TRUE(TestBase64::testEncodingDecoding(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBase64::testEncodingDecoding(const double testDuration)
{
	Log::info() << "Encoding/decoding test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int messageLength = RandomI::random(randomGenerator, 1u, 8192u);
		IO::Base64::Buffer message;
		message.reserve(messageLength);

		for (unsigned int i = 0u; i < messageLength; ++i)
		{
			message.push_back((unsigned char)(RandomI::random(randomGenerator, 255u)));
		}

		IO::Base64::Buffer encodedBuffer;
		IO::Base64::encode(message.data(), message.size(), encodedBuffer);

		IO::Base64::Buffer decodedBuffer;
		IO::Base64::decode(encodedBuffer.data(), encodedBuffer.size(), decodedBuffer);

		allSucceeded = std::equal(message.begin(), message.end(), decodedBuffer.begin()) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestIO

} // namespace Test

} // namespace Ocean
