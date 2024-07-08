/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameVariance.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameVariance.h"

#include "ocean/math/Approximation.h"
#include "ocean/math/Math.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameVariance::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Variance test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned int window : {5u, 11u, 21u})
	{
		allSucceeded = testDeviation1Channel8Bit<int8_t>(width, height, window, testDuration) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testDeviation1Channel8Bit<uint8_t>(width, height, window, testDuration) && allSucceeded;

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	allSucceeded = testFrameStatistics(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Variance test succeeded.";
	}
	else
	{
		Log::info() << "Frame Variance test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameVariance, TestDeviation1Channel8Bit_int8_5Window)
{
	Worker worker;
	EXPECT_TRUE((TestFrameVariance::testDeviation1Channel8Bit<int8_t>(1920u, 1080u, 5u, GTEST_TEST_DURATION)));
}

TEST(TestFrameVariance, TestDeviation1Channel8Bit_uint8_5Window)
{
	Worker worker;
	EXPECT_TRUE((TestFrameVariance::testDeviation1Channel8Bit<uint8_t>(1920u, 1080u, 5u, GTEST_TEST_DURATION)));
}


TEST(TestFrameVariance, TestDeviation1Channel8Bit_int8_11Window)
{
	Worker worker;
	EXPECT_TRUE((TestFrameVariance::testDeviation1Channel8Bit<int8_t>(1920u, 1080u, 11u, GTEST_TEST_DURATION)));
}

TEST(TestFrameVariance, TestDeviation1Channel8Bit_uint8_11Window)
{
	Worker worker;
	EXPECT_TRUE((TestFrameVariance::testDeviation1Channel8Bit<uint8_t>(1920u, 1080u, 11u, GTEST_TEST_DURATION)));
}


TEST(TestFrameVariance, TestDeviation1Channel8Bit_int8_21Window)
{
	Worker worker;
	EXPECT_TRUE((TestFrameVariance::testDeviation1Channel8Bit<int8_t>(1920u, 1080u, 21u, GTEST_TEST_DURATION)));
}

TEST(TestFrameVariance, TestDeviation1Channel8Bit_uint8_21Window)
{
	Worker worker;
	EXPECT_TRUE((TestFrameVariance::testDeviation1Channel8Bit<uint8_t>(1920u, 1080u, 21u, GTEST_TEST_DURATION)));
}

TEST(TestFrameVariance, FrameStatistics)
{
	EXPECT_TRUE(TestFrameVariance::testFrameStatistics(1920u, 1080u, GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestFrameVariance::testDeviation1Channel8Bit(const unsigned int width, const unsigned int height, const unsigned int window, const double testDuration)
{
	static_assert(std::is_same<T, int8_t>::value || std::is_same<T, uint8_t>::value, "Invalid data type!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(window >= 1u && window % 2u == 1u);

	Log::info() << "Testing 8 bit with frame size " << width << "x" << height << " and window " << window << ", using '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	const unsigned int window_2 = window / 2u;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, window_2, 1024u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, window_2, 1024u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(1u), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame deviationFrame = CV::CVUtilities::randomizedFrame(FrameType(frame.frameType(), FrameType::genericPixelFormat<uint8_t>(1u)), &randomGenerator);

			const Frame copyDevicationFrame(deviationFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(performanceIteration);
				const bool localResult = CV::FrameVariance::deviation1Channel8Bit(frame.constdata<T>(), deviationFrame.data<uint8_t>(), testWidth, testHeight, frame.paddingElements(), deviationFrame.paddingElements(), window);
			performance.stopIf(performanceIteration);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(deviationFrame, copyDevicationFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!localResult)
			{
				allSucceeded = false;
			}

			if (!validateDeviation1Channel<T, uint8_t>(frame, deviationFrame, window))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

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

bool TestFrameVariance::testFrameStatistics(const unsigned width, const unsigned int height, const double testDuration)
{
	Log::info() << "Frame statistics (mean, variance, standard deviation):";
	Log::info() << " ";

	bool succeeded = true;

	succeeded = succeeded && testFrameStatistics<int8_t, int64_t, int32_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<int8_t, int64_t, int32_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<int8_t, int64_t, int32_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<int8_t, int64_t, int32_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, uint32_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, double, 1u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, double, 2u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, double, 3u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<uint8_t, uint64_t, double, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	succeeded = succeeded && testFrameStatistics<float, double, double, 1u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<float, double, double, 2u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<float, double, double, 3u>(width, height, testDuration);
	Log::info() << " ";
	succeeded = succeeded && testFrameStatistics<float, double, double, 4u>(width, height, testDuration);

	Log::info() << " ";

	if (succeeded)
	{
		Log::info() << "Frame statistics: succeeded.";
	}
	else
	{
		Log::info() << "Frame statistics: FAILED!";
	}

	return succeeded;
}

template <typename TElementType, typename TSummationType, typename TMultiplicationType, unsigned int tChannels>
bool TestFrameVariance::testFrameStatistics(const unsigned width, const unsigned int height, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	bool succeeded = true;

	RandomGenerator randomGenerator;
	HighPerformanceStatistic performance;

	Log::info() << "Image size: " << width << "x" << height << " px";
	Log::info() << "Element, intermediate, multiplication, channels: " << TypeNamer::name<TElementType>() << ", " << TypeNamer::name<TSummationType>() << ", " << TypeNamer::name<TMultiplicationType>() << ", " << tChannels;

	double maxErrorMean = 0.0;
	double maxErrorVariance = 0.0;
	double maxErrorStandardDeviation = 0.0;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
		{
			const bool benchmark = benchmarkIteration == 0u;

			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, 1920u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<TElementType, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			double mean[tChannels];
			double variance[tChannels];
			double standardDeviation[tChannels];

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				mean[i] = NumericD::maxValue();
				variance[i] = NumericD::maxValue();
				standardDeviation[i] = NumericD::maxValue();
			}

			double* returnMean = RandomI::random(randomGenerator, 1u) == 0u ? mean : nullptr;
			double* returnVariance = RandomI::random(randomGenerator, 1u) == 0u ? variance : nullptr;
			double* returnStandardDeviation = RandomI::random(randomGenerator, 1u) == 0u ? standardDeviation : nullptr;

			performance.startIf(benchmark);
				CV::FrameVariance::imageStatistics<TElementType, TSummationType, TMultiplicationType, tChannels>(frame.constdata<TElementType>(), frame.width(), frame.height(), frame.paddingElements(), returnMean, returnVariance, standardDeviation);
			performance.stopIf(benchmark);

			double currentErrorMean = 0.0;
			double currentErrorVariance = 0.0;
			double currentErrorStandardDeviation = 0.0;

			succeeded = succeeded && validateFrameStatistics<TElementType, tChannels>(frame.constdata<TElementType>(), frame.width(), frame.height(), frame.paddingElements(), returnMean, returnVariance, returnStandardDeviation, currentErrorMean, currentErrorVariance, currentErrorStandardDeviation);

			maxErrorMean = std::max(maxErrorMean, currentErrorMean);
			maxErrorVariance = std::max(maxErrorMean, currentErrorVariance);
			maxErrorStandardDeviation = std::max(maxErrorMean, currentErrorStandardDeviation);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << String::toAString(performance.bestMseconds(), 3u) << "/" << String::toAString(performance.medianMseconds(), 3u) << "/" << String::toAString(performance.worstMseconds(), 3u) << " ms";
	Log::info() << "Max. errors (mean/variance/stddev): " << String::toAString(maxErrorMean, 3u) << "/" << String::toAString(maxErrorVariance, 3u) << "/" << String::toAString(maxErrorStandardDeviation, 3u);

	if (!succeeded)
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

template <typename T, typename TVariance>
bool TestFrameVariance::validateDeviation1Channel(const Frame& frame, const Frame& variance, const unsigned int window)
{
	ocean_assert(frame.isValid() && variance.isValid());
	ocean_assert(frame.channels() == 1u && variance.channels() == 1u);
	ocean_assert(frame.dataType() == FrameType::dataType<T>());
	ocean_assert(variance.dataType() == FrameType::dataType<TVariance>());

	ocean_assert(window >= 1u && window % 2u == 1u);

	const unsigned int border = window / 2u;

	const double normalization = 1.0 / double(window * window);

	for (unsigned int y = border; y < frame.height() - border; ++y)
	{
		for (unsigned int x = border; x < frame.width() - border; ++x)
		{
			double sqrValues = 0.0;
			double values = 0.0;

			for (unsigned int yy = y - border; yy <= y + border; ++yy)
			{
				for (unsigned int xx = x - border; xx <= x + border; ++xx)
				{
					const double value = double(frame.constpixel<T>(xx, yy)[0]);

					values += value;
					sqrValues += value * value;
				}
			}

			const double meanSqrValues = sqrValues * normalization;
			const double meanValues = values * normalization;
			const double sqrMeanValues = meanValues * meanValues;
			ocean_assert(meanSqrValues >= sqrMeanValues);

			const unsigned int varianceValue = (unsigned int)(meanSqrValues - sqrMeanValues + 0.5);
			ocean_assert(varianceValue <= 256u * 256u);
			const unsigned int deviation = Approximation::sqrt(uint16_t(varianceValue));

			double errors = 0;
			for (unsigned int yy = y - border; yy <= y + border; ++yy)
			{
				for (unsigned int xx = x - border; xx <= x + border; ++xx)
				{
					const double value = double(frame.constpixel<T>(xx, yy)[0]) - meanValues;
					errors += value * value;
				}
			}

			const uint32_t controlVariance = uint32_t(errors * normalization + 0.5);
			ocean_assert(controlVariance <= 256u * 256u);
			const uint32_t controlDeviation = Approximation::sqrt(uint16_t(controlVariance));

			const uint32_t testDeviation = uint32_t(variance.constpixel<TVariance>(x, y)[0]);

			if (abs(int32_t(deviation) - int32_t(controlDeviation)) > 2)
			{
				return false;
			}

			if (abs(int32_t(deviation) - int32_t(testDeviation)) > 2)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename TElementType, unsigned int tChannels>
bool TestFrameVariance::validateFrameStatistics(const TElementType* frame, const unsigned int width, const unsigned int height, const unsigned paddingElements, const double* testMean, const double* testVariance, const double* testStandardDeviation, double& maxErrorMean, double& maxErrorVariance, double& maxErrorStandardDeviation)
{
	static_assert(tChannels != 0u, "Number of channels must be 1 or larger");

	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	maxErrorMean = 0.0;
	maxErrorVariance = 0.0;
	maxErrorStandardDeviation = 0.0;

	double localSum[tChannels] = { 0.0 };
	double localSquareSum[tChannels] = { 0.0 };

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				localSum[c] += (double)(*frame);
				localSquareSum[c] += (double)(*frame) * (double)(*frame);

				frame++;
			}
		}

		frame += paddingElements;
	}

	double localMean[tChannels] = { 0.0 };
	double localVariance[tChannels] = { 0.0 };
	double localStandardDeviation[tChannels] = { 0.0 };

	const unsigned int pixelCount = width * height;
	ocean_assert(pixelCount != 0u);

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		localMean[c] = localSum[c] / double(pixelCount);

		// var = sum((I_i - mean)^2) / N, i = 1...N
		//     = (sum(I_i^2)) / N) - (2 * sum(I_i) * mean / N) + mean^2
		//     = (sum(I_i^2)) / N) - (2 * mean * mean)         + mean^2
		//     = (sum(I_i^2)) / N) - mean^2
		localVariance[c] = (localSquareSum[c] / double(pixelCount)) - (localMean[c] * localMean[c]);
		ocean_assert(localVariance[c] >= 0.0);

		localStandardDeviation[c] = NumericD::sqrt(localVariance[c]);

		if (testMean)
		{
			maxErrorMean = std::max(maxErrorMean, NumericD::abs(testMean[c] - localMean[c]));
		}

		if (testVariance)
		{
			maxErrorVariance = std::max(maxErrorVariance, NumericD::abs(testVariance[c] - localVariance[c]));
		}

		if (testStandardDeviation)
		{
			maxErrorStandardDeviation = std::max(maxErrorStandardDeviation, NumericD::abs(testStandardDeviation[c] - localStandardDeviation[c]));
		}
	}

	constexpr double maxAllowedError = 1e-4;

	return maxErrorMean <= maxAllowedError && maxErrorVariance <= maxAllowedError && maxErrorStandardDeviation <= maxAllowedError;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
