/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterSeparable.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterSeparable.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterSeparable::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Separable filter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExtremeDimensions(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalDimensions(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReusableMemory<uint8_t, uint32_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testReusableMemory<float, float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReusableMemoryComfort<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testReusableMemoryComfort<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableFilterUniversalExtremeResolutions<char>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<unsigned char>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<short>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<unsigned short>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<int>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<unsigned int>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<float>(testDuration, worker) && allSucceeded;
	allSucceeded = testSeparableFilterUniversalExtremeResolutions<double>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableFilterUniversal<unsigned char>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableFilterUniversal<short>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableFilterUniversal<int>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableFilterUniversal<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableFilterUniversal<double>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Separable filter test succeeded.";
	}
	else
	{
		Log::info() << "Separable filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterSeparable, SeparableFilterExtremeResolutionsUnsignedChar)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testExtremeDimensions<unsigned char, unsigned int>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterExtremeResolutionsFloat)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testExtremeDimensions<float, float>(GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar1Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 1u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar2Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 2u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar3Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 3u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar4Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 4u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, SeparableFilterFloat1Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 1u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat2Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 2u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat3Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 3u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat4Channel3x3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 4u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar1Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 1u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar2Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 2u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar3Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 3u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar4Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 4u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, SeparableFilterFloat1Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 1u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat2Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 2u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat3Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 3u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat4Channel5x9)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 4u, 5u, 9u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar1Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 1u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar2Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 2u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar3Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 3u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterUnsignedChar4Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<unsigned char, unsigned int>(1920u, 1080u, 4u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, SeparableFilterFloat1Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 1u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat2Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 2u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat3Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 3u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSeparable, SeparableFilterFloat4Channel11x7)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSeparable::testFilter8BitPerChannel<float, float>(1920u, 1080u, 4u, 11u, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSeparable, ReusableMemory_uint8)
{
	EXPECT_TRUE((TestFrameFilterSeparable::testReusableMemory<uint8_t, uint32_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterSeparable, ReusableMemory_float)
{
	EXPECT_TRUE((TestFrameFilterSeparable::testReusableMemory<float, float>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterSeparable, ReusableMemoryComfort_uint8)
{
	EXPECT_TRUE((TestFrameFilterSeparable::testReusableMemoryComfort<uint8_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterSeparable, ReusableMemoryComfort_float)
{
	EXPECT_TRUE((TestFrameFilterSeparable::testReusableMemoryComfort<float>(GTEST_TEST_DURATION)));
}


TEST(TestFrameFilterSeparable, SeparableFilterUniversalExtremeResolutionsShort)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversalExtremeResolutions<short>(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalExtremeResolutionsInt)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversalExtremeResolutions<int>(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalExtremeResolutionsUnsignedChar)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversalExtremeResolutions<unsigned char>(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalExtremeResolutionsDouble)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversalExtremeResolutions<double>(GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterSeparable, SeparableFilterUniversalShort1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<short>(1920u, 1080u, 1u, 3u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalShort2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<short>(1920u, 1080u, 2u, 3u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalShort3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<short>(1920u, 1080u, 3u, 5u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalShort4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<short>(1920u, 1080u, 4u, 9u, 7u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterSeparable, SeparableFilterUniversalUnsignedChar1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<unsigned char>(1920u, 1080u, 1u, 3u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalUnsignedChar2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<unsigned char>(1920u, 1080u, 2u, 3u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalUnsignedChar3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<unsigned char>(1920u, 1080u, 3u, 5u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalUnsignedChar4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<unsigned char>(1920u, 1080u, 4u, 9u, 7u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterSeparable, SeparableFilterUniversalFloat1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<float>(1920u, 1080u, 1u, 3u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalFloat2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<float>(1920u, 1080u, 2u, 3u, 7u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalFloat3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<float>(1920u, 1080u, 3u, 5u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSeparable, SeparableFilterUniversalFloat4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSeparable::testSeparableFilterUniversal<float>(1920u, 1080u, 4u, 3u, 3u, GTEST_TEST_DURATION, worker));
}

#endif

bool TestFrameFilterSeparable::testExtremeDimensions(const double testDuration, Worker& worker)
{
	Log::info() << "Testing extreme frame dimensions:";

	bool allSucceeded = true;

	allSucceeded = testExtremeDimensions<unsigned char, unsigned int>(testDuration, worker) && allSucceeded;
	allSucceeded = testExtremeDimensions<float, float>(testDuration, worker) && allSucceeded;

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

template <typename T, typename TFilter>
bool TestFrameFilterSeparable::testExtremeDimensions(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#else
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#endif

	RandomGenerator randomGenerator;

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 64u);

		unsigned int horizontalFilterSize = RandomI::random(1u, width);
		unsigned int verticalFilterSize = RandomI::random(1u, height);

		// we need odd filter sizes but also smaller than width and height
		if ((horizontalFilterSize & 0x01u) == 0x00u)
		{
			horizontalFilterSize--;
		}

		if ((verticalFilterSize & 0x01u) == 0x00u)
		{
			verticalFilterSize--;
		}

		const std::vector<TFilter> horizontalFilter(randomFilter<TFilter>(randomGenerator, horizontalFilterSize));
		const std::vector<TFilter> verticalFilter(randomFilter<TFilter>(randomGenerator, verticalFilterSize));

		for (unsigned int channels = 1u; channels <= 5u; ++channels)
		{
			for (const bool useWorker : {true, false})
			{
				const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(channels);

				const Frame source = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame target = CV::CVUtilities::randomizedFrame(source.frameType(), &randomGenerator);

				const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				constexpr CV::FrameFilterSeparable::ReusableMemory* reusableMemory = nullptr;

				if (!CV::FrameFilterSeparable::filter<T, TFilter>(source.constdata<T>(), target.data<T>(), source.width(), source.height(), source.channels(), source.paddingElements(), target.paddingElements(), horizontalFilter.data(), horizontalFilterSize, verticalFilter.data(), verticalFilterSize, useWorker ? &worker : nullptr, reusableMemory, processorInstructions))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				const std::vector<float> normalizedHorizontalFilter(normalizedFilter(horizontalFilter));
				const std::vector<float> normalizedVerticalFilter(normalizedFilter(verticalFilter));

				double averageAbsError = NumericD::maxValue();
				double maximalAbsError = NumericD::maxValue();
				TestFrameFilterSeparable::validateFilter<T>(source.constdata<T>(), target.constdata<T>(), source.width(), source.height(), source.channels(), normalizedHorizontalFilter, normalizedVerticalFilter, &averageAbsError, &maximalAbsError, nullptr, source.paddingElements(), target.paddingElements(), 0u);
				if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	return allSucceeded;
}

bool TestFrameFilterSeparable::testNormalDimensions(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing normal frame dimensions:";
	Log::info() << " ";

	const unsigned int widths[] =  {640u, 800u, 1280u, 1281u, 1920u, 3840u};
	const unsigned int heights[] = {480u, 640u,  720u,  723u, 1080u, 2160u};

	const IndexPair32 filterCombinations[] = {IndexPair32(3u, 3u), IndexPair32(5u, 9u), IndexPair32(11u, 7u)};

	bool allSucceeded = true;

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		if (n != 0u)
		{
			Log::info() << " ";
			Log::info() << "-";
			Log::info() << " ";
		}

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "Testing frame size " << width << "x" << height << ":";

		for (unsigned int filterIndex = 0u; filterIndex < sizeof(filterCombinations) / sizeof(filterCombinations[0]); ++filterIndex)
		{
			const IndexPair32& filterCombination = filterCombinations[filterIndex];

			for (unsigned int channels = 1u; channels <= 4u; ++channels)
			{
				Log::info() << " ";
				Log::info() << "... with " << channels << " channels with filter kernel size " << filterCombination.first << "x" << filterCombination.second << ":";
				Log::info() << " ";

				Log::info() << "... with data type 'unsigned char'";
				allSucceeded = testFilter8BitPerChannel<unsigned char, unsigned int>(width, height, channels, filterCombination.first, filterCombination.second, testDuration, worker) && allSucceeded;

				Log::info() << " ";

				Log::info() << "... with data type 'float'";
				allSucceeded = testFilter8BitPerChannel<float, float>(width, height, channels, filterCombination.first, filterCombination.second, testDuration, worker) && allSucceeded;
			}
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Normal frame dimension test succeeded.";
	}
	else
	{
		Log::info() << "Normal frame dimension test FAILED!";
	}

	return allSucceeded;
}

template <typename T, typename TFilter>
bool TestFrameFilterSeparable::testReusableMemory(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	constexpr unsigned int filterSize = 7u;

	Log::info() << "Testing reusable memory for 3 channels '" << TypeNamer::name<T>() << "' image with resolution " << width << "x" << height << ", with filter size " << filterSize << ":";
	Log::info() << " ";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#else
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#endif

	RandomGenerator randomGenerator;

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	bool allSucceeded = true;

	HighPerformanceStatistic performanceStandard;
	HighPerformanceStatistic performanceReusableMemory;

	CV::FrameFilterSeparable::ReusableMemory reusableMemory;

	for (const bool useReusableMemory : {false, true})
	{
		HighPerformanceStatistic& performance = useReusableMemory ? performanceReusableMemory : performanceStandard;

		const Timestamp startTimestamp(true);

		do
		{
			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 3u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator, true);
			Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator, true);

			const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const std::vector<TFilter> horizontalFilter(randomFilter<TFilter>(randomGenerator, filterSize));
			const std::vector<TFilter> verticalFilter(randomFilter<TFilter>(randomGenerator, filterSize));

			performance.start();
				CV::FrameFilterSeparable::filter<T, TFilter>(frame.constdata<T>(), target.data<T>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), target.paddingElements(), horizontalFilter.data(), filterSize, verticalFilter.data(), filterSize, nullptr, useReusableMemory ? &reusableMemory : nullptr, processorInstructions);
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			const std::vector<float> normalizedHorizontalFilter(normalizedFilter(horizontalFilter));
			const std::vector<float> normalizedVerticalFilter(normalizedFilter(verticalFilter));

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), frame.channels(), normalizedHorizontalFilter, normalizedVerticalFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
			if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Standard performance: Best: " << performanceStandard.bestMseconds() << "ms, worst: " << performanceStandard.worstMseconds() << "ms, average: " << performanceStandard.averageMseconds() << ", median: " << performanceStandard.medianMseconds() << "ms";
	Log::info() << "Reusable memory performance: Best: " << performanceReusableMemory.bestMseconds() << "ms, worst: " << performanceReusableMemory.worstMseconds() << "ms, average: " << performanceReusableMemory.averageMseconds() << "ms, median: " << performanceReusableMemory.medianMseconds() << "ms";
	Log::info() << "Reusable memory boost factor: Best: " << String::toAString(performanceStandard.best() / performanceReusableMemory.best(), 2u) << "x, worst: " << String::toAString(performanceStandard.worst() / performanceReusableMemory.worst(), 2u) << "x, average: " << String::toAString(performanceStandard.average() / performanceReusableMemory.average(), 2u) << "x, median: " << String::toAString(performanceStandard.median() / performanceReusableMemory.median(), 2u) << "x";

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

template <typename T>
bool TestFrameFilterSeparable::testReusableMemoryComfort(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	constexpr unsigned int filterSize = 7u;

	Log::info() << "Testing comfort reusable memory for 3 channels '" << TypeNamer::name<T>() << "' image with resolution " << width << "x" << height << ", with filter size " << filterSize << ":";
	Log::info() << " ";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = std::is_same<T, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<T, float>::value ? 0.1 : 6.0;
#else
	const double averageErrorThreshold = std::is_same<T, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<T, float>::value ? 0.1 : 6.0;
#endif

	RandomGenerator randomGenerator;

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	bool allSucceeded = true;

	HighPerformanceStatistic performanceStandard;
	HighPerformanceStatistic performanceReusableMemory;

	CV::FrameFilterSeparable::ReusableMemory reusableMemory;

	for (const bool useReusableMemory : {false, true})
	{
		HighPerformanceStatistic& performance = useReusableMemory ? performanceReusableMemory : performanceStandard;

		const Timestamp startTimestamp(true);

		do
		{
			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 3u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator, true);
			Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator, true);

			const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const Indices32 horizontalFilter(randomFilter<uint32_t>(randomGenerator, filterSize));
			const Indices32 verticalFilter(randomFilter<uint32_t>(randomGenerator, filterSize));

			performance.start();
				CV::FrameFilterSeparable::filter(frame, target, horizontalFilter, verticalFilter, nullptr, useReusableMemory ? &reusableMemory : nullptr, processorInstructions);
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			const std::vector<float> normalizedHorizontalFilter(normalizedFilter(horizontalFilter));
			const std::vector<float> normalizedVerticalFilter(normalizedFilter(verticalFilter));

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), frame.channels(), normalizedHorizontalFilter, normalizedVerticalFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
			if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Standard performance: Best: " << performanceStandard.bestMseconds() << "ms, worst: " << performanceStandard.worstMseconds() << "ms, average: " << performanceStandard.averageMseconds() << ", median: " << performanceStandard.medianMseconds() << "ms";
	Log::info() << "Reusable memory performance: Best: " << performanceReusableMemory.bestMseconds() << "ms, worst: " << performanceReusableMemory.worstMseconds() << "ms, average: " << performanceReusableMemory.averageMseconds() << "ms, median: " << performanceReusableMemory.medianMseconds() << "ms";
	Log::info() << "Reusable memory boost factor: Best: " << String::toAString(performanceStandard.best() / performanceReusableMemory.best(), 2u) << "x, worst: " << String::toAString(performanceStandard.worst() / performanceReusableMemory.worst(), 2u) << "x, average: " << String::toAString(performanceStandard.average() / performanceReusableMemory.average(), 2u) << "x, median: " << String::toAString(performanceStandard.median() / performanceReusableMemory.median(), 2u) << "x";

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

template <typename T, typename TFilter>
bool TestFrameFilterSeparable::testFilter8BitPerChannel(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker)
{
	ocean_assert(width >= horizontalFilterSize && height >= verticalFilterSize);
	ocean_assert(testDuration > 0.0);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#else
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#endif

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(channels);

	RandomGenerator randomGenerator;

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator, true);
			Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator, true);

			const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const std::vector<TFilter> horizontalFilter(randomFilter<TFilter>(randomGenerator, horizontalFilterSize));
			const std::vector<TFilter> verticalFilter(randomFilter<TFilter>(randomGenerator, verticalFilterSize));

			constexpr CV::FrameFilterSeparable::ReusableMemory* reusableMemory = nullptr;

			performance.start();
				CV::FrameFilterSeparable::filter<T, TFilter>(frame.constdata<T>(), target.data<T>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), target.paddingElements(), horizontalFilter.data(), horizontalFilterSize, verticalFilter.data(), verticalFilterSize, useWorker, reusableMemory, processorInstructions);
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			const std::vector<float> normalizedHorizontalFilter(normalizedFilter(horizontalFilter));
			const std::vector<float> normalizedVerticalFilter(normalizedFilter(verticalFilter));

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), channels, normalizedHorizontalFilter, normalizedVerticalFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
			if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << ", median: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";

		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1) << "x";
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

template <typename T>
bool TestFrameFilterSeparable::testSeparableFilterUniversalExtremeResolutions(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Universal separable filter test for data type '" << TypeNamer::name<T>() << "' and extreme resolutions:";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 0.5;
	const double maximalErrorThreshold = 2.0;
#else
	const double averageErrorThreshold = 0.5;
	const double maximalErrorThreshold = 1.0;
#endif

	RandomGenerator randomGenerator;

	unsigned long long iterations = 0ull;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 64u);
		const unsigned int height = RandomI::random(1u, 64u);

		unsigned int horizontalFilterSize = RandomI::random(1u, width);
		unsigned int verticalFilterSize = RandomI::random(1u, height);

		if (horizontalFilterSize % 2u == 0u)
		{
			horizontalFilterSize--;
		}

		if (verticalFilterSize % 2u == 0u)
		{
			verticalFilterSize--;
		}

		std::vector<float> horizontalFilter(horizontalFilterSize);
		std::vector<float> verticalFilter(verticalFilterSize);

		float horizontalFilterSum = 0.0f;
		for (size_t n = 0; n < horizontalFilter.size(); ++n)
		{
			horizontalFilter[n] = RandomF::scalar(randomGenerator, 0.0001f, 10.0f);
			horizontalFilterSum += horizontalFilter[n];
		}

		float verticalFilterSum = 0.0f;
		for (size_t n = 0; n < verticalFilter.size(); ++n)
		{
			verticalFilter[n] = RandomF::scalar(randomGenerator, 0.0001f, 10.0f);
			verticalFilterSum += verticalFilter[n];
		}

		ocean_assert(NumericF::isNotEqualEps(horizontalFilterSum));
		for (size_t n = 0; n < horizontalFilter.size(); ++n)
		{
			horizontalFilter[n] /= horizontalFilterSum;
		}

		ocean_assert(NumericF::isNotEqualEps(verticalFilterSum));
		for (size_t n = 0; n < verticalFilter.size(); ++n)
		{
			verticalFilter[n] /= verticalFilterSum;
		}

		for (unsigned int channels = 1u; channels <= 5u; ++channels)
		{
			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator, true);
			Frame targetFrame = CV::CVUtilities::randomizedFrame(sourceFrame.frameType(), &randomGenerator, true);

			Worker* useWorker = (iterations % 2ull == 0ull) ? nullptr : &worker;

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			if (!CV::FrameFilterSeparable::filterUniversal<T>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements(), horizontalFilter.data(), (unsigned int)(horizontalFilter.size()), verticalFilter.data(), (unsigned int)(verticalFilter.size()), useWorker))
			{
				allSucceeded = false;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateFilter<T>(sourceFrame.constdata<T>(), targetFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), horizontalFilter, verticalFilter, &averageAbsError, &maximalAbsError, nullptr, sourceFrame.paddingElements(), targetFrame.paddingElements(), 0u);

			if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
			{
				allSucceeded = false;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameFilterSeparable::testSeparableFilterUniversal(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Universal separable filter test for data type '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testSeparableFilterUniversal<T>(640u, 480u, n, 3u, 3u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testSeparableFilterUniversal<T>(640u, 480u, n, 5u, 9u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testSeparableFilterUniversal<T>(640u, 480u, n, 11u, 7u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testSeparableFilterUniversal<T>(1920u, 1080u, n, 3u, 3u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testSeparableFilterUniversal<T>(1920u, 1080u, n, 5u, 9u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testSeparableFilterUniversal<T>(1920u, 1080u, n, 11u, 7u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	if (allSucceeded)
		Log::info() << "Universal separable filter test succeeded.";
	else
		Log::info() << "Universal separable filter test FAILED!";

	return allSucceeded;
}

template <typename T>
bool TestFrameFilterSeparable::testSeparableFilterUniversal(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	if (width > 64u)
	{
		Log::info() << "... filtering " << width << "x" << height << ", " << channels << " channels and filter size " << horizontalFilterSize << "x" << verticalFilterSize << ":";
	}

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	double sumAverageError = 0.0;
	double maximalError = 0.0;
	unsigned long long measurements = 0ull;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator, true);
			Frame targetFrame = CV::CVUtilities::randomizedFrame(sourceFrame.frameType(), &randomGenerator, true);

			std::vector<float> horizontalFilter(horizontalFilterSize);
			std::vector<float> verticalFilter(verticalFilterSize);

			float horizontalFilterSum = 0.0f;
			for (size_t n = 0; n < horizontalFilter.size(); ++n)
			{
				horizontalFilter[n] = RandomF::scalar(randomGenerator, 0.0001f, 10.0f);
				horizontalFilterSum += horizontalFilter[n];
			}

			float verticalFilterSum = 0.0f;
			for (size_t n = 0; n < verticalFilter.size(); ++n)
			{
				verticalFilter[n] = RandomF::scalar(randomGenerator, 0.0001f, 10.0f);
				verticalFilterSum += verticalFilter[n];
			}

			ocean_assert(NumericF::isNotEqualEps(horizontalFilterSum));
			for (size_t n = 0; n < horizontalFilter.size(); ++n)
			{
				horizontalFilter[n] /= horizontalFilterSum;
			}

			ocean_assert(NumericF::isNotEqualEps(verticalFilterSum));
			for (size_t n = 0; n < verticalFilter.size(); ++n)
			{
				verticalFilter[n] /= verticalFilterSum;
			}

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
				const bool localResult = CV::FrameFilterSeparable::filterUniversal<T>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements(), horizontalFilter.data(), (unsigned int)(horizontalFilter.size()), verticalFilter.data(), (unsigned int)(verticalFilter.size()), useWorker);
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!localResult)
			{
				allSucceeded = false;
			}

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateFilter<T>(sourceFrame.constdata<T>(), targetFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), horizontalFilter, verticalFilter, &averageAbsError, &maximalAbsError, nullptr, sourceFrame.paddingElements(), targetFrame.paddingElements(), 0u);

			sumAverageError += averageAbsError;
			maximalError = max(maximalError, maximalAbsError);
			measurements++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (width > 64u)
	{
		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 0.5;
	const double maximalErrorThreshold = 2.0;
#else
	const double averageErrorThreshold = 0.5;
	const double maximalErrorThreshold = 1.0;
#endif

	ocean_assert(measurements != 0ull);
	const double averageAbsError = sumAverageError / double(measurements);

	allSucceeded = allSucceeded && averageAbsError <= averageErrorThreshold && maximalError <= maximalErrorThreshold;

	if (width > 64u)
	{
		Log::info() << "Validation: average error: " << String::toAString(averageAbsError, 2u) << ", maximal error: " << String::toAString(maximalError, 2u);

		if (!allSucceeded)
		{
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

}

}

}
