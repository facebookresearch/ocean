/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestSSE.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/math/Random.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/SSE.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestSSE::test(const double testDuration)
{
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC == 1
	static_assert(sizeof(CV::SSE::M128i) == 16, "Invalid data type");
#endif

	ocean_assert(testDuration > 0.0);

	Log::info() << "---   SSE test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testAveraging1Channel8Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging2Channel16Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging3Channel24Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging4Channel32Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging1Channel32Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging2Channel64Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging3Channel96Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging4Channel128Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBinaryAveraging1Channel8Bit2x2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAveraging1Channel8Bit3x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeInterleave3Channel8Bit15Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeInterleave3Channel8Bit24Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeInterleave3Channel8Bit48Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeInterleave3Channel8Bit45Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterleave3Channel8Bit48Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseChannelOrder2Channel8Bit32Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseChannelOrder3Channel8Bit48Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseChannelOrder4Channel8Bit64Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSwapReversedChannelOrder3Channel8Bit48Elements() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseElements8Bit48Elements() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSwapReversedElements8Bit48Elements() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSumInterleave1Channel8Bit16Elements() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSumInterleave1Channel8Bit15Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSumInterleave3Channel8Bit48Elements() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSumInterleave3Channel8Bit45Elements() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolation1Channel8Bit15Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolation3Channel24Bit12Elements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddOffsetBeforeRightShiftDivisionByTwoSigned16Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddOffsetBeforeRightShiftDivisionSigned16Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddOffsetBeforeRightShiftDivisionByTwoSigned32Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddOffsetBeforeRightShiftDivisionSigned32Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultiplyInt8x16ToInt32x8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultiplyInt8x16ToInt32x8AndAccumulate(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SSE test succeeded.";
	}
	else
	{
		Log::info() << "SSE test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSSE, Averaging1Channel8Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging1Channel8Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging2Channel16Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging2Channel16Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging3Channel24Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging3Channel24Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging4Channel32Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging4Channel32Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging1Channel32Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging1Channel32Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging2Channel64Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging2Channel64Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging3Channel96Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging3Channel96Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging4Channel128Bit2x2)
{
	EXPECT_TRUE(TestSSE::testAveraging4Channel128Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, BinaryAveraging1Channel8Bit2x2)
{
	EXPECT_TRUE(TestSSE::testBinaryAveraging1Channel8Bit2x2(GTEST_TEST_DURATION));
}

TEST(TestSSE, Averaging1Channel8Bit3x3)
{
	EXPECT_TRUE(TestSSE::testAveraging1Channel8Bit3x3(GTEST_TEST_DURATION));
}

TEST(TestSSE, DeInterleave3Channel8Bit15Elements)
{
	EXPECT_TRUE(TestSSE::testDeInterleave3Channel8Bit15Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, DeInterleave3Channel8Bit24Elements)
{
	EXPECT_TRUE(TestSSE::testDeInterleave3Channel8Bit24Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, DeInterleave3Channel8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testDeInterleave3Channel8Bit48Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, DeInterleave3Channel8Bit45Elements)
{
	EXPECT_TRUE(TestSSE::testDeInterleave3Channel8Bit45Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, Interleave3Channel8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testInterleave3Channel8Bit48Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, ReverseChannelOrder2Channel8Bit32Elements)
{
	EXPECT_TRUE(TestSSE::testReverseChannelOrder2Channel8Bit32Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, ReverseChannelOrder3Channel8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testReverseChannelOrder3Channel8Bit48Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, ReverseChannelOrder4Channel8Bit64Elements)
{
	EXPECT_TRUE(TestSSE::testReverseChannelOrder4Channel8Bit64Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, SwapReversedChannelOrder3Channel8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testSwapReversedChannelOrder3Channel8Bit48Elements());
}

TEST(TestSSE, ReverseElements8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testReverseElements8Bit48Elements());
}

TEST(TestSSE, SwapReversedElements8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testSwapReversedElements8Bit48Elements());
}

TEST(TestSSE, SumInterleave1Channel8Bit16Elements)
{
	EXPECT_TRUE(TestSSE::testSumInterleave1Channel8Bit16Elements());
}

TEST(TestSSE, SumInterleave1Channel8Bit15Elements)
{
	EXPECT_TRUE(TestSSE::testSumInterleave1Channel8Bit15Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, SumInterleave3Channel8Bit48Elements)
{
	EXPECT_TRUE(TestSSE::testSumInterleave3Channel8Bit48Elements());
}

TEST(TestSSE, SumInterleave3Channel8Bit45Elements)
{
	EXPECT_TRUE(TestSSE::testSumInterleave3Channel8Bit45Elements());
}

TEST(TestSSE, Interpolation1Channel8Bit15Elements)
{
	EXPECT_TRUE(TestSSE::testInterpolation1Channel8Bit15Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, Interpolation3Channel24Bit12Elements)
{
	EXPECT_TRUE(TestSSE::testInterpolation3Channel24Bit12Elements(GTEST_TEST_DURATION));
}

TEST(TestSSE, AddOffsetBeforeRightShiftDivisionByTwoSigned16Bit)
{
	EXPECT_TRUE(TestSSE::testAddOffsetBeforeRightShiftDivisionByTwoSigned16Bit(GTEST_TEST_DURATION));
}

TEST(TestSSE, AddOffsetBeforeRightShiftDivisionSigned16Bit)
{
	EXPECT_TRUE(TestSSE::testAddOffsetBeforeRightShiftDivisionSigned16Bit(GTEST_TEST_DURATION));
}

TEST(TestSSE, AddOffsetBeforeRightShiftDivisionByTwoSigned32Bit)
{
	EXPECT_TRUE(TestSSE::testAddOffsetBeforeRightShiftDivisionByTwoSigned32Bit(GTEST_TEST_DURATION));
}

TEST(TestSSE, AddOffsetBeforeRightShiftDivisionSigned32Bit)
{
	EXPECT_TRUE(TestSSE::testAddOffsetBeforeRightShiftDivisionSigned32Bit(GTEST_TEST_DURATION));
}

TEST(TestSSE, MultiplyInt8x16ToInt32x8)
{
	EXPECT_TRUE(TestSSE::testMultiplyInt8x16ToInt32x8(GTEST_TEST_DURATION));
}

TEST(TestSSE, MultiplyInt8x16ToInt32x8AndAccumulate)
{
	EXPECT_TRUE(TestSSE::testMultiplyInt8x16ToInt32x8AndAccumulate(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSSE::testAveraging1Channel8Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 1 channel 8 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 0u; i <= 2u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements1Channel8Bit2x2(testDuration, 8u << i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging1Channel32Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 1 channel 32 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 0u; i <= 0u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements1Channel32Bit2x2(testDuration, 8u << i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging1Channel8Bit3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging 3x3 of a 1 channel 8 bit image information:";

	bool allSucceeded = true;

	{
		Log::info() << " ";
		allSucceeded = testAverageElements1Channel8Bit3x3(testDuration, 30u) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging2Channel16Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 2 channel 16 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 0u; i <= 2u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements2Channel16Bit2x2(testDuration, 8 << i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging2Channel64Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 2 channel 64 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 0u; i <= 0u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements2Channel64Bit2x2(testDuration, 8 << i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging3Channel24Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 3 channel 24 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 2u; i <= 2u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements3Channel24Bit2x2(testDuration, 12u * i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging3Channel96Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 3 channel 96 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 1u; i <= 1u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements3Channel96Bit2x2(testDuration, 6 * i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging4Channel32Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 4 channel 32 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 1u; i <= 2u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements4Channel32Bit2x2(testDuration, 8 << i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testAveraging4Channel128Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 4 channel 128 bit image information:";

	bool allSucceeded = true;

	for (unsigned int i = 0u; i <= 0u; i++)
	{
		Log::info() << " ";
		allSucceeded = testAverageElements4Channel128Bit2x2(testDuration, 8 << i) && allSucceeded;
	}

	return allSucceeded;
}

bool TestSSE::testBinaryAveraging1Channel8Bit2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test averaging of a 1 channel binary image:";

	bool allSucceeded = true;

	for (const unsigned int elements : {8u, 16u, 32u})
	{
		for (const unsigned int threshold : {400u, 640u, 766u}) // two, three, four pixel in 2x2 are 0xFF
		{
			Log::info() << " ";

			allSucceeded = testAverageElementsBinary1Channel8Bit2x2(testDuration, elements, threshold) && allSucceeded;
		}
	}

	return allSucceeded;
}

bool TestSSE::testDeInterleave3Channel8Bit15Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test de-interleave 3 channel 8 bit data with 15 elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t interleaved[15 + 1];

		for (unsigned int n = 0u; n < 15u + 1u; ++n)
		{
			interleaved[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		const __m128i interleave0 = CV::SSE::load128i(interleaved);

		__m128i deinterleaved01;
		__m128i deinterleaved2;
		CV::SSE::deInterleave3Channel8Bit15Elements(interleave0, deinterleaved01, deinterleaved2);

		uint8_t deinterleaved[32];

		CV::SSE::store128i(deinterleaved01, deinterleaved + 0);
		CV::SSE::store128i(deinterleaved2, deinterleaved + 16);

		const uint8_t* const channel0 = deinterleaved + 0;
		const uint8_t* const channel1 = deinterleaved + 8;
		const uint8_t* const channel2 = deinterleaved + 16;
		const uint8_t* const zeros = deinterleaved + 24;

		for (unsigned int n = 0u; n < 5u; ++n)
		{
			if (channel0[n] != interleaved[n * 3u + 0u] || channel1[n] != interleaved[n * 3u + 1u] || channel2[n] != interleaved[n * 3u + 2u] || zeros[n] != 0u)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 5u; n < 8u; ++n)
		{
			if (channel0[n] != 0u || channel1[n] != 0u || channel2[n] != 0u || zeros[n] != 0u)
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testDeInterleave3Channel8Bit24Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test de-interleave 3 channel 8 bit data with 24 elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t interleaved[24 + 8];

		for (unsigned int n = 0u; n < 24u + 8u; ++n)
		{
			interleaved[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		const __m128i interleave0 = CV::SSE::load128i(interleaved + 0);
		const __m128i interleave1 = CV::SSE::load128i(interleaved + 16);

		__m128i deinterleaved01;
		__m128i deinterleaved2;
		CV::SSE::deInterleave3Channel8Bit24Elements(interleave0, interleave1, deinterleaved01, deinterleaved2);

		uint8_t deinterleaved[24 + 8];

		CV::SSE::store128i(deinterleaved01, deinterleaved + 0);
		CV::SSE::store128i(deinterleaved2, deinterleaved + 16);

		const uint8_t* const channel0 = deinterleaved + 0;
		const uint8_t* const channel1 = deinterleaved + 8;
		const uint8_t* const channel2 = deinterleaved + 16;
		const uint8_t* const zeros = deinterleaved + 24;

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			if (channel0[n] != interleaved[n * 3u + 0u] || channel1[n] != interleaved[n * 3u + 1u] || channel2[n] != interleaved[n * 3u + 2u] || zeros[n] != 0u)
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testDeInterleave3Channel8Bit48Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test de-interleave 3 channel 8 bit data with 48 elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t interleaved[48];

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			interleaved[3u * n + 0u] = uint8_t(RandomI::random(randomGenerator, 255u));
			interleaved[3u * n + 1u] = uint8_t(RandomI::random(randomGenerator, 255u));
			interleaved[3u * n + 2u] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		uint8_t channel0[16], channel1[16], channel2[16];
		CV::SSE::deInterleave3Channel8Bit48Elements(interleaved, channel0, channel1, channel2);

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			if (channel0[n] != interleaved[n * 3u + 0u] || channel1[n] != interleaved[n * 3u + 1u] || channel2[n] != interleaved[n * 3u + 2u])
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testDeInterleave3Channel8Bit45Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test de-interleave 3 channel 8 bit data with 45 elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t interleaved[48];

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			interleaved[3u * n + 0u] = uint8_t(RandomI::random(randomGenerator, 255u));
			interleaved[3u * n + 1u] = uint8_t(RandomI::random(randomGenerator, 255u));
			interleaved[3u * n + 2u] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		__m128i channel0, channel1, channel2;
		CV::SSE::deInterleave3Channel8Bit45Elements(interleaved, channel0, channel1, channel2);

		for (unsigned int n = 0u; n < 15u; ++n)
		{
			if (CV::SSE::value_u8(channel0, n) != interleaved[n * 3u + 0u] || CV::SSE::value_u8(channel1, n) != interleaved[n * 3u + 1u] || CV::SSE::value_u8(channel2, n) != interleaved[n * 3u + 2u])
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 15u; n < 16; ++n)
		{
			if (CV::SSE::value_u8(channel0, n) != 0u || CV::SSE::value_u8(channel1, n) != 0u || CV::SSE::value_u8(channel2, n) != 0u)
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testInterleave3Channel8Bit48Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test interleave 3 channel 8 bit data with 48 elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t channel0[16];
		uint8_t channel1[16];
		uint8_t channel2[16];

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			channel0[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			channel1[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			channel2[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		uint8_t interleaved[16u * 3u];
		CV::SSE::interleave3Channel8Bit48Elements(channel0, channel1, channel2, interleaved);

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			if (channel0[n] != interleaved[n * 3u + 0u] || channel1[n] != interleaved[n * 3u + 1u] || channel2[n] != interleaved[n * 3u + 2u])
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testReverseChannelOrder2Channel8Bit32Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int pixels = 16u;
	constexpr unsigned int channels = 2u;
	constexpr unsigned int elements = pixels * channels;

	Log::info() << "Test reversing channel order of " << channels << " channels 8 bit data with " << elements << " elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t interleaved[elements];

		for (unsigned int n = 0u; n < elements; ++n)
		{
			interleaved[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		uint8_t reverseInterleaved[elements];

		CV::SSE::reverseChannelOrder2Channel8Bit32Elements(interleaved, reverseInterleaved);

		for (unsigned int n = 0u; n < pixels; ++n)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				if (reverseInterleaved[channels * n + (channels - c - 1u)] != interleaved[channels * n + c])
				{
					allSucceeded = false;
				}
			}
		}
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

bool TestSSE::testReverseChannelOrder3Channel8Bit48Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int pixels = 16u;
	constexpr unsigned int channels = 3u;
	constexpr unsigned int elements = pixels * channels;

	Log::info() << "Test reversing channel order of " << channels << " channels 8 bit data with " << elements << " elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			uint8_t interleaved[elements];

			for (unsigned int n = 0u; n < elements; ++n)
			{
				interleaved[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			uint8_t reverseInterleaved[elements];

			CV::SSE::reverseChannelOrder3Channel8Bit48Elements(interleaved, reverseInterleaved);

			for (unsigned int n = 0u; n < pixels; ++n)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					if (reverseInterleaved[channels * n + (channels - c - 1u)] != interleaved[channels * n + c])
					{
						allSucceeded = false;
					}
				}
			}
		}

		{
			uint8_t interleaved[elements];

			for (unsigned int n = 0u; n < elements; ++n)
			{
				interleaved[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			uint8_t reverseInterleaved[elements];
			memcpy(reverseInterleaved, interleaved, elements * sizeof(uint8_t));

			CV::SSE::reverseChannelOrder3Channel8Bit48Elements(reverseInterleaved);

			for (unsigned int n = 0u; n < pixels; ++n)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					if (reverseInterleaved[channels * n + (channels - c - 1u)] != interleaved[channels * n + c])
					{
						allSucceeded = false;
					}
				}
			}
		}
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

bool TestSSE::testReverseChannelOrder4Channel8Bit64Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int pixels = 16u;
	constexpr unsigned int channels = 4u;
	constexpr unsigned int elements = pixels * channels;

	Log::info() << "Test reversing channel order of " << channels << " channels 8 bit data with " << elements << " elements:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		uint8_t interleaved[elements];

		for (unsigned int n = 0u; n < elements; ++n)
		{
			interleaved[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		uint8_t reverseInterleaved[elements];

		CV::SSE::reverseChannelOrder4Channel8Bit64Elements(interleaved, reverseInterleaved);

		for (unsigned int n = 0u; n < pixels; ++n)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				if (reverseInterleaved[channels * n + (channels - c - 1u)] != interleaved[channels * n + c])
				{
					allSucceeded = false;
				}
			}
		}
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

bool TestSSE::testSwapReversedChannelOrder3Channel8Bit48Elements()
{
	Log::info() << "Test swapping and reversing channel order of 3 channel 8 bit data with 48 elements:";

	bool allSucceeded = true;

	uint8_t first[48], second[48];

	for (unsigned int n = 0u; n < 16u; ++n)
	{
		first[3u * n + 0u] = uint8_t(1u + n);
		first[3u * n + 1u] = uint8_t(101u + n);
		first[3u * n + 2u] = uint8_t(201u + n);

		second[3u * n + 0u] = uint8_t(31u + n);
		second[3u * n + 1u] = uint8_t(131u + n);
		second[3u * n + 2u] = uint8_t(231u + n);
	}

	CV::SSE::swapReversedChannelOrder3Channel8Bit48Elements(first, second);

	for (unsigned int n = 0u; n < 16u; ++n)
	{
		if (second[3u * n + 2u] != 1u + n || second[3u * n + 1u] != 101u + n || second[3u * n + 0u] != 201u + n
				|| first[3u * n + 2u] != 31u + n || first[3u * n + 1u] != 131u + n || first[3u * n + 0u] != 231u + n)
		{
			allSucceeded = false;
		}
	}

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

bool TestSSE::testReverseElements8Bit48Elements()
{
	Log::info() << "Test reversing elements 8 bit data with 48 elements:";

	bool allSucceeded = true;

	{
		uint8_t elements[48];

		for (unsigned int n = 0u; n < 48u; ++n)
		{
			elements[n] = uint8_t(n + 1u);
		}

		uint8_t reverseElements[48];

		CV::SSE::reverseElements8Bit48Elements(elements, reverseElements);

		for (unsigned int n = 0u; n < 48u; ++n)
		{
			if (reverseElements[48u - n - 1u] != uint8_t(n + 1u))
			{
				allSucceeded = false;
			}
		}
	}

	{
		uint8_t elements[48];

		for (unsigned int n = 0u; n < 48u; ++n)
		{
			elements[n] = uint8_t(n + 1u);
		}

		CV::SSE::reverseElements8Bit48Elements(elements);

		for (unsigned int n = 0u; n < 48u; ++n)
		{
			if (elements[48u - n - 1u] != uint8_t(n + 1u))
			{
				allSucceeded = false;
			}
		}
	}

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

bool TestSSE::testSwapReversedElements8Bit48Elements()
{
	Log::info() << "Test swapping and reversing elements 8 bit data with 48 elements:";

	bool allSucceeded = true;

	uint8_t first[48], second[48];

	for (unsigned int n = 0u; n < 48u; ++n)
	{
		first[n] = uint8_t(n + 1u);
		second[n] = uint8_t(n + 101u);
	}

	CV::SSE::swapReversedElements8Bit48Elements(first, second);

	for (unsigned int n = 0u; n < 48u; ++n)
	{
		if (second[48u - n - 1u] != n + 1u || first[48u - n - 1u] != n + 101u)
		{
			allSucceeded = false;
		}
	}

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

bool TestSSE::testSumInterleave1Channel8Bit16Elements()
{
	Log::info() << "Test sum of 1x16 elements of image with 1 channel:";

	bool allSucceeded = false;

	uint8_t elements[16];

	for (unsigned int n = 0u; n < 16u; ++n)
	{
		elements[n + 0u] = uint8_t(1u + n);
	}

	allSucceeded = true;

	const __m128i sums = CV::SSE::sum1Channel8Bit16Elements(elements);

	if (CV::SSE::value_u32<0u>(sums) != (1u + 16u) * 8u)
	{
		allSucceeded = false;
	}

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

bool TestSSE::testSumInterleave1Channel8Bit15Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test sum of 1x15 elements of image with 1 channel:";

	bool allSucceeded = true;

	uint8_t elements15[15];
	uint8_t elements16Front[16];
	uint8_t elements16Back[16];

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int i = 0u; i < 1000u; ++i)
		{
			unsigned int test = 0u;

			for (unsigned int n = 0u; n < 15u; ++n)
			{
				const uint8_t randomValue = uint8_t(RandomI::random(255u));

				elements15[n] = randomValue;
				elements16Front[n] = randomValue;
				elements16Back[n + 1u] = randomValue;

				test += randomValue;
			}

			elements16Front[15] = uint8_t(RandomI::random(255u));
			elements16Back[0] = uint8_t(RandomI::random(255u));

			__m128i sums = CV::SSE::sum1Channel8BitFront15Elements<false>(elements15);
			if (CV::SSE::value_u32<0u>(sums) != test)
				allSucceeded = false;

			sums = CV::SSE::sum1Channel8BitFront15Elements<true>(elements16Front);
			if (CV::SSE::value_u32<0u>(sums) != test)
				allSucceeded = false;

			sums = CV::SSE::sum1Channel8BitBack15Elements(elements16Back);
			if (CV::SSE::value_u32<0u>(sums) != test)
				allSucceeded = false;
		}
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

bool TestSSE::testSumInterleave3Channel8Bit48Elements()
{
	Log::info() << "Test sum of 3x16 elements of image with 3 channels:";

	bool allSucceeded = false;

	uint8_t interleaved[48];

	for (unsigned int n = 0u; n < 16u; ++n)
	{
		interleaved[3u * n + 0u] = uint8_t(1u + n);
		interleaved[3u * n + 1u] = uint8_t(101u + n);
		interleaved[3u * n + 2u] = uint8_t(201u + n);
	}

	allSucceeded = true;

	const __m128i sums = CV::SSE::sumInterleave3Channel8Bit48Elements(interleaved);

	if (CV::SSE::value_u32<0u>(sums) != (1u + 16u) * 8u || CV::SSE::value_u32<1u>(sums) != (101u + 116u) * 8u || CV::SSE::value_u32<2u>(sums) != (201u + 216u) * 8u)
	{
		allSucceeded = false;
	}

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

bool TestSSE::testSumInterleave3Channel8Bit45Elements()
{
	Log::info() << "Test sum of 3x15 elements of image with 3 channels:";

	bool allSucceeded = false;

	uint8_t interleaved[45];

	for (unsigned int n = 0u; n < 15u; ++n)
	{
		interleaved[3u * n + 0u] = uint8_t(1u + n);
		interleaved[3u * n + 1u] = uint8_t(101u + n);
		interleaved[3u * n + 2u] = uint8_t(201u + n);
	}

	allSucceeded = true;

	const __m128i sums = CV::SSE::sumInterleave3Channel8Bit45Elements(interleaved);

	if (CV::SSE::value_u32<0u>(sums) != (1u + 15u) * 7u + 8u || CV::SSE::value_u32<1u>(sums) != (101u + 115u) * 7u + 108u || CV::SSE::value_u32<2u>(sums) != (201u + 215u) * 7u + 208u)
	{
		allSucceeded = false;
	}

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

bool TestSSE::testInterpolation1Channel8Bit15Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test interpolation of 15 elements of a 1 channel 8 bit image information:";

	bool allSucceeded = false;

	uint8_t row0[16];
	uint8_t row1[16];

	uint8_t target[15];
	uint8_t test[15];

	allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar xFactor = Random::scalar(0, 1);
		const Scalar yFactor = Random::scalar(0, 1);

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			row0[n] = uint8_t(RandomI::random(0u, 255u));
			row1[n] = uint8_t(RandomI::random(0u, 255u));
		}

		const unsigned int fx = (unsigned int)(xFactor * Scalar(128) + Scalar(0.5));
		const unsigned int fy = (unsigned int)(yFactor * Scalar(128) + Scalar(0.5));

		const unsigned int fx_ = 128u - fx;
		const unsigned int fy_ = 128u - fy;

		const unsigned int fxfy = fx * fy;
		const unsigned int fx_fy = fx_ * fy;
		const unsigned int fx_fy_ = fx_ * fy_;
		const unsigned int fxfy_ = fx * fy_;

		const unsigned int i_fx_fy_fxfy_ = fx_fy_ | (fxfy_ << 16);
		const unsigned int i_fx_fyfxfy = fx_fy | (fxfy << 16);

		const __m128i value0 = CV::SSE::load128i(row0);
		const __m128i value1 = CV::SSE::load128i(row1);

		const __m128i fx_fy_fxfy_ = _mm_set1_epi32(int(i_fx_fy_fxfy_));
		const __m128i fx_fyfxfy = _mm_set1_epi32(int(i_fx_fyfxfy));

		const __m128i result_128(CV::SSE::interpolation1Channel8Bit15Elements(value0, value1, fx_fy_fxfy_, fx_fyfxfy));
		memcpy(target, &result_128, 15);

		for (unsigned int n = 0u; n < 15u; ++n)
		{
			test[n] = uint8_t(((row0[n] * fx_ + row0[n + 1u] * fx) * fy_ + (row1[n] * fx_ + row1[n + 1u] * fx) * fy + 8192u) / 16384u);
		}

		for (unsigned int n = 0u; n < 15u; ++n)
		{
			if (target[n] != test[n])
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testInterpolation3Channel24Bit12Elements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test interpolation of 12 elements of a 3 channel 24 bit image information:";

	bool allSucceeded = false;

	uint8_t row0[16];
	uint8_t row1[16];

	uint8_t target[12];
	uint8_t test[12];

	allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const Scalar xFactor = Random::scalar(0, 1);
		const Scalar yFactor = Random::scalar(0, 1);

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			row0[n] = uint8_t(RandomI::random(0u, 255u));
			row1[n] = uint8_t(RandomI::random(0u, 255u));
		}

		const unsigned int fx = (unsigned int)(xFactor * Scalar(128) + Scalar(0.5));
		const unsigned int fy = (unsigned int)(yFactor * Scalar(128) + Scalar(0.5));

		const unsigned int fx_ = 128u - fx;
		const unsigned int fy_ = 128u - fy;

		const unsigned int fxfy = fx * fy;
		const unsigned int fx_fy = fx_ * fy;
		const unsigned int fx_fy_ = fx_ * fy_;
		const unsigned int fxfy_ = fx * fy_;

		const unsigned int i_fx_fy_fxfy_ = fx_fy_ | (fxfy_ << 16);
		const unsigned int i_fx_fyfxfy = fx_fy | (fxfy << 16);

		const __m128i value0 = CV::SSE::load128i(row0);
		const __m128i value1 = CV::SSE::load128i(row1);

		const __m128i fx_fy_fxfy_ = _mm_set1_epi32(int(i_fx_fy_fxfy_));
		const __m128i fx_fyfxfy = _mm_set1_epi32(int(i_fx_fyfxfy));

		const __m128i result_128(CV::SSE::interpolation3Channel24Bit12Elements(value0, value1, fx_fy_fxfy_, fx_fyfxfy));
		memcpy(target, &result_128, 12);

		for (unsigned int n = 0u; n < 12u; ++n)
		{
			test[n] = uint8_t(((row0[n] * fx_ + row0[n + 3u] * fx) * fy_ + (row1[n] * fx_ + row1[n + 3u] * fx) * fy + 8192u) / 16384u);
		}

		for (unsigned int n = 0u; n < 12u; ++n)
		{
			if (target[n] != test[n])
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testAddOffsetBeforeRightShiftDivisionByTwoSigned16Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test offset adding for right shift for 16 bit integer division by two:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		short values[8];

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			values[n] = short(RandomI::random(randomGenerator, 0xFFFFu));
		}

		constexpr unsigned int rightShifts = 1u;
		constexpr unsigned int denominator = 1u << rightShifts;

		const __m128i values_s_16x8 = _mm_loadu_si128((const __m128i*)values);

		const __m128i adjustedValues_s_16x8 = CV::SSE::addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(values_s_16x8);
		const __m128i dividedValues_s_16x8 = _mm_srai_epi16(adjustedValues_s_16x8, int(rightShifts));

		short dividedValues[8];
		_mm_storeu_si128((__m128i*)dividedValues, dividedValues_s_16x8);

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			const int testValue = int(values[n]) / int(denominator);

			if (testValue != int(dividedValues[n]))
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testAddOffsetBeforeRightShiftDivisionSigned16Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test offset adding for right shift for 16 bit integer for division:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		short values[8];

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			values[n] = short(RandomI::random(randomGenerator, 0xFFFFu));
		}

		const unsigned int rightShifts = RandomI::random(randomGenerator, 15u);
		const unsigned int denominator = 1u << rightShifts;

		const __m128i values_s_16x8 = _mm_loadu_si128((const __m128i*)values);

		{
			// test CV::SSE::addOffsetBeforeRightShiftDivisionSigned16Bit()

			const __m128i adjustedValues_s_16x8 = CV::SSE::addOffsetBeforeRightShiftDivisionSigned16Bit(values_s_16x8, rightShifts);
			const __m128i dividedValues_s_16x8 = _mm_srai_epi16(adjustedValues_s_16x8, int(rightShifts));

			short dividedValues[8];
			_mm_storeu_si128((__m128i*)dividedValues, dividedValues_s_16x8);

			for (unsigned int n = 0u; n < 8u; ++n)
			{
				const int testValue = int(values[n]) / int(denominator);

				if (testValue != int(dividedValues[n]))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// test CV::SSE::divideByRightShiftSigned16Bit()

			const __m128i dividedValues_s_16x8 = CV::SSE::divideByRightShiftSigned16Bit(values_s_16x8, rightShifts);

			short dividedValues[8];
			_mm_storeu_si128((__m128i*)dividedValues, dividedValues_s_16x8);

			for (unsigned int n = 0u; n < 8u; ++n)
			{
				const int testValue = int(values[n]) / int(denominator);

				if (testValue != int(dividedValues[n]))
				{
					allSucceeded = false;
				}
			}
		}
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

bool TestSSE::testAddOffsetBeforeRightShiftDivisionByTwoSigned32Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test offset adding for right shift of 32 bit integer for division by two:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		int values[4];

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			values[n] = int(RandomI::random32(randomGenerator));
		}

		constexpr unsigned int rightShifts = 1u;
		constexpr unsigned int denominator = 1u << rightShifts;

		const __m128i values_s_32x4 = _mm_loadu_si128((const __m128i*)values);

		const __m128i adjustedValues_s_32x4 = CV::SSE::addOffsetBeforeRightShiftDivisionByTwoSigned32Bit(values_s_32x4);
		const __m128i dividedValues_s_32x4 = _mm_srai_epi32(adjustedValues_s_32x4, int(rightShifts));

		int dividedValues[4];
		_mm_storeu_si128((__m128i*)dividedValues, dividedValues_s_32x4);

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			const int testValue = int(values[n]) / int(denominator);

			if (testValue != int(dividedValues[n]))
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testAddOffsetBeforeRightShiftDivisionSigned32Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test offset adding for right shift for 32 bit integer for division:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		int values[4];

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			values[n] = int(RandomI::random32(randomGenerator));
		}

		const unsigned int rightShifts = RandomI::random(randomGenerator, 31u);
		const unsigned int denominator = 1u << rightShifts;

		const __m128i values_s_32x4 = _mm_loadu_si128((const __m128i*)values);

		{
			// test CV::SSE::addOffsetBeforeRightShiftDivisionSigned32Bit()

			const __m128i adjustedValues_s_32x4 = CV::SSE::addOffsetBeforeRightShiftDivisionSigned32Bit(values_s_32x4, rightShifts);
			const __m128i dividedValues_s_32x4 = _mm_srai_epi32(adjustedValues_s_32x4, int(rightShifts));

			int dividedValues[4];
			_mm_storeu_si128((__m128i*)dividedValues, dividedValues_s_32x4);

			for (unsigned int n = 0u; n < 4u; ++n)
			{
				const int testValue = int(values[n]) / int(denominator);

				if (testValue != int(dividedValues[n]))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// test CV::SSE::divideByRightShiftSigned32Bit()

			const __m128i dividedValues_s_32x4 = CV::SSE::divideByRightShiftSigned32Bit(values_s_32x4, rightShifts);

			int dividedValues[4];
			_mm_storeu_si128((__m128i*)dividedValues, dividedValues_s_32x4);

			for (unsigned int n = 0u; n < 4u; ++n)
			{
				const int testValue = int(values[n]) / int(denominator);

				if (testValue != int(dividedValues[n]))
				{
					allSucceeded = false;
				}
			}
		}
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

bool TestSSE::testMultiplyInt8x16ToInt32x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test multiplying 16 bit integer values:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		short valuesA[8];
		short valuesB[8];

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			valuesA[n] = short(RandomI::random(randomGenerator, 0xFFFFu));
			valuesB[n] = short(RandomI::random(randomGenerator, 0xFFFFu));
		}

		const __m128i valuesA_s_16x8 = _mm_loadu_si128((const __m128i*)valuesA);
		const __m128i valuesB_s_16x8 = _mm_loadu_si128((const __m128i*)valuesB);

		__m128i productsA_s_32x4;
		__m128i productsB_s_32x4;
		CV::SSE::multiplyInt8x16ToInt32x8(valuesA_s_16x8, valuesB_s_16x8, productsA_s_32x4, productsB_s_32x4);

		int productValues[8];
		_mm_storeu_si128((__m128i*)productValues + 0, productsA_s_32x4);
		_mm_storeu_si128((__m128i*)productValues + 1, productsB_s_32x4);

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			const int testValue = int(valuesA[n]) * int(valuesB[n]);

			if (testValue != productValues[n])
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testMultiplyInt8x16ToInt32x8AndAccumulate(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test multiplying and accumulating 16 bit integer values:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		short valuesA[8];
		short valuesB[8];
		int results[8];

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			valuesA[n] = short(RandomI::random(randomGenerator, 0xFFFFu));
			valuesB[n] = short(RandomI::random(randomGenerator, 0xFFFFu));
			results[n] = int(RandomI::random(randomGenerator, 0xFFFFu));
		}

		const __m128i valuesA_s_16x8 = _mm_loadu_si128((const __m128i*)valuesA);
		const __m128i valuesB_s_16x8 = _mm_loadu_si128((const __m128i*)valuesB);

		__m128i resultsA_s_16x8 = _mm_loadu_si128((const __m128i*)results + 0);
		__m128i resultsB_s_16x8 = _mm_loadu_si128((const __m128i*)results + 1);

		CV::SSE::multiplyInt8x16ToInt32x8AndAccumulate(valuesA_s_16x8, valuesB_s_16x8, resultsA_s_16x8, resultsB_s_16x8);

		int resultValues[8];
		_mm_storeu_si128((__m128i*)resultValues + 0, resultsA_s_16x8);
		_mm_storeu_si128((__m128i*)resultValues + 1, resultsB_s_16x8);

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			const int testValue = results[n] + int(valuesA[n]) * int(valuesB[n]);

			if (testValue != resultValues[n])
			{
				allSucceeded = false;
			}
		}
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

bool TestSSE::testAverageElements1Channel8Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 32u || elements == 16u || elements == 8u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements << " elements via SSE::average" << elements << "Elements1Channel8Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements, 2u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements_2, 1u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const uint8_t* row0 = source.constrow<uint8_t>(0u);
			const uint8_t* row1 = source.constrow<uint8_t>(1u);

			uint8_t* reference = validation.data<uint8_t>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements1Channel8Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 8u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average8Elements1Channel8Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 16u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average16Elements1Channel8Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 32u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average32Elements1Channel8Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		if (memcmp(target.constdata<void>(), validation.constdata<void>(), target.size()) != 0)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElementsBinary1Channel8Bit2x2(const double testDuration, const unsigned int elements, const unsigned int sumThreshold)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 8u || elements == 16u || elements == 32u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements << " elements via SSE::average" << elements << "ElementsBinary1Channel8Bit2x2 (threshold = "<< sumThreshold << ") and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements, 2u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements_2, 1u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		for (unsigned int n = 0u; n < source.pixels(); ++n)
		{
			source.data<uint8_t>()[n] = uint8_t(255u * RandomI::random(randomGenerator, 1u));
		}

		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const uint8_t* row0 = source.constrow<uint8_t>(0u);
			const uint8_t* row1 = source.constrow<uint8_t>(1u);

			uint8_t* reference = validation.data<uint8_t>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElementsBinary1Channel8Bit2x2(elements_2, row0, row1, reference, sumThreshold);

				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 8u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average8ElementsBinary1Channel8Bit2x2(row0, row1, test, uint16_t(sumThreshold));

					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 16u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average16ElementsBinary1Channel8Bit2x2(row0, row1, test, uint16_t(sumThreshold));

					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 32u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average32ElementsBinary1Channel8Bit2x2(row0, row1, test, uint16_t(sumThreshold));

					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		if (memcmp(target.constdata<void>(), validation.constdata<void>(), target.size()) != 0)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements1Channel32Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 8u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements << " elements via SSE::average" << elements << "Elements1Channel32Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements, 2u,  FrameType::FORMAT_F32, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements_2, 1u, FrameType::FORMAT_F32, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const float* row0 = source.constrow<float>(0u);
			const float* row1 = source.constrow<float>(1u);

			float* reference = validation.data<float>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements1Channel32Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 8u:
			{
				const float* row0 = source.constrow<float>(0u);
				const float* row1 = source.constrow<float>(1u);

				float* test = target.data<float>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average8Elements1Channel32Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		for (unsigned int i = 0u; i < target.pixels(); ++i)
		{
			if (!NumericF::isWeakEqual(target.constdata<float>()[i], validation.constdata<float>()[i]))
			{
				allSucceeded = false;
			}
		}

	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements1Channel8Bit3x3(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 30u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "3x" << elements << " elements via SSE::average" << elements << "Elements1Channel8Bit3x3 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_3 = elements / 3u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements, 3u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements_3, 1u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const uint8_t* row0 = source.constrow<uint8_t>(0u);
			const uint8_t* row1 = source.constrow<uint8_t>(1u);
			const uint8_t* row2 = source.constrow<uint8_t>(2u);

			uint8_t* reference = validation.data<uint8_t>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements1Channel8Bit3x3(elements_3, row0, row1, row2, reference);
				row0 += elements;
				row1 += elements;
				row2 += elements;
				reference += elements_3;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 30u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);
				const uint8_t* row2 = source.constrow<uint8_t>(2u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average30Elements1Channel8Bit3x3(row0, row1, row2, test);
					row0 += elements;
					row1 += elements;
					row2 += elements;
					test += elements_3;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		if (memcmp(target.constdata<uint8_t>(), validation.constdata<uint8_t>(), target.size()) != 0)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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


bool TestSSE::testAverageElements2Channel16Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 32u || elements == 16u || elements == 8u);

	constexpr unsigned int repetitions = 100000u;

	const unsigned int elements_2 = elements / 2u;

	Log::info() << "2x" << elements_2 << " pixels via SSE::average" << elements << "Elements2Channel16Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements_2, 2u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements / 4u, 1u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const uint8_t* row0 = source.constrow<uint8_t>(0u);
			const uint8_t* row1 = source.constrow<uint8_t>(1u);

			uint8_t* reference = validation.data<uint8_t>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements2Channel16Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 8u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average8Elements2Channel16Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 16u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average16Elements2Channel16Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 32u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average32Elements2Channel16Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		if (memcmp(target.constdata<void>(), validation.constdata<void>(), target.size()) != 0)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements2Channel64Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 8u);

	constexpr unsigned int repetitions = 100000u;

	const unsigned int elements_2 = elements / 2u;

	Log::info() << "2x" << elements_2 << " pixels via SSE::average" << elements << "Elements2Channel64Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements_2, 2u,  FrameType::genericPixelFormat<float, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements / 4u, 1u, FrameType::genericPixelFormat<float, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const float* row0 = source.constrow<float>(0u);
			const float* row1 = source.constrow<float>(1u);

			float* reference = validation.data<float>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements2Channel64Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 8u:
			{
				const float* row0 = source.constrow<float>(0u);
				const float* row1 = source.constrow<float>(1u);

				float* test = target.data<float>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average8Elements2Channel64Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		for (unsigned int i = 0u; i < target.pixels() * target.channels(); ++i)
		{
			if (!NumericF::isWeakEqual(target.constdata<float>()[i], validation.constdata<float>()[i]))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements3Channel24Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 12u || elements == 24u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements / 3u << " pixels via SSE::average" << elements << "Elements3Channel24Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements / 3u, 2u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements / 6u, 1u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const uint8_t* row0 = source.constrow<uint8_t>(0u);
			const uint8_t* row1 = source.constrow<uint8_t>(1u);

			uint8_t* reference = validation.data<uint8_t>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0u; i < repetitions; i++)
			{
				referenceAverageElements3Channel24Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 24u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0u; i < repetitions; i++)
				{
					CV::SSE::average24Elements3Channel24Bit2x2(row0, row1, test);

					row0 += elements;
					row1 += elements;
					test += elements_2;
				}

				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		if (memcmp(target.constdata<void>(), validation.constdata<void>(), target.size()) != 0)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements3Channel96Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 6u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements / 3u << " pixels via SSE::average" << elements << "Elements3Channel96Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements / 3u, 2u,  FrameType::genericPixelFormat<float, 3u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements / 6u, 1u, FrameType::genericPixelFormat<float, 3u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const float* row0 = source.constrow<float>(0u);
			const float* row1 = source.constrow<float>(1u);

			float* reference = validation.data<float>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements3Channel96Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 6u:
			{
				const float* row0 = source.constrow<float>(0u);
				const float* row1 = source.constrow<float>(1u);

				float* test = target.data<float>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average6Elements3Channel96Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		for (unsigned int i = 0u; i < target.pixels() * target.channels(); ++i)
		{
			if (!NumericF::isWeakEqual(target.constdata<float>()[i], validation.constdata<float>()[i]))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements4Channel32Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 32u || elements == 16u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements / 4u << " pixels via SSE::average" << elements << "Elements4Channel32Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements / 4u, 2u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements / 8u, 1u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const uint8_t* row0 = source.constrow<uint8_t>(0u);
			const uint8_t* row1 = source.constrow<uint8_t>(1u);

			uint8_t* reference = validation.data<uint8_t>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements4Channel32Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 16u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average16Elements4Channel32Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			case 32u:
			{
				const uint8_t* row0 = source.constrow<uint8_t>(0u);
				const uint8_t* row1 = source.constrow<uint8_t>(1u);

				uint8_t* test = target.data<uint8_t>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average32Elements4Channel32Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		if (memcmp(target.constdata<void>(), validation.constdata<void>(), target.size()) != 0)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::testAverageElements4Channel128Bit2x2(const double testDuration, const unsigned int elements)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(elements == 8u);

	constexpr unsigned int repetitions = 100000u;

	Log::info() << "2x" << elements / 4u << " pixels via SSE::average" << elements << "Elements4Channel128Bit2x2 and " << String::insertCharacter(String::toAString(repetitions), ',', 3, false) << " repetitions:";

	const unsigned int elements_2 = elements / 2u;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceSSE;

	const Timestamp startTimestamp(true);

	do
	{
		Frame source(FrameType(repetitions * elements / 4u, 2u,  FrameType::genericPixelFormat<float, 4u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame target(FrameType(repetitions * elements / 8u, 1u, FrameType::genericPixelFormat<float, 4u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame validation(target.frameType());

		ocean_assert(source.isContinuous() && target.isContinuous() && validation.isContinuous());

		CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(validation, false, &randomGenerator);

		// calculate reference frame
		{
			const float* row0 = source.constrow<float>(0u);
			const float* row1 = source.constrow<float>(1u);

			float* reference = validation.data<float>();

			const HighPerformanceStatistic::ScopedStatistic scoped(performance);

			for (unsigned int i = 0; i < repetitions; i++)
			{
				referenceAverageElements4Channel128Bit2x2(elements_2, row0, row1, reference);
				row0 += elements;
				row1 += elements;
				reference += elements_2;
			}
		}

		// calculate frame via SSE
		switch (elements)
		{
			case 8u:
			{
				const float* row0 = source.constrow<float>(0u);
				const float* row1 = source.constrow<float>(1u);

				float* test = target.data<float>();

				const HighPerformanceStatistic::ScopedStatistic scoped(performanceSSE);

				for (unsigned int i = 0; i < repetitions; i++)
				{
					CV::SSE::average8Elements4Channel128Bit2x2(row0, row1, test);
					row0 += elements;
					row1 += elements;
					test += elements_2;
				}
				break;
			}

			default:
				ocean_assert(false && "not implemented");
				break;
		}

		for (unsigned int i = 0u; i < target.pixels() * target.channels(); ++i)
		{
			if (!NumericF::isWeakEqual(target.constdata<float>()[i], validation.constdata<float>()[i]))
			{
				allSucceeded = false;
			}
		}

	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Standard performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "SSE performance: " << String::toAString(performanceSSE.averageMseconds()) << "ms";

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

bool TestSSE::referenceAverageElements1Channel8Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine)
{
	ocean_assert(imageLine0Buffer8Bit && imageLine1Buffer8Bit && averageLine);

	for (unsigned int n = 0u; n < elements; ++n)
	{
		const unsigned int sum = 2u + imageLine0Buffer8Bit[2u * n] + imageLine0Buffer8Bit[2u * n + 1u] + imageLine1Buffer8Bit[2u * n] + imageLine1Buffer8Bit[2u * n + 1u]; // +2u for rounding

		averageLine[n] = uint8_t(sum / 4u);
	}

	return true;
}

bool TestSSE::referenceAverageElementsBinary1Channel8Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine, const unsigned int sumThreshold)
{
	ocean_assert(imageLine0Buffer8Bit && imageLine1Buffer8Bit && averageLine);

	for (unsigned int n = 0u; n < elements; ++n)
	{
		const unsigned int sum = imageLine0Buffer8Bit[2u * n] + imageLine0Buffer8Bit[2u * n + 1u] + imageLine1Buffer8Bit[2u * n] + imageLine1Buffer8Bit[2u * n + 1u];

		averageLine[n] = sum >= sumThreshold ? 0xFFu : 0x00u;
	}

	return true;
}

bool TestSSE::referenceAverageElements1Channel8Bit3x3(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, const uint8_t* const imageLine2Buffer8Bit, uint8_t* const averageLine)
{
	ocean_assert(imageLine0Buffer8Bit && imageLine1Buffer8Bit && imageLine2Buffer8Bit && averageLine);

	/*		| 1 2 1 |
	 * 1/16x| 2 4 2 |
	 *		| 1 2 1 |*/

	for (unsigned int n = 0u; n < elements; ++n)
	{
		const unsigned int sum = imageLine0Buffer8Bit[3u * n] + 2u * imageLine0Buffer8Bit[3u * n + 1u] + imageLine0Buffer8Bit[3u * n + 2u] + 2u * imageLine1Buffer8Bit[3u * n] + 4u * imageLine1Buffer8Bit[3u * n + 1u] + 2u * imageLine1Buffer8Bit[3u * n + 2u] + imageLine2Buffer8Bit[3u * n] + 2* imageLine2Buffer8Bit[3u * n + 1u] + imageLine2Buffer8Bit[3u * n + 2u] + 8u; // +8u for rounding

		averageLine[n] = uint8_t(sum / 16u);
	}

	return true;
}

bool TestSSE::referenceAverageElements1Channel32Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine)
{
	ocean_assert(imageLine0Buffer32Bit && imageLine1Buffer32Bit && averageLine);

	for (unsigned int n = 0u; n < elements; ++n)
	{
		const float sum = imageLine0Buffer32Bit[2u * n] + imageLine0Buffer32Bit[2u * n + 1u] + imageLine1Buffer32Bit[2u * n] + imageLine1Buffer32Bit[2u * n + 1u];

		averageLine[n] = 0.25f * sum;
	}

	return true;
}


bool TestSSE::referenceAverageElements2Channel16Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine)
{
	ocean_assert(imageLine0Buffer8Bit && imageLine1Buffer8Bit && averageLine);

	for (unsigned int n = 0u; n < elements; n += 2)
	{
		const unsigned int sumY = 2u + imageLine0Buffer8Bit[2u * n] + imageLine0Buffer8Bit[2u * n + 2u] + imageLine1Buffer8Bit[2u * n] + imageLine1Buffer8Bit[2u * n + 2u]; // +2u for rounding
		const unsigned int sumA = 2u + imageLine0Buffer8Bit[2u * n + 1] + imageLine0Buffer8Bit[2u * n + 3u] + imageLine1Buffer8Bit[2u * n + 1u] + imageLine1Buffer8Bit[2u * n + 3u]; // +2u for rounding

		averageLine[n] = uint8_t(sumY / 4u);
		averageLine[n + 1u] = uint8_t(sumA / 4u);
	}

	return true;
}

bool TestSSE::referenceAverageElements2Channel64Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine)
{
	ocean_assert(imageLine0Buffer32Bit && imageLine1Buffer32Bit && averageLine);

	for (unsigned int n = 0u; n < elements; n += 2)
	{
		const float sumY = imageLine0Buffer32Bit[2u * n] + imageLine0Buffer32Bit[2u * n + 2u] + imageLine1Buffer32Bit[2u * n] + imageLine1Buffer32Bit[2u * n + 2u];
		const float sumA = imageLine0Buffer32Bit[2u * n + 1u] + imageLine0Buffer32Bit[2u * n + 3u] + imageLine1Buffer32Bit[2u * n + 1u] + imageLine1Buffer32Bit[2u * n + 3u];

		averageLine[n] = 0.25f * sumY;
		averageLine[n + 1u] =  0.25f * sumA;
	}

	return true;
}

bool TestSSE::referenceAverageElements3Channel24Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine)
{
	ocean_assert(imageLine0Buffer8Bit && imageLine1Buffer8Bit && averageLine);

	for (unsigned int n = 0u; n < elements; n += 3)
	{
		const unsigned int sum0 = 2u + imageLine0Buffer8Bit[2u * n]      + imageLine0Buffer8Bit[2u * n + 3u] + imageLine1Buffer8Bit[2u * n]      + imageLine1Buffer8Bit[2u * n + 3u]; // +2u for rounding
		const unsigned int sum1 = 2u + imageLine0Buffer8Bit[2u * n + 1u] + imageLine0Buffer8Bit[2u * n + 4u] + imageLine1Buffer8Bit[2u * n + 1u] + imageLine1Buffer8Bit[2u * n + 4u]; // +2u for rounding
		const unsigned int sum2 = 2u + imageLine0Buffer8Bit[2u * n + 2u] + imageLine0Buffer8Bit[2u * n + 5u] + imageLine1Buffer8Bit[2u * n + 2u] + imageLine1Buffer8Bit[2u * n + 5u]; // +2u for rounding

		averageLine[n] = uint8_t(sum0 / 4u);
		averageLine[n + 1u] = uint8_t(sum1 / 4u);
		averageLine[n + 2u] = uint8_t(sum2 / 4u);
	}

	return true;
}

bool TestSSE::referenceAverageElements3Channel96Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine)
{
	ocean_assert(imageLine0Buffer32Bit && imageLine1Buffer32Bit && averageLine);

	for (unsigned int n = 0u; n < elements; n += 3)
	{
		const float sum0 = imageLine0Buffer32Bit[2u * n]      + imageLine0Buffer32Bit[2u * n + 3u] + imageLine1Buffer32Bit[2u * n]      + imageLine1Buffer32Bit[2u * n + 3u];
		const float sum1 = imageLine0Buffer32Bit[2u * n + 1u] + imageLine0Buffer32Bit[2u * n + 4u] + imageLine1Buffer32Bit[2u * n + 1u] + imageLine1Buffer32Bit[2u * n + 4u];
		const float sum2 = imageLine0Buffer32Bit[2u * n + 2u] + imageLine0Buffer32Bit[2u * n + 5u] + imageLine1Buffer32Bit[2u * n + 2u] + imageLine1Buffer32Bit[2u * n + 5u];

		averageLine[n] =  0.25f * sum0;
		averageLine[n + 1u] =  0.25f * sum1;
		averageLine[n + 2u] =  0.25f * sum2;
	}

	return true;
}


bool TestSSE::referenceAverageElements4Channel32Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine)
{
	ocean_assert(imageLine0Buffer8Bit && imageLine1Buffer8Bit && averageLine);

	for (unsigned int n = 0u; n < elements; n += 4)
	{
		const unsigned int sum0 = 2u + imageLine0Buffer8Bit[2u * n]      + imageLine0Buffer8Bit[2u * n + 4u] + imageLine1Buffer8Bit[2u * n]      + imageLine1Buffer8Bit[2u * n + 4u]; // +2u for rounding
		const unsigned int sum1 = 2u + imageLine0Buffer8Bit[2u * n + 1u] + imageLine0Buffer8Bit[2u * n + 5u] + imageLine1Buffer8Bit[2u * n + 1u] + imageLine1Buffer8Bit[2u * n + 5u]; // +2u for rounding
		const unsigned int sum2 = 2u + imageLine0Buffer8Bit[2u * n + 2u] + imageLine0Buffer8Bit[2u * n + 6u] + imageLine1Buffer8Bit[2u * n + 2u] + imageLine1Buffer8Bit[2u * n + 6u]; // +2u for rounding
		const unsigned int sum3 = 2u + imageLine0Buffer8Bit[2u * n + 3u] + imageLine0Buffer8Bit[2u * n + 7u] + imageLine1Buffer8Bit[2u * n + 3u] + imageLine1Buffer8Bit[2u * n + 7u]; // +2u for rounding

		averageLine[n] = uint8_t(sum0 / 4u);
		averageLine[n + 1u] = uint8_t(sum1 / 4u);
		averageLine[n + 2u] = uint8_t(sum2 / 4u);
		averageLine[n + 3u] = uint8_t(sum3 / 4u);
	}

	return true;
}

bool TestSSE::referenceAverageElements4Channel128Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine)
{
	ocean_assert(imageLine0Buffer32Bit && imageLine1Buffer32Bit && averageLine);

	for (unsigned int n = 0u; n < elements; n += 4)
	{
		const float sum0 = imageLine0Buffer32Bit[2u * n]      + imageLine0Buffer32Bit[2u * n + 4u] + imageLine1Buffer32Bit[2u * n]      + imageLine1Buffer32Bit[2u * n + 4u];
		const float sum1 = imageLine0Buffer32Bit[2u * n + 1u] + imageLine0Buffer32Bit[2u * n + 5u] + imageLine1Buffer32Bit[2u * n + 1u] + imageLine1Buffer32Bit[2u * n + 5u];
		const float sum2 = imageLine0Buffer32Bit[2u * n + 2u] + imageLine0Buffer32Bit[2u * n + 6u] + imageLine1Buffer32Bit[2u * n + 2u] + imageLine1Buffer32Bit[2u * n + 6u];
		const float sum3 = imageLine0Buffer32Bit[2u * n + 3u] + imageLine0Buffer32Bit[2u * n + 7u] + imageLine1Buffer32Bit[2u * n + 3u] + imageLine1Buffer32Bit[2u * n + 7u];

		averageLine[n] =  0.25f * sum0;
		averageLine[n + 1u] =  0.25f * sum1;
		averageLine[n + 2u] =  0.25f * sum2;
		averageLine[n + 3u] =  0.25f * sum3;
	}

	return true;
}

}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41
