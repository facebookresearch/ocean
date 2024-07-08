/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestSumSquareDifferencesNoCenter.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/SumSquareDifferencesNoCenter.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestSumSquareDifferencesNoCenter::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Advanced non-center sum square differences test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPatch8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced non-center sum square differences test succeeded.";
	}
	else
	{
		Log::info() << "Advanced non-center sum square differences test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSumSquareDifferencesNoCenter, Patch8BitPerChannel)
{
	EXPECT_TRUE(TestSumSquareDifferencesNoCenter::testPatch8BitPerChannel(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSumSquareDifferencesNoCenter::testPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pixel accurate SSD between two patches:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatch8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestSumSquareDifferencesNoCenter::testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << "):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int paddingElements0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame0(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements0);
		Frame frame1(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements1);

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, tPatchSize_2, width0 - tPatchSize_2 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, tPatchSize_2, height0 - tPatchSize_2 - 1u);

			centersX1[n] = RandomI::random(randomGenerator, tPatchSize_2, width1 - tPatchSize_2 - 1u);
			centersY1[n] = RandomI::random(randomGenerator, tPatchSize_2, height1 - tPatchSize_2 - 1u);
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::SumSquareDifferencesNoCenterBase::patch8BitPerChannel(data0, data1, tChannels, tPatchSize, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::SumSquareDifferencesNoCenterBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize >= 5u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsSSE[n] = CV::Advanced::SumSquareDifferencesNoCenterSSE::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
						}

						break;
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tPatchSize >= 5u)
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsNEON[n] = CV::Advanced::SumSquareDifferencesNoCenterNEON::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
						}
					}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::SumSquareDifferencesNoCenter::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			uint32_t ssdTest = 0u;

			const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, centersY0[n] - tPatchSize_2);
			const uint8_t* pixel1 = frame1.constpixel<uint8_t>(centersX1[n] - tPatchSize_2, centersY1[n] - tPatchSize_2);

			for (unsigned int y = 0u; y < tPatchSize; ++y)
			{
				for (unsigned int i = 0u; i < tChannels * tPatchSize; ++i)
				{
					const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
					ssdTest += uint32_t(value * value);
				}

				pixel0 += frame0.strideElements();
				pixel1 += frame1.strideElements();
			}

			pixel0 = frame0.constpixel<uint8_t>(centersX0[n], centersY0[n]);
			pixel1 = frame1.constpixel<uint8_t>(centersX1[n], centersY1[n]);

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
				ssdTest -= uint32_t(value * value);
			}

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

	if (performanceNaive.measurements() != 0u)
	{
		Log::info() << "   Naive: [" << performanceNaive.bestMseconds() << ", " << performanceNaive.medianMseconds() << ", " << performanceNaive.worstMseconds() << "] ms";
	}

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
	}

	if (performanceSSE.measurements() != 0u)
	{
		Log::info() << "     SSE: [" << performanceSSE.bestMseconds() << ", " << performanceSSE.medianMseconds() << ", " << performanceSSE.worstMseconds() << "] ms";
	}

	if (performanceNEON.measurements() != 0u)
	{
		Log::info() << "    NEON: [" << performanceNEON.bestMseconds() << ", " << performanceNEON.medianMseconds() << ", " << performanceNEON.worstMseconds() << "] ms";
	}

	ocean_assert(performanceDefault.measurements() != 0u);
	Log::info() << " Default: [" << performanceDefault.bestMseconds() << ", " << performanceDefault.medianMseconds() << ", " << performanceDefault.worstMseconds() << "] ms";

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

}
