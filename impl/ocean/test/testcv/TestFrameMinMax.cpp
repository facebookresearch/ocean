/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameMinMax.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameMinMax.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

#include <limits>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameMinMax::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame MinMax test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetermineMinValue(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetermineMaxValue(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetermineMinMaxValues(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCountElementsOutsideRange(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame MinMax test succeeded.";
	}
	else
	{
		Log::info() << "Frame MinMax test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameMinMax, DetermineMinValue)
{
	EXPECT_TRUE(TestFrameMinMax::testDetermineMinValue(GTEST_TEST_DURATION));
}

TEST(TestFrameMinMax, DetermineMaxValue)
{
	EXPECT_TRUE(TestFrameMinMax::testDetermineMaxValue(GTEST_TEST_DURATION));
}


TEST(TestFrameMinMax, DetermineMinMaxValues_uint8_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMinMax::testDetermineMinMaxValues<uint8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameMinMax, DetermineMinMaxValues_int16_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMinMax::testDetermineMinMaxValues<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameMinMax, DetermineMinMaxValues_uint32_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMinMax::testDetermineMinMaxValues<uint32_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameMinMax, DetermineMinMaxValues_float_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMinMax::testDetermineMinMaxValues<float>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameMinMax, CountElementsOutsideRange)
{
	EXPECT_TRUE(TestFrameMinMax::testCountElementsOutsideRange(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrameMinMax::testDetermineMinValue(const double testDuration)
{
	Log::info() << "Determine minimum value test:";
	Log::info() << " ";

	const unsigned int fixedWidth = 1920u;
	const unsigned int fixedHeight = 1080u;

	Log::info() << "For resolution " << fixedWidth << "x" << fixedHeight << " (1 channel, float):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	HighPerformanceStatistic performance;

	// we use the following dummy locations to ensure that the compiler does not apply unintended optimizations
	float dummyValue = 0.0f;
	unsigned int dummyLocationX = 0u;
	unsigned int dummyLocationY = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		{
			float minValue;
			CV::PixelPosition minLocation((unsigned int)(-1), (unsigned int)(-1));

			const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(fixedWidth, fixedHeight, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

			performance.start();
			CV::FrameMinMax::determineMinValue<float>(frame.constdata<float>(), fixedWidth, fixedHeight, frame.paddingElements(), &minValue, &minLocation);
			performance.stop();

			dummyValue += minValue;
			dummyLocationX += minLocation.x();
			dummyLocationY += minLocation.x();
		}

		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 500u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 500u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

			allSucceeded = testDetermineMinValue<int8_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMinValue<uint8_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMinValue<int16_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMinValue<uint16_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMinValue<int32_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMinValue<uint32_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMinValue<int64_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMinValue<uint64_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMinValue<float>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMinValue<double>(width, height, paddingElements) && allSucceeded;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

	if (allSucceeded)
	{
		if (dummyValue > 5.0f &&  dummyLocationX > 5u && dummyLocationY > 5u)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: succeeded.";
		}
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameMinMax::testDetermineMaxValue(const double testDuration)
{
	Log::info() << "Determine maximum value test:";
	Log::info() << " ";

	const unsigned int fixedWidth = 1920u;
	const unsigned int fixedHeight = 1080u;

	Log::info() << "For resolution " << fixedWidth << "x" << fixedHeight << " (1 channel, float):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	HighPerformanceStatistic performance;

	// we use the following dummy locations to ensure that the compiler does not apply unintended optimizations
	float dummyValue = 0.0f;
	unsigned int dummyLocationX = 0u;
	unsigned int dummyLocationY = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		{
			float maxValue;
			CV::PixelPosition maxLocation((unsigned int)(-1), (unsigned int)(-1));

			const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(fixedWidth, fixedHeight, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

			performance.start();
			CV::FrameMinMax::determineMaxValue<float>(frame.constdata<float>(), fixedWidth, fixedHeight, frame.paddingElements(), &maxValue, &maxLocation);
			performance.stop();

			dummyValue += maxValue;
			dummyLocationX += maxLocation.x();
			dummyLocationY += maxLocation.x();
		}

		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 500u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 500u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

			allSucceeded = testDetermineMaxValue<int8_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint8_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<int16_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint16_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<int32_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint32_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<int64_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint64_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<float>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<double>(width, height, paddingElements) && allSucceeded;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

	if (allSucceeded)
	{
		if (dummyValue > 5.0f &&  dummyLocationX > 5u && dummyLocationY > 5u)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: succeeded.";
		}
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameMinMax::testDetermineMinMaxValues(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Determine minimal and maximal values test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetermineMinMaxValues<uint8_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDetermineMinMaxValues<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDetermineMinMaxValues<uint32_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDetermineMinMaxValues<float>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

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
bool TestFrameMinMax::testDetermineMinMaxValues(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 5u; ++channels)
	{
		Log::info().newLine(channels != 1u);
		Log::info() << "... " << width << "x" << height << ", " << TypeNamer::name<T>() << ", " << channels << " channels";

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
				const T minRangeValueBig = T(max(-1000.0, double(NumericT<T>::minValue())));
				const T maxRangeValueBig = T(min(double(NumericT<T>::maxValue()), 1000.0));

				const T minRangeValueSmall = T(double(minRangeValueBig) + (double(maxRangeValueBig) - double(minRangeValueBig)) * 0.25);
				const T maxRangeValueSmall = T(double(maxRangeValueBig) - (double(maxRangeValueBig) - double(minRangeValueBig)) * 0.25);

				for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
				{
					const bool benchmark = benchmarkIteration == 0u;

					const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
					const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);

					const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					T* frameData = frame.data<T>();

					for (unsigned int n = 0u; n < frame.height() * frame.strideElements(); ++n)
					{
						if (RandomI::random(randomGenerator, 50u) == 0u)
						{
							*frameData++ = T(RandomD::scalar(randomGenerator, double(minRangeValueBig), double(maxRangeValueBig))); // in 2% of all pixels we the big value range
						}
						else
						{
							*frameData++ = T(RandomD::scalar(randomGenerator, double(minRangeValueSmall) * 0.5, double(maxRangeValueSmall) * 0.5)); // in all other cases we use the smaller value range
						}
					}

					std::vector<T> minValues(channels);
					std::vector<T> maxValues(channels);

					performance.startIf(benchmark);

					switch (channels)
					{
						case 1u:
							CV::FrameMinMax::determineMinMaxValues<T, 1u>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), minValues.data(), maxValues.data(), useWorker);
							break;

						case 2u:
							CV::FrameMinMax::determineMinMaxValues<T, 2u>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), minValues.data(), maxValues.data(), useWorker);
							break;

						case 3u:
							CV::FrameMinMax::determineMinMaxValues<T, 3u>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), minValues.data(), maxValues.data(), useWorker);
							break;

						case 4u:
							CV::FrameMinMax::determineMinMaxValues<T, 4u>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), minValues.data(), maxValues.data(), useWorker);
							break;

						case 5u:
							CV::FrameMinMax::determineMinMaxValues<T, 5u>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), minValues.data(), maxValues.data(), useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
					}

					performance.stopIf(benchmark);

					if (!validateDetermineMinMaxValues<T>(frame, minValues.data(), maxValues.data()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}
	}

	return allSucceeded;
}

bool TestFrameMinMax::testCountElementsOutsideRange(const double testDuration)
{
	Log::info() << "Count elements outside range test:";
	Log::info() << " ";

	const unsigned int fixedWidth = 1920u;
	const unsigned int fixedHeight = 1080u;

	Log::info() << "For resolution " << fixedWidth << "x" << fixedHeight << " (1 channel, uint8_t):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	HighPerformanceStatistic performance;

	// A dummy value to ensure that the compiler does not apply unintended optimizations
	uint32_t dummyValue = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// Performance
			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(fixedWidth, fixedHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(frame, /* skip padding */ false, &randomGenerator);

			// Using randomized ranges of values is fine as they don't affect performance.
			const uint8_t rangeStart = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			const uint8_t rangeEnd = uint8_t(RandomI::random(randomGenerator, uint32_t(rangeStart), 255u));
			ocean_assert(rangeStart <= rangeEnd);

			uint32_t countBelowRange = uint32_t(-1);
			uint32_t countAboveRange = uint32_t(-1);

			performance.start();
			const bool countStatus = CV::FrameMinMax::countElementsOutsideRange<uint8_t>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), rangeStart, rangeEnd, &countBelowRange, &countAboveRange);
			performance.stop();

			allSucceeded = allSucceeded && countStatus;

			dummyValue = std::max(1u, dummyValue + countBelowRange + countAboveRange);
		}

		{
			// Validation
			const unsigned int width = RandomI::random(randomGenerator, 1u, 500u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 500u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			allSucceeded = testCountElementsOutsideRange<int8_t>(randomGenerator, width, height, paddingElements) && allSucceeded;
			allSucceeded = testCountElementsOutsideRange<uint8_t>(randomGenerator, width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<int16_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint16_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<int32_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint32_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<int64_t>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<uint64_t>(width, height, paddingElements) && allSucceeded;

			allSucceeded = testDetermineMaxValue<float>(width, height, paddingElements) && allSucceeded;
			allSucceeded = testDetermineMaxValue<double>(width, height, paddingElements) && allSucceeded;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

	if (allSucceeded)
	{
		if (dummyValue > 0u)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: succeeded.";
		}
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameMinMax::testDetermineMinValue(const unsigned int width, const unsigned int height, const unsigned int paddingElements)
{
	ocean_assert(width >= 1u && height >= 1u);

	const T minRangeValue = T(max(-1000.0, double(NumericT<T>::minValue())));
	const T maxRangeValue = T(min(double(NumericT<T>::maxValue()), 1000.0));

	Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);

	RandomGenerator randomGenerator;

	for (unsigned int y = 0u; y < height; ++y)
	{
		T* data = frame.row<T>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			data[x] = T(RandomD::scalar(randomGenerator, double(minRangeValue), double(maxRangeValue)));
		}
	}

	T minValue = T(RandomD::scalar(randomGenerator, double(minRangeValue), double(maxRangeValue)));
	CV::PixelPosition minLocation((unsigned int)(-1), (unsigned int)(-1));
	CV::FrameMinMax::determineMinValue<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), &minValue, &minLocation);

	T justMinValue = T(Random::scalar(randomGenerator, double(minRangeValue), double(maxRangeValue)));
	CV::FrameMinMax::determineMinValue<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), &justMinValue, nullptr);

	CV::PixelPosition justMinLocation((unsigned int)(-1), (unsigned int)(-1));
	CV::FrameMinMax::determineMinValue<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), nullptr, &justMinLocation);

	double testMinValue = NumericD::maxValue();
	int testMinLocationX = -1;
	int testMinLocationY = -1;

	// we start from top left and end bottom right, as the actual implementation
	for (unsigned int y = 0u; y < height; ++y)
	{
		const T* data = frame.constrow<T>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			const double value = double(data[x]);

			if (value < testMinValue)
			{
				testMinLocationX = int(x);
				testMinLocationY = int(y);

				testMinValue = value;
			}
		}
	}

	ocean_assert(testMinLocationX >= 0 && testMinLocationX >= 0);

	if (testMinLocationX < 0 || testMinLocationX >= int(width))
	{
		return false;
	}

	if (testMinLocationY < 0 || testMinLocationY >= int(height))
	{
		return false;
	}

	if (double(minValue) < double(minRangeValue) || double(minValue) > double(maxRangeValue))
	{
		return false;
	}

	if (double(minValue) != testMinValue)
	{
		return false;
	}

	if (double(justMinValue) != testMinValue)
	{
		return false;
	}

	if (minLocation.x() >= width || minLocation.y() >= height)
	{
		return false;
	}

	if (justMinLocation.x() >= width || justMinLocation.y() >= height)
	{
		return false;
	}

	if (testMinLocationX != int(minLocation.x()) || testMinLocationY != int(minLocation.y()))
	{
		// we do not have a guarantee that we have the same location when having more than one location with best value
		// lust, we check the value only

		ocean_assert(double(minValue) == testMinValue);

		const T value = frame.constpixel<T>(minLocation.x(), minLocation.y())[0];

		if (double(value) != testMinValue)
		{
			return false;
		}
	}

	if (minLocation != justMinLocation)
	{
		return false;
	}

	return true;
}

template <typename T>
bool TestFrameMinMax::testDetermineMaxValue(const unsigned int width, const unsigned int height, const unsigned int paddingElements)
{
	ocean_assert(width >= 1u && height >= 1u);

	const T minRangeValue = T(max(-1000.0, double(NumericT<T>::minValue())));
	const T maxRangeValue = T(min(double(NumericT<T>::maxValue()), 1000.0));

	Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);

	RandomGenerator randomGenerator;

	for (unsigned int y = 0u; y < height; ++y)
	{
		T* data = frame.row<T>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			data[x] = T(RandomD::scalar(randomGenerator, double(minRangeValue), double(maxRangeValue)));
		}
	}

	T maxValue = T(RandomD::scalar(randomGenerator, double(minRangeValue), double(maxRangeValue)));
	CV::PixelPosition maxLocation((unsigned int)(-1), (unsigned int)(-1));
	CV::FrameMinMax::determineMaxValue<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), &maxValue, &maxLocation);

	T justMaxValue = T(Random::scalar(randomGenerator, double(minRangeValue), double(maxRangeValue)));
	CV::FrameMinMax::determineMaxValue<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), &justMaxValue, nullptr);

	CV::PixelPosition justMaxLocation((unsigned int)(-1), (unsigned int)(-1));
	CV::FrameMinMax::determineMaxValue<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), nullptr, &justMaxLocation);

	double testMaxValue = NumericD::minValue();
	int testMaxLocationX = -1;
	int testMaxLocationY = -1;

	// we start from top left and end bottom right, as the actual implementation
	for (unsigned int y = 0u; y < height; ++y)
	{
		const T* data = frame.constrow<T>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			const double value = double(data[x]);

			if (value > testMaxValue)
			{
				testMaxLocationX = int(x);
				testMaxLocationY = int(y);

				testMaxValue = value;
			}
		}
	}

	ocean_assert(testMaxLocationX >= 0 && testMaxLocationX >= 0);

	if (testMaxLocationX < 0 || testMaxLocationX >= int(width))
	{
		return false;
	}

	if (testMaxLocationY < 0 || testMaxLocationY >= int(height))
	{
		return false;
	}

	if (double(maxValue) < double(minRangeValue) || double(maxValue) > double(maxRangeValue))
	{
		return false;
	}

	if (double(maxValue) != testMaxValue)
	{
		return false;
	}

	if (double(justMaxValue) != testMaxValue)
	{
		return false;
	}

	if (maxLocation.x() >= width || maxLocation.y() >= height)
	{
		return false;
	}

	if (justMaxLocation.x() >= width || justMaxLocation.y() >= height)
	{
		return false;
	}

	if (testMaxLocationX != int(maxLocation.x()) || testMaxLocationY != int(maxLocation.y()))
	{
		// we do not have a guarantee that we have the same location when having more than one location with best value
		// lust, we check the value only

		ocean_assert(double(maxValue) == testMaxValue);

		const T value = frame.constpixel<T>(maxLocation.x(), maxLocation.y())[0];

		if (double(value) != testMaxValue)
		{
			return false;
		}
	}

	if (maxLocation != justMaxLocation)
	{
		return false;
	}

	return true;
}

template <typename T>
bool TestFrameMinMax::testCountElementsOutsideRange(RandomGenerator& randomGenerator, const uint32_t width, const uint32_t height, const uint32_t paddingElements)
{
	ocean_assert(width >= 1u && height >= 1u);

	const T rangeStart = T(RandomT<T>::random(randomGenerator, NumericT<T>::minValue(), NumericT<T>::maxValue()));
	const T rangeEnd = T(RandomT<T>::random(randomGenerator, rangeStart, NumericT<T>::maxValue()));
	ocean_assert(rangeStart <= rangeEnd);

	Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
	CV::CVUtilities::randomizeFrame(frame, /* skip padding */ false, &randomGenerator);

	uint32_t elementsBelowRange = RandomI::random(randomGenerator, 100000u);
	uint32_t elementsAboveRange = RandomI::random(randomGenerator, 100000u);
	bool executionSuccessful = CV::FrameMinMax::countElementsOutsideRange<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), rangeStart, rangeEnd, &elementsBelowRange, &elementsAboveRange);

	uint32_t elementsBelowRangeOnly = RandomI::random(randomGenerator, 100000u);
	executionSuccessful = CV::FrameMinMax::countElementsOutsideRange<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), rangeStart, rangeEnd, &elementsBelowRangeOnly, nullptr) && executionSuccessful;

	uint32_t elementsAboveRangeOnly = RandomI::random(randomGenerator, 100000u);
	executionSuccessful = CV::FrameMinMax::countElementsOutsideRange<T>(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), rangeStart, rangeEnd, nullptr, &elementsAboveRangeOnly) && executionSuccessful;

	if (executionSuccessful == false)
	{
		return false;
	}

	uint32_t elementsBelowRangeGroundtruth = 0u;
	uint32_t elementsAboveRangeGroundtruth = 0u;

	for (uint32_t y = 0u; y < frame.height(); ++y)
	{
		const T* const frameRow = frame.constrow<T>(y);

		for (uint32_t x = 0u; x < frame.width(); ++x)
		{
			if (frameRow[x] < rangeStart)
			{
				++elementsBelowRangeGroundtruth;
			}
			else if (frameRow[x] >= rangeEnd)
			{
				++elementsAboveRangeGroundtruth;
			}
		}
	}

	const bool allSucceeded = elementsBelowRange == elementsBelowRangeGroundtruth
								&& elementsAboveRange == elementsAboveRangeGroundtruth
								&& elementsBelowRangeOnly == elementsBelowRangeGroundtruth
								&& elementsAboveRangeOnly == elementsAboveRangeGroundtruth;

	return allSucceeded;
}

template <typename T>
bool TestFrameMinMax::validateDetermineMinMaxValues(const Frame& frame, const T* const minValues, const T* const maxValues)
{
	ocean_assert(frame.isValid());
	ocean_assert(minValues != nullptr && maxValues != nullptr);

	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::dataType<T>());

	const unsigned int channels = frame.channels();

	std::vector<T> testMinValues(channels, NumericT<T>::maxValue());
	std::vector<T> testMaxValues(channels, NumericT<T>::minValue());

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		const T* const row = frame.constrow<T>(y);

		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			const T* const pixel = row + x * channels;

			for (unsigned int c = 0u; c < channels; ++c)
			{
				testMinValues[c] = std::min(testMinValues[c], pixel[c]);
				testMaxValues[c] = std::max(testMaxValues[c], pixel[c]);
			}
		}
	}

	for (unsigned int c = 0u; c < channels; ++c)
	{
		if (minValues[c] != testMinValues[c])
		{
			return false;
		}

		if (maxValues[c] != testMaxValues[c])
		{
			return false;
		}
	}

	return true;
}

}

}

}
