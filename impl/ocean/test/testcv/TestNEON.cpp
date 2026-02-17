/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestNEON.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/NEON.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include <array>

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestNEON::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("NEON test");
	Log::info() << " ";

	if (selector.shouldRun("sum16bit4blocks3x3"))
	{
		testResult = testSum16Bit4Blocks3x3(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("averagingpixels2x2"))
	{
		testResult = testAveragingPixels2x2(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("multiply"))
	{
		testResult = testMultiply(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copysign"))
	{
		testResult = testCopySign(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("castelements"))
	{
		testResult = testCastElements(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sumhorizontal_u_32x4"))
	{
		testResult = testSumHorizontal_u_32x4(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestNEON, Sum16Bit4Blocks3x3)
{
	EXPECT_TRUE(TestNEON::testSum16Bit4Blocks3x3(GTEST_TEST_DURATION));
}

TEST(TestNEON, AveragingPixels2x2)
{
	EXPECT_TRUE(TestNEON::testAveragingPixels2x2(GTEST_TEST_DURATION));
}

TEST(TestNEON, Multiply)
{
	EXPECT_TRUE(TestNEON::testMultiply(GTEST_TEST_DURATION));
}

TEST(TestNEON, CopySign)
{
	EXPECT_TRUE(TestNEON::testCopySign(GTEST_TEST_DURATION));
}

TEST(TestNEON, CastElements)
{
	EXPECT_TRUE(TestNEON::testCastElements(GTEST_TEST_DURATION));
}

TEST(TestNEON, SumHorizontal_u_32x4)
{
	EXPECT_TRUE(TestNEON::testSumHorizontal_u_32x4(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestNEON::testSum16Bit4Blocks3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int blocks4 = 10000u;
	constexpr unsigned int sums = blocks4 * 4u;
	constexpr unsigned int elements = sums + 2u;

	static_assert(elements < (1u << 16u), "Invalid size");

	Log::info() << "Test sum of four 3x3 blocks 16 bit (" << blocks4 << " iterations):";

	HighPerformanceStatistic performanceStandard;
	HighPerformanceStatistic performanceNEON;

	Memory topRowMemory = Memory::create<short>(elements);
	Memory centerRowMemory = Memory::create<short>(elements);
	Memory bottomRowMemory = Memory::create<short>(elements);

	short* const topRow = topRowMemory.data<short>();
	short* const centerRow = centerRowMemory.data<short>();
	short* const bottomRow = bottomRowMemory.data<short>();

	Memory sumsStandardMemory = Memory::create<int>(sums);
	int* const sumsStandard = sumsStandardMemory.data<int>();

	Memory sumsNEONMemory = Memory::create<int>(sums);
	int* const sumsNEON = sumsNEONMemory.data<int>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < elements; ++n)
		{
			topRow[n] = short(RandomI::random32(randomGenerator));
			centerRow[n] = short(RandomI::random32(randomGenerator));
			bottomRow[n] = short(RandomI::random32(randomGenerator));
		}

		for (unsigned int n = 0u; n < sums; ++n)
		{
			sumsStandard[n] = 0;
			sumsNEON[n] = 0;
		}

		performanceStandard.start();
		for (unsigned int n = 0u; n < blocks4; ++n)
		{
			for (unsigned int i = 0u; i < 4u; ++i)
			{
				sumsStandard[n * 4u + i] += topRow[4u * n + 0u + i] + topRow[4u * n + 1u + i] + topRow[4u * n + 2u + i]
											+ centerRow[4u * n + 0u + i] + centerRow[4u * n + 1u + i] + centerRow[4u * n + 2u + i]
											+ bottomRow[4u * n + 0u + i] + bottomRow[4u * n + 1u + i] + bottomRow[4u * n + 2u + i];
			}
		}
		performanceStandard.stop();


		performanceNEON.start();
		for (unsigned int n = 0u; n < blocks4; ++n)
		{
			const int32x4_t sum32x4 = CV::NEON::sum16Bit4Blocks3x3(topRow + 4u * n, centerRow + 4u * n, bottomRow + 4u * n);

			vst1q_s32(sumsNEON + n * 4u, sum32x4);
		}
		performanceNEON.stop();

		for (unsigned int n = 0u; n < sums; ++n)
		{
			OCEAN_EXPECT_EQUAL(validation, sumsNEON[n], sumsStandard[n]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Standard performance: Best: " << String::toAString(performanceStandard.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceStandard.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceStandard.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceStandard.medianMseconds(), 3u) << "ms";
	Log::info() << "NEON performance: Best: " << String::toAString(performanceNEON.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceNEON.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceNEON.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceNEON.medianMseconds(), 3u) << "ms";
	Log::info() << "NEON boost factor: Best: " << String::toAString(performanceStandard.best() / performanceNEON.best(), 1u) << "x, worst: " << String::toAString(performanceStandard.worst() / performanceNEON.worst(), 1u) << "x, average: " << String::toAString(performanceStandard.average() / performanceNEON.average(), 1u) << "x, median: " << String::toAString(performanceStandard.median() / performanceNEON.median(), 1u) << "x";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNEON::testAveragingPixels2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test 2x2 averaging of pixel blocks:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, validateAveragePixels2x2<1u, 16u>(CV::NEON::average16Elements1Channel8Bit2x2, randomGenerator));
		OCEAN_EXPECT_TRUE(validation, validateAveragePixels2x2<1u, 32u>(CV::NEON::average32Elements1Channel8Bit2x2, randomGenerator));

		OCEAN_EXPECT_TRUE(validation, validateAveragePixels2x2<2u, 16u>(CV::NEON::average32Elements2Channel16Bit2x2, randomGenerator));
		OCEAN_EXPECT_TRUE(validation, validateAveragePixels2x2<2u, 32u>(CV::NEON::average64Elements2Channel16Bit2x2, randomGenerator));

		OCEAN_EXPECT_TRUE(validation, validateAveragePixels2x2<3u, 16u>(CV::NEON::average48Elements3Channel24Bit2x2, randomGenerator));

		OCEAN_EXPECT_TRUE(validation, validateAveragePixels2x2<4u, 16u>(CV::NEON::average64Elements4Channel32Bit2x2, randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNEON::testMultiply(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test multiply functions:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const uint64_t values64[2] =
		{
			RandomI::random64(randomGenerator),
			RandomI::random64(randomGenerator)
		};

		const uint32_t values32[2] =
		{
			RandomI::random32(randomGenerator),
			RandomI::random32(randomGenerator)
		};

		const uint64x2_t values64_u_64x2 = vld1q_u64(values64);
		const uint32x2_t values32_u_32x2 = vld1_u32(values32);

		const uint64x2_t results_u_64x2 = CV::NEON::multiply(values64_u_64x2, values32_u_32x2);

		uint64_t results[2];
		vst1q_u64(results, results_u_64x2);

		for (unsigned int n = 0u; n < 2u; ++n)
		{
			const uint64_t test = values64[n] * uint64_t(values32[n]);

			OCEAN_EXPECT_EQUAL(validation, test, results[n]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNEON::testCopySign(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test copySign functions:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		uint32_t signReceiver[4];
		int32_t signProvider[4];

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			signReceiver[n] = RandomI::random(randomGenerator, 1000u);
			signProvider[n] = RandomI::random(randomGenerator, -1000, 1000);
		}

		const int32x4_t result_s_32x4 = CV::NEON::copySign(vld1q_u32(signReceiver), vld1q_s32(signProvider));

		int32_t result[4];
		vst1q_s32(result, result_s_32x4);

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			if (signProvider[n] < 0)
			{
				OCEAN_EXPECT_EQUAL(validation, result[n], -int32_t(signReceiver[n]));
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, result[n], int32_t(signReceiver[n]));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNEON::testCastElements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test cast elements functions:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// uint8_t -> float

			std::vector<uint8_t> elements_u(16);

			for (uint8_t& element : elements_u)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			const float32x4x4_t elements_f_32x4x4 = CV::NEON::cast16ElementsNEON(elements_u.data());

			std::vector<float> elements_f(16);

			vst1q_f32(elements_f.data() +  0, elements_f_32x4x4.val[0]);
			vst1q_f32(elements_f.data() +  4, elements_f_32x4x4.val[1]);
			vst1q_f32(elements_f.data() +  8, elements_f_32x4x4.val[2]);
			vst1q_f32(elements_f.data() + 12, elements_f_32x4x4.val[3]);

			for (size_t n = 0; n < elements_u.size(); ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, elements_f[n], float(elements_u[n]));
			}
		}

		{
			// float -> uint8_t

			std::vector<float> elements_f(16);

			for (float& element : elements_f)
			{
				element = float(RandomI::random(randomGenerator, 255u));
			}

			const uint8x16_t elements_u_8x16 = CV::NEON::cast16ElementsNEON(elements_f.data());

			std::vector<uint8_t> elements_u(16);

			vst1q_u8(elements_u.data(), elements_u_8x16);

			for (size_t n = 0; n < elements_u.size(); ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, elements_u[n], uint8_t(elements_f[n]));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNEON::testSumHorizontal_u_32x4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test horizontal sum four uint32_t values functions:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		uint32_t values[4];

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			values[n] = RandomI::random(randomGenerator, 100000u);
		}

		const uint32_t result = CV::NEON::sumHorizontal_u_32x4(vld1q_u32(values));

		const uint32_t test = values[0] + values[1] + values[2] + values[3];

		OCEAN_EXPECT_EQUAL(validation, test, result);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <unsigned int tChannels, unsigned int tSourcePixels>
bool TestNEON::validateAveragePixels2x2(const AveragingFunction averagingFunction, RandomGenerator& randomGenerator)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSourcePixels >= 2u && tSourcePixels % 2u == 0u, "Invalid channel number!");

	ocean_assert(averagingFunction != nullptr);

	constexpr unsigned int tTargetPixels = tSourcePixels / 2u;

	constexpr unsigned int tSourceElements = tSourcePixels * tChannels;
	static_assert(tSourceElements % 2u == 0u);

	constexpr unsigned int tTargetElements = tSourceElements / 2u;

	{
		// testing with heap memory

		std::vector<uint8_t> source0(tSourceElements);
		std::vector<uint8_t> source1(tSourceElements);
		std::vector<uint8_t> target(tTargetElements + 1u);

		for (size_t n = 0; n < source0.size(); ++n)
		{
			source0[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			source1[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		for (size_t n = 0; n < target.size(); ++n)
		{
			target[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		const uint8_t targetLastElement = target.back();

		averagingFunction(source0.data(), source1.data(), target.data());

		if (targetLastElement != target.back())
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		for (unsigned int nTargetPixel = 0u; nTargetPixel < tTargetPixels; ++nTargetPixel)
		{
			const unsigned int nSourcePixel = nTargetPixel * 2u;

			const uint8_t* leftSourcePixel0 = source0.data() + nSourcePixel * tChannels;
			const uint8_t* leftSourcePixel1 = source1.data() + nSourcePixel * tChannels;

			const uint8_t* targetPixel = target.data() + nTargetPixel * tChannels;

			const uint8_t* rightSourcePixel0 = leftSourcePixel0 + tChannels;
			const uint8_t* rightSourcePixel1 = leftSourcePixel1 + tChannels;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const uint32_t value = uint32_t((leftSourcePixel0[n] + rightSourcePixel0[n] + leftSourcePixel1[n] + rightSourcePixel1[n] + 2u) / 4u);
				ocean_assert(value <= 255u);

				if (value > 255u)
				{
					return false;
				}

				const int32_t error = std::abs(int32_t(value) - int32_t(targetPixel[n]));

				if (error > 1)
				{
					return false;
				}
			}
		}
	}

	{
		// testing with stack memory

		std::array<uint8_t, tSourceElements> source0;
		std::array<uint8_t, tSourceElements> source1;
		std::array<uint8_t, tTargetElements + 1u> target;

		for (size_t n = 0; n < source0.size(); ++n)
		{
			source0[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			source1[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		for (size_t n = 0; n < target.size(); ++n)
		{
			target[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		const uint8_t targetLastElement = target.back();

		averagingFunction(source0.data(), source1.data(), target.data());

		if (targetLastElement != target.back())
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		for (unsigned int nTargetPixel = 0u; nTargetPixel < tTargetPixels; ++nTargetPixel)
		{
			const unsigned int nSourcePixel = nTargetPixel * 2u;

			const uint8_t* leftSourcePixel0 = source0.data() + nSourcePixel * tChannels;
			const uint8_t* leftSourcePixel1 = source1.data() + nSourcePixel * tChannels;

			const uint8_t* targetPixel = target.data() + nTargetPixel * tChannels;

			const uint8_t* rightSourcePixel0 = leftSourcePixel0 + tChannels;
			const uint8_t* rightSourcePixel1 = leftSourcePixel1 + tChannels;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const uint32_t value = uint32_t((leftSourcePixel0[n] + rightSourcePixel0[n] + leftSourcePixel1[n] + rightSourcePixel1[n] + 2u) / 4u);
				ocean_assert(value <= 255u);

				if (value > 255u)
				{
					return false;
				}

				const int32_t error = std::abs(int32_t(value) - int32_t(targetPixel[n]));

				if (error > 1)
				{
					return false;
				}
			}
		}
	}

	return true;
}

}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
