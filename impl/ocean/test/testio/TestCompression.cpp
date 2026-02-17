/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestCompression.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

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

bool TestCompression::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("Compression test");
	Log::info() << " ";

	if (selector.shouldRun("gzipcompression"))
	{
		testResult = testGzipCompression(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	IO::Compression::Buffer uncompressedBuffer, compressedBuffer, testBuffer;

	HighPerformanceStatistic performanceCompression;
	HighPerformanceStatistic performanceDecompression;

	const Timestamp startTimestamp(true);

	do
	{
		uncompressedBuffer.clear();
		compressedBuffer.clear();
		testBuffer.clear();

		const unsigned int size = 102439u + RandomI::random(randomGenerator, 2u);

		uncompressedBuffer.resize(size);
		for (unsigned int n = 0u; n < size; ++n)
		{
			uncompressedBuffer[n] = 'a' + char(RandomI::random(randomGenerator, 1u));
		}

		performanceCompression.start();
		IO::Compression::gzipCompress(uncompressedBuffer.data(), uncompressedBuffer.size(), compressedBuffer);
		performanceCompression.stop();

		performanceDecompression.start();
		IO::Compression::gzipDecompress(compressedBuffer.data(), compressedBuffer.size(), testBuffer);
		performanceDecompression.stop();

		OCEAN_EXPECT_EQUAL(validation, uncompressedBuffer.size(), testBuffer.size());

		if (uncompressedBuffer.size() == testBuffer.size())
		{
			OCEAN_EXPECT_TRUE(validation, std::equal(uncompressedBuffer.begin(), uncompressedBuffer.end(), testBuffer.begin()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Compression: Best: " << performanceCompression.bestMseconds() << "ms, worst: " << performanceCompression.worstMseconds() << "ms, average: " << performanceCompression.averageMseconds() << "ms";
	Log::info() << "Decompression: Best: " << performanceDecompression.bestMseconds() << "ms, worst: " << performanceDecompression.worstMseconds() << "ms, average: " << performanceDecompression.averageMseconds() << "ms";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
