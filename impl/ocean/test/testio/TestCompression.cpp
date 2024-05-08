/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestCompression.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Compression.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestCompression::test(const double testDuration)
{
	Log::info() << "Compression test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testGzipCompression(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Entire Compression test succeeded.";
	else
		Log::info() << "Compression test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCompression, GzipCompression)
{
	EXPECT_TRUE(TestCompression::testGzipCompression(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestCompression::testGzipCompression(const double testDuration)
{
	Log::info() << "Gzip 100KB test:";

	bool allSucceeded = true;

	IO::Compression::Buffer uncompressedBuffer, compressedBuffer, testBuffer;

	HighPerformanceStatistic performanceCompression;
	HighPerformanceStatistic performanceDecompression;

	const Timestamp startTimestamp(true);

	do
	{
		uncompressedBuffer.clear();
		compressedBuffer.clear();
		testBuffer.clear();

		const unsigned int size = 102439u +  RandomI::random(2u);

		uncompressedBuffer.resize(size);
		for (unsigned int n = 0u; n < size; ++n)
		{
			uncompressedBuffer[n] = 'a' + char(RandomI::random(1u));
		}

		performanceCompression.start();
		IO::Compression::gzipCompress(uncompressedBuffer.data(), uncompressedBuffer.size(), compressedBuffer);
		performanceCompression.stop();

		performanceDecompression.start();
		IO::Compression::gzipDecompress(compressedBuffer.data(), compressedBuffer.size(), testBuffer);
		performanceDecompression.stop();

		if (uncompressedBuffer.size() != testBuffer.size())
		{
			allSucceeded = false;
		}
		else
		{
			for (unsigned int n = 0u; n < uncompressedBuffer.size(); ++n)
			{
				if (uncompressedBuffer[n] != testBuffer[n])
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Compression: Best: " << performanceCompression.bestMseconds() << "ms, worst: " << performanceCompression.worstMseconds() << "ms, average: " << performanceCompression.averageMseconds() << "ms";
	Log::info() << "Decompression: Best: " << performanceDecompression.bestMseconds() << "ms, worst: " << performanceDecompression.worstMseconds() << "ms, average: " << performanceDecompression.averageMseconds() << "ms";

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

}

}

}
