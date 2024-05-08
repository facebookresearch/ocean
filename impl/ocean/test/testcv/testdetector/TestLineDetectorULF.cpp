/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestLineDetectorULF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestLineDetectorULF::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   ULF Line detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRowSums(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowSqrSums(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRMSBarEdgeDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testHorizontalRMSBarEdgeDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRMSBarLineDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRMSStepEdgeDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testHorizontalRMSStepEdgeDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRMSStepLineDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSDStepEdgeDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testHorizontalSDStepEdgeDetector(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ULF Line detector test succeeded.";
	}
	else
	{
		Log::info() << "ULF Line detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestLineDetectorULF, RowSums)
{
	EXPECT_TRUE(TestLineDetectorULF::testRowSums(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, RowSqrSums)
{
	EXPECT_TRUE(TestLineDetectorULF::testRowSqrSums(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, RMSBarEdgeDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testRMSBarEdgeDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, HorizontalRMSBarEdgeDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testHorizontalRMSBarEdgeDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, RMSBarLineDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testRMSBarLineDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, RMSStepEdgeDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testRMSStepEdgeDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, HorizontalRMSStepEdgeDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testHorizontalRMSStepEdgeDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, RMSStepLineDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testRMSStepLineDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, SDStepEdgeDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testSDStepEdgeDetector(GTEST_TEST_DURATION));
}

TEST(TestLineDetectorULF, HorizontalSDStepEdgeDetector)
{
	EXPECT_TRUE(TestLineDetectorULF::testHorizontalSDStepEdgeDetector(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestLineDetectorULF::testRowSums(const double testDuration)
{
	Log::info() << "Sliding window sums for rows test (just sums):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		unsigned int outOfMemoryAccessChecker = 0u; // allocate as early as possible

		const unsigned int window = RandomI::random(randomGenerator, 1u, 5u);
		const unsigned int width = RandomI::random(randomGenerator, 20u, 1920u);

		std::vector<unsigned char> row(width);

		for (size_t n = 0; n < row.size(); ++n)
		{
			row[n] = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		ocean_assert(width >= window);
		const unsigned int sumElements = width - window + 1u;

		std::vector<unsigned int> sums(sumElements + 1u); // +1 for out of memory checker

		for (size_t n = 0; n < sums.size(); ++n)
		{
			sums[n] = RandomI::random32(randomGenerator);
		}

		outOfMemoryAccessChecker = sums.back();

		EdgeDetector::determineRowSums(row.data(), width, window, sums.data());

		for (unsigned int n = 0u; n < sumElements; ++n)
		{
			unsigned int testSum = 0u;

			for (unsigned int i = 0u; i < window; ++i)
			{
				testSum += row[n + i];
			}

			if (testSum != sums[n])
			{
				allSucceeded = false;
			}
		}

		ocean_assert(outOfMemoryAccessChecker == sums.back());
		if (outOfMemoryAccessChecker != sums.back())
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testRowSqrSums(const double testDuration)
{
	Log::info() << "Sliding window sums for rows test (sums and sums of squared):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing sum function with 32bit for normal sum and 32 bit for square sum

			unsigned int outOfMemoryAccessCheckerSums = 0u; // allocate as early as possible
			unsigned int outOfMemoryAccessCheckerSqrSums = 0u; // allocate as early as possible

			const unsigned int window = RandomI::random(randomGenerator, 1u, 5u);
			const unsigned int width = RandomI::random(randomGenerator, 20u, 1920u);

			std::vector<uint8_t> row(width);

			for (size_t n = 0; n < row.size(); ++n)
			{
				row[n] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			}

			ocean_assert(width >= window);
			const unsigned int sumElements = width - window + 1u;

			std::vector<uint32_t> sums(sumElements + 1u); // +1 for out of memory checker
			std::vector<uint32_t> sqrSums(sumElements + 1u); // +1 for out of memory checker

			for (size_t n = 0; n < sums.size(); ++n)
			{
				sums[n] = RandomI::random32(randomGenerator);
				sqrSums[n] = RandomI::random32(randomGenerator);
			}

			outOfMemoryAccessCheckerSums = sums.back();
			outOfMemoryAccessCheckerSqrSums = sqrSums.back();

			EdgeDetector::determineRowSums(row.data(), width, window, sums.data(), sqrSums.data());

			for (unsigned int n = 0u; n < sumElements; ++n)
			{
				uint64_t testSum = 0u;
				uint64_t testSqrSum = 0u;

				for (unsigned int i = 0u; i < window; ++i)
				{
					testSum += uint64_t(row[n + i]);
					testSqrSum += uint64_t(row[n + i]) * uint64_t(row[n + i]);
				}

				if (testSum != uint64_t(sums[n]))
				{
					allSucceeded = false;
				}

				if (testSqrSum != uint64_t(sqrSums[n]))
				{
					allSucceeded = false;
				}
			}

			ocean_assert(outOfMemoryAccessCheckerSums == sums.back());
			if (outOfMemoryAccessCheckerSums != sums.back())
			{
				allSucceeded = false;
			}

			ocean_assert(outOfMemoryAccessCheckerSqrSums == sqrSums.back());
			if (outOfMemoryAccessCheckerSqrSums != sqrSums.back())
			{
				allSucceeded = false;
			}
		}

		{
			// testing sum function with 16bit for normal sum and 32 bit for square sum

			unsigned int outOfMemoryAccessCheckerSums = 0u; // allocate as early as possible
			unsigned int outOfMemoryAccessCheckerSqrSums = 0u; // allocate as early as possible

			const unsigned int window = RandomI::random(randomGenerator, 1u, 5u);
			const unsigned int width = RandomI::random(randomGenerator, 20u, 1920u);

			std::vector<uint8_t> row(width);

			for (size_t n = 0; n < row.size(); ++n)
			{
				row[n] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			}

			ocean_assert(width >= window);
			const unsigned int sumElements = width - window + 1u;

			std::vector<uint16_t> sums(sumElements + 1u); // +1 for out of memory checker
			std::vector<uint32_t> sqrSums(sumElements + 1u); // +1 for out of memory checker

			for (size_t n = 0; n < sums.size(); ++n)
			{
				sums[n] = uint16_t(RandomI::random32(randomGenerator));
				sqrSums[n] = RandomI::random32(randomGenerator);
			}

			outOfMemoryAccessCheckerSums = sums.back();
			outOfMemoryAccessCheckerSqrSums = sqrSums.back();

			EdgeDetector::determineRowSums(row.data(), width, window, sums.data(), sqrSums.data());

			for (unsigned int n = 0u; n < sumElements; ++n)
			{
				uint64_t testSum = 0u;
				uint64_t testSqrSum = 0u;

				for (unsigned int i = 0u; i < window; ++i)
				{
					testSum += uint64_t(row[n + i]);
					testSqrSum += uint64_t(row[n + i]) * uint64_t(row[n + i]);
				}

				if (testSum != uint64_t(sums[n]))
				{
					allSucceeded = false;
				}

				if (testSqrSum != uint64_t(sqrSums[n]))
				{
					allSucceeded = false;
				}
			}

			ocean_assert(outOfMemoryAccessCheckerSums == sums.back());
			if (outOfMemoryAccessCheckerSums != sums.back())
			{
				allSucceeded = false;
			}

			ocean_assert(outOfMemoryAccessCheckerSqrSums == sqrSums.back());
			if (outOfMemoryAccessCheckerSqrSums != sqrSums.back())
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testRMSBarEdgeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "RMS bar edge detector test:";
	Log::info() << " ";

	bool allSucceeded = true;

	std::vector<double> maxAbsErrors;
	std::vector<double> maxAbsErrorsPercent;
	std::vector<double> averageAbsErrorsPercent;

	constexpr unsigned int windowSize = 4u;
	constexpr unsigned int minimalDelta = 5u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 1920u);

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		// we run the floating-point based implementation (almost original implementation)

		const RMSBarEdgeDetectorF rmsBarEdgeDetectorF(windowSize, minimalDelta);

		Frame responseFrameF(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(responseFrameF, false, &randomGenerator);

		ocean_assert(responseFrameF.isContinuous());
		rmsBarEdgeDetectorF.invokeVertical(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), responseFrameF.data<int16_t>(), yFrame.paddingElements());

		// we run the integer base implementation

		const RMSBarEdgeDetectorI rmsBarEdgeDetectorI(windowSize, minimalDelta);

		Frame responseFrameI(responseFrameF.frameType());
		CV::CVUtilities::randomizeFrame(responseFrameI, false, &randomGenerator);

		ocean_assert(responseFrameI.isContinuous());
		rmsBarEdgeDetectorI.invokeVertical(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), responseFrameI.data<int16_t>(), yFrame.paddingElements());

		ocean_assert(sqr(rmsBarEdgeDetectorF.adjustThreshold(100u)) == rmsBarEdgeDetectorI.adjustThreshold(100u));
		if (sqr(rmsBarEdgeDetectorF.adjustThreshold(100u)) != rmsBarEdgeDetectorI.adjustThreshold(100u))
		{
			allSucceeded = false;
		}

		double maxAbsError = 0.0;
		double maxAbsErrorPercent = 0.0;
		double sumAbsErrorPercent = 0.0;

		unsigned int measurements = 0u;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const int16_t* responseRowF = responseFrameF.constrow<int16_t>(y);
			const int16_t* responseRowI = responseFrameI.constrow<int16_t>(y);

			for (unsigned int x = 0u; x < width; ++x)
			{
				const double groundTruth = rmsBarEdgeResponse(yFrame, x, y, windowSize, double(minimalDelta)) * 16.0; // 16: is an explicit scaling factor to increase the response to a reasonable number

				if (abs(NumericD::round32(groundTruth) - int(responseRowF[x])) > 3)
				{
					allSucceeded = false;
				}

				const double responseF = minmax(-181.0, double(responseRowF[x]), 181.0); // maximal possible sqrt response: 181 = sqrt(2^15)
				const int responseI = responseRowI[x];

				const double sqrtResponseI = NumericD::sqrt(double(std::abs(responseI))) * double(NumericT<int>::sign(responseI));

				// we do not check the accuracy for smaller response values due to integer precision (and sqr responses)
				if (NumericD::abs(responseF) >= 10.0)
				{
					const double absError = NumericD::abs(responseF - sqrtResponseI);
					maxAbsError = std::max(maxAbsError, absError);

					const double absErrorPercent = absError / std::max(1.0, std::max(std::abs(responseF), std::abs(sqrtResponseI)));

					if (absErrorPercent >= 0.1)
					{
						allSucceeded = false;
					}

					maxAbsErrorPercent = std::max(maxAbsErrorPercent, absErrorPercent);

					sumAbsErrorPercent += absErrorPercent;

					measurements++;
				}
			}
		}

		const double averageAbsErrorPercent = NumericD::ratio(sumAbsErrorPercent, double(measurements));

		maxAbsErrors.push_back(maxAbsError);
		maxAbsErrorsPercent.push_back(maxAbsErrorPercent);
		averageAbsErrorsPercent.push_back(averageAbsErrorPercent);
	}
	while (startTimestamp + testDuration > Timestamp(true));

	std::sort(maxAbsErrors.begin(), maxAbsErrors.end());
	std::sort(maxAbsErrorsPercent.begin(), maxAbsErrorsPercent.end());
	std::sort(averageAbsErrorsPercent.begin(), averageAbsErrorsPercent.end());

	const double maxAbsErrorP90 = maxAbsErrors[maxAbsErrors.size() * 90 / 100];
	const double maxAbsErrorsPercentP90 = maxAbsErrorsPercent[maxAbsErrorsPercent.size() * 90 / 100];
	const double averageAbsErrorsPercentP90 = averageAbsErrorsPercent[averageAbsErrorsPercent.size() * 90 / 100];

	if (maxAbsErrorsPercentP90 >= 0.085) // 8.5%
	{
		allSucceeded = false;
	}

	if (averageAbsErrorsPercentP90 >= 0.025) // 2.5%
	{
		allSucceeded = false;
	}

	Log::info() << "Error between integer and floating point calculation:";
	Log::info() << "Maximal error p90: " << maxAbsErrorP90;
	Log::info() << "Maximal error percent p90: " << String::toAString(maxAbsErrorsPercentP90 * 100.0, 1u) << "%";
	Log::info() << "Average error percent p90: " << String::toAString(averageAbsErrorsPercentP90 * 100.0, 1u) << "%";

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testHorizontalRMSBarEdgeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Horizontal RMS bar edge detector test:";
	Log::info() << " ";

	constexpr unsigned int windowSize = 4u;

	const RMSBarEdgeDetectorI rmsBarEdgeDetectorI(windowSize);

	if (!rmsBarEdgeDetectorI.hasInvokeHorizontal(1280u, 720u))
	{
		Log::info() << "Skipping test as the horizontal edge detector is not implemented.";
		return true;
	}

	return testHorizontalEdgeDetector(rmsBarEdgeDetectorI, testDuration);
}

bool TestLineDetectorULF::testRMSBarLineDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "RMS bar line detector test:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 1920u);

		Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const bool darkBackground = RandomI::random(randomGenerator, 1u) == 0u;

		const uint8_t backgroundColor = darkBackground ? 0x00 : 0xFF;
		const uint8_t foregroundColor = darkBackground ? 0xFF : 0x00;

		yFrame.setValue(backgroundColor);

		const bool horizontal = RandomI::random(randomGenerator, 1u) == 0u;

		unsigned int position = (unsigned int)(-1);

		if (horizontal)
		{
			position = RandomI::random(randomGenerator, 10u, height - 11u);

			memset(yFrame.row<void>(position), foregroundColor, width * sizeof(uint8_t));
		}
		else
		{
			position = RandomI::random(randomGenerator, 10u, width - 11u);

			for (unsigned int y = 0u; y < height; ++y)
			{
				yFrame.pixel<uint8_t>(position, y)[0] = foregroundColor;
			}
		}

		constexpr unsigned int threshold = 50u;
		constexpr unsigned int minimalLength = 20u;
		constexpr float maximalStraightLineDistance = 1.6f;

		EdgeTypes types;
		const FiniteLines2 lines = LineDetectorULF::detectLines(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), LineDetectorULF::RMSBarEdgeDetectorI::asEdgeDetectors(), threshold, minimalLength, maximalStraightLineDistance, &types);

		if (types.size() != lines.size())
		{
			allSucceeded = false;
		}

		if (lines.size() != 1 || types.size() != 1)
		{
			allSucceeded = false;
		}
		else
		{
			const FiniteLine2& line = lines.front();

			constexpr Scalar maximalError = Scalar(0.5);

			if (horizontal)
			{
				if (Numeric::isNotEqual(line.point0().y(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.point1().y(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.length(), Scalar(width - 1u), maximalError))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (Numeric::isNotEqual(line.point0().x(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.point1().x(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.length(), Scalar(height - 1u), maximalError))
				{
					allSucceeded = false;
				}
			}

			if (darkBackground)
			{
				if (types.front() != (ET_SIGN_POSITIVE | ET_BAR))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (types.front() != (ET_SIGN_NEGATIVE | ET_BAR))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testRMSStepEdgeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "RMS step edge detector test:";
	Log::info() << " ";

	bool allSucceeded = true;

	std::vector<double> maxAbsErrors;
	std::vector<double> maxAbsErrorsPercent;
	std::vector<double> averageAbsErrorsPercent;

	constexpr unsigned int windowSize = 4u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 1920u);

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		// we run the floating-point based implementation (almost original implementation)

		const RMSStepEdgeDetectorF rmsStepEdgeDetectorF(windowSize);

		Frame responseFrameF(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(responseFrameF, false, &randomGenerator);

		ocean_assert(responseFrameF.isContinuous());
		rmsStepEdgeDetectorF.invokeVertical(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), responseFrameF.data<int16_t>(), yFrame.paddingElements());

		// we run the integer base implementation

		const RMSStepEdgeDetectorI rmsStepEdgeDetectorI(windowSize);

		Frame responseFrameI(responseFrameF.frameType());
		CV::CVUtilities::randomizeFrame(responseFrameI, false, &randomGenerator);

		ocean_assert(responseFrameI.isContinuous());
		rmsStepEdgeDetectorI.invokeVertical(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), responseFrameI.data<int16_t>(), yFrame.paddingElements());

		ocean_assert(sqr(rmsStepEdgeDetectorF.adjustThreshold(100u)) == rmsStepEdgeDetectorI.adjustThreshold(100u));
		if (sqr(rmsStepEdgeDetectorF.adjustThreshold(100u)) != rmsStepEdgeDetectorI.adjustThreshold(100u))
		{
			allSucceeded = false;
		}

		double maxAbsError = 0.0;
		double maxAbsErrorPercent = 0.0;
		double sumAbsErrorPercent = 0.0;

		unsigned int measurements = 0u;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const int16_t* responseRowF = responseFrameF.constrow<int16_t>(y);
			const int16_t* responseRowI = responseFrameI.constrow<int16_t>(y);

			for (unsigned int x = 0u; x < width; ++x)
			{
				const double groundTruthSeparateResidual = rmsStepEdgeResponse<true, false>(yFrame, x, y, windowSize);

				if (abs(NumericD::round32(groundTruthSeparateResidual) - int(responseRowF[x])) > 1)
				{
					allSucceeded = false;
				}

				double decisionDelta = 0.0;
				const double groundTruthCommonResidualSqr = rmsStepEdgeResponse<false, true>(yFrame, x, y, windowSize, &decisionDelta);
				const double groundTruthCommonResidual = NumericD::copySign(NumericD::sqrt(NumericD::abs(groundTruthCommonResidualSqr)), groundTruthCommonResidualSqr);

				const double responseF = minmax(-181.0, double(responseRowF[x]), 181.0); // maximal possible sqrt response: 181 = sqrt(2^15)
				const int responseI = responseRowI[x];

				const double sqrtResponseI = NumericD::sqrt(double(std::abs(responseI))) * double(NumericT<int>::sign(responseI));

				// we do not check the accuracy for smaller response values due to integer precision (and sqr responses)
				if (NumericD::abs(responseF) >= 10.0 && decisionDelta > 1.0)
				{
					const double absError = NumericD::abs(groundTruthCommonResidual - sqrtResponseI);
					maxAbsError = std::max(maxAbsError, absError);

					const double absErrorPercent = absError / std::max(1.0, std::max(std::abs(responseF), std::abs(sqrtResponseI)));

					if (absErrorPercent >= 0.1)
					{
						allSucceeded = false;
					}

					maxAbsErrorPercent = std::max(maxAbsErrorPercent, absErrorPercent);

					sumAbsErrorPercent += absErrorPercent;

					measurements++;
				}
			}
		}

		const double averageAbsErrorPercent = NumericD::ratio(sumAbsErrorPercent, double(measurements));

		maxAbsErrors.push_back(maxAbsError);
		maxAbsErrorsPercent.push_back(maxAbsErrorPercent);
		averageAbsErrorsPercent.push_back(averageAbsErrorPercent);
	}
	while (startTimestamp + testDuration > Timestamp(true));

	std::sort(maxAbsErrors.begin(), maxAbsErrors.end());
	std::sort(maxAbsErrorsPercent.begin(), maxAbsErrorsPercent.end());
	std::sort(averageAbsErrorsPercent.begin(), averageAbsErrorsPercent.end());

	const double maxAbsErrorP90 = maxAbsErrors[maxAbsErrors.size() * 90 / 100];
	const double maxAbsErrorsPercentP90 = maxAbsErrorsPercent[maxAbsErrorsPercent.size() * 90 / 100];
	const double averageAbsErrorsPercentP90 = averageAbsErrorsPercent[averageAbsErrorsPercent.size() * 90 / 100];

	if (maxAbsErrorsPercentP90 >= 0.085) // 8.5%
	{
		allSucceeded = false;
	}

	if (averageAbsErrorsPercentP90 >= 0.025) // 2.5%
	{
		allSucceeded = false;
	}

	Log::info() << "Error between integer and floating point calculation:";
	Log::info() << "Maximal error p90: " << maxAbsErrorP90;
	Log::info() << "Maximal error percent p90: " << String::toAString(maxAbsErrorsPercentP90 * 100.0, 1u) << "%";
	Log::info() << "Average error percent p90: " << String::toAString(averageAbsErrorsPercentP90 * 100.0, 1u) << "%";

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testHorizontalRMSStepEdgeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Horizontal RMS steps edge detector test:";
	Log::info() << " ";

	constexpr unsigned int windowSize = 4u;

	const RMSStepEdgeDetectorI rmsStepEdgeDetectorI(windowSize);

	if (!rmsStepEdgeDetectorI.hasInvokeHorizontal(1280u, 720u))
	{
		Log::info() << "Skipping test as the horizontal edge detector is not implemented.";
		return true;
	}

	return testHorizontalEdgeDetector(rmsStepEdgeDetectorI, testDuration);
}

bool TestLineDetectorULF::testRMSStepLineDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "RMS step line detector test:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 1920u);

		Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const bool darkStart = RandomI::random(randomGenerator, 1u) == 0u;

		const uint8_t startColor = darkStart ? 0x00 : 0xFF;
		const uint8_t endColor = darkStart ? 0xFF : 0x00;

		yFrame.setValue(startColor);

		const bool horizontal = RandomI::random(randomGenerator, 1u) == 0u;

		unsigned int position = (unsigned int)(-1);

		if (horizontal)
		{
			position = RandomI::random(randomGenerator, 10u, height - 11u);

			for (unsigned int y = position; y < height; ++y)
			{
				memset(yFrame.row<uint8_t>(y), endColor, width * sizeof(uint8_t));
			}
		}
		else
		{
			position = RandomI::random(randomGenerator, 10u, width - 11u);

			for (unsigned int y = 0u; y < height; ++y)
			{
				memset(yFrame.row<uint8_t>(y) + position, endColor, (width - position) * sizeof(uint8_t));
			}
		}

		constexpr unsigned int threshold = 50u;
		constexpr unsigned int minimalLength = 20u;
		constexpr float maximalStraightLineDistance = 1.6f;

		EdgeTypes types;
		const FiniteLines2 lines = LineDetectorULF::detectLines(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), LineDetectorULF::RMSStepEdgeDetectorI::asEdgeDetectors(), threshold, minimalLength, maximalStraightLineDistance, &types);

		if (types.size() != lines.size())
		{
			allSucceeded = false;
		}

		if (lines.size() != 1 || types.size() != 1)
		{
			allSucceeded = false;
		}
		else
		{
			const FiniteLine2& line = lines.front();

			constexpr Scalar maximalError = Scalar(1.1);

			if (horizontal)
			{
				if (Numeric::isNotEqual(line.point0().y(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.point1().y(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.length(), Scalar(width - 1u), maximalError))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (Numeric::isNotEqual(line.point0().x(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.point1().x(), Scalar(position), maximalError))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(line.length(), Scalar(height - 1u), maximalError))
				{
					allSucceeded = false;
				}
			}

			if (darkStart)
			{
				if (types.front() != (ET_SIGN_NEGATIVE | ET_STEP))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (types.front() != (ET_SIGN_POSITIVE | ET_STEP))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testSDStepEdgeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SD step edge detector test:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int windowSize = RandomI::random(randomGenerator, 1u, 5u);
		const unsigned int stepSize = RandomI::random(randomGenerator, 0u, 1u);

		const unsigned int width = RandomI::random(randomGenerator, 50u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 1920u);

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame responseFrameI(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(responseFrameI, false, &randomGenerator);

		const SDStepEdgeDetectorI sdStepEdgeDetectorI(windowSize, stepSize);

		ocean_assert(responseFrameI.isContinuous());
		sdStepEdgeDetectorI.invokeVertical(yFrame.constdata<uint8_t>(), width, height, responseFrameI.data<int16_t>(), yFrame.paddingElements());

		for (unsigned int y = 0u; y < height; ++y)
		{
			const int16_t* responseRow = responseFrameI.constrow<int16_t>(y);

			for (unsigned int x = 0u; x < width; ++x)
			{
				const double response = double(responseRow[x]);
				const double groundTruthResponse = sdStepEdgeResponse(yFrame, x, y, stepSize, windowSize);

				if (groundTruthResponse != response)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLineDetectorULF::testHorizontalSDStepEdgeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Horizontal SD steps edge detector test:";
	Log::info() << " ";

	const unsigned int windowSize = RandomI::random(1u, 5u);

	const SDStepEdgeDetectorI sdStepEdgeDetectorI(windowSize);

	if (!sdStepEdgeDetectorI.hasInvokeHorizontal(1280u, 720u))
	{
		Log::info() << "Skipping test as the horizontal edge detector is not implemented.";
		return true;
	}

	return testHorizontalEdgeDetector(sdStepEdgeDetectorI, testDuration);
}

bool TestLineDetectorULF::testHorizontalEdgeDetector(const EdgeDetector& edgeDetector, const double testDuration)
{
	ocean_assert(edgeDetector.hasInvokeHorizontal(50, 50));
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	HighPerformanceStatistic performanceVertical;
	HighPerformanceStatistic performanceVerticalTranspose;
	HighPerformanceStatistic performanceHorizontal;

	RandomGenerator randomGenerator;

	unsigned int iteration = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		const bool performanceIteration = iteration % 2u == 0u;

		const unsigned int width = performanceIteration ? 1280u : RandomI::random(randomGenerator, 50u, 1920u);
		const unsigned int height = performanceIteration ? 720u : RandomI::random(randomGenerator, 50u, 1920u);

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		ocean_assert(yFrame.isValid());
		ocean_assert(edgeDetector.hasInvokeHorizontal(yFrame.width(), yFrame.height()));

		const unsigned int transposedPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame yTransposedFrame(FrameType(yFrame, yFrame.height(), yFrame.width()), transposedPaddingElements);
		Frame transposedResposeFrame(FrameType(yTransposedFrame, FrameType::genericPixelFormat<int16_t, 1u>()));
		Frame responseFrame(FrameType(transposedResposeFrame, transposedResposeFrame.height(), transposedResposeFrame.width()));
		Frame horizontalResponseFrame(FrameType(yFrame, FrameType::genericPixelFormat<int16_t, 1u>()));

		if (performanceIteration)
		{
			performanceVertical.start();
			performanceVerticalTranspose.start();
		}

		if (!CV::FrameTransposer::transpose(yFrame, yTransposedFrame))
		{
			ocean_assert(false && "This should never happen!");
			allSucceeded = false;
		}

		if (performanceIteration)
		{
			performanceVerticalTranspose.stop();
		}

		ocean_assert(transposedResposeFrame.isContinuous());
		edgeDetector.invokeVertical(yTransposedFrame.constdata<uint8_t>(), yTransposedFrame.width(), yTransposedFrame.height(), transposedResposeFrame.data<int16_t>(), yTransposedFrame.paddingElements());

		if (performanceIteration)
		{
			performanceVertical.stop();

			performanceHorizontal.start();
		}

		ocean_assert(horizontalResponseFrame.isContinuous());
		if (!edgeDetector.invokeHorizontal(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), horizontalResponseFrame.data<int16_t>(), yFrame.paddingElements()))
		{
			allSucceeded = false;
		}

		if (performanceIteration)
		{
			performanceHorizontal.stop();
		}

		ocean_assert(responseFrame.isContinuous());
		if (!CV::FrameTransposer::transpose(transposedResposeFrame, responseFrame))
		{
			ocean_assert(false && "This should never happen!");
			allSucceeded = false;
		}

		ocean_assert(responseFrame.width() == yFrame.width() && responseFrame.height() == yFrame.height());

		for (unsigned int n = 0u; n < responseFrame.pixels(); ++n)
		{
			if (abs(int32_t(responseFrame.constdata<int16_t>()[n]) - int32_t(horizontalResponseFrame.constdata<int16_t>()[n])) > 1)
			{
				allSucceeded = false;
			}
		}

		++iteration;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance vertical: " << performanceVertical.medianMseconds() << "ms, including " << performanceVerticalTranspose.medianMseconds() << "ms for transpose";
	Log::info() << "Performance horizontal: " << performanceHorizontal.medianMseconds() << "ms";

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

double TestLineDetectorULF::rmsBarEdgeResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int windowSize, const double minimalDelta)
{
	constexpr unsigned int barSize = 3u;
	constexpr unsigned int barSize_2 = barSize / 2u;

	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(windowSize >= 1u);
	ocean_assert(yFrame.width() >= windowSize * 2u + barSize);

	ocean_assert(x < yFrame.width() && y < yFrame.height());

	if (x < windowSize + barSize_2 || x >= yFrame.width() - (windowSize + barSize_2))
	{
		return 0.0;
	}

	const uint8_t* pixel = yFrame.constpixel<uint8_t>(x, y);

	// first, we check whether we have a (positive or negative) peak value (left <= center > right, or left >= center < right)

	const int pixelLeft = int(*(pixel - 1));
	const int pixelCenter = int(*pixel);
	const int pixelRight = int(*(pixel + 1));

	// rms = max(1, sqrt(1/n * sum[(mean - yi)^2]));

	double leftMean = 0.0;
	for (unsigned int n = 0u; n < windowSize; ++n)
	{
		leftMean += double(*(pixel - int(barSize_2 + 1u + n)));
	}
	leftMean /= double(windowSize);

	double rightMean = 0.0;
	for (unsigned int n = 0u; n < windowSize; ++n)
	{
		rightMean += double(*(pixel + barSize_2 + 1u + n));
	}
	rightMean /= double(windowSize);

	if (!((double(pixelCenter) < leftMean && double(pixelCenter) < rightMean) || (double(pixelCenter) > leftMean && double(pixelCenter) > rightMean)))
	{
		return 0.0;
	}

	const double mean = (leftMean + rightMean) * 0.5;

	double sum = 0.0;
	for (unsigned int n = 0u; n < windowSize; ++n)
	{
		sum += NumericD::sqr(mean - double(*(pixel + barSize_2 + 1u + n)));
		sum += NumericD::sqr(mean - double(*(pixel - int(barSize_2 + 1u + n))));
	}

	ocean_assert(sum >= 0.0);

	double rms = NumericD::sqrt(sum / double(windowSize * 2));
	rms = std::max(1.0, rms); // we ensure a minimal rms

	// response = sign * [abs(peakDelta + secondPeakDelta) / 2] / rms

	const double deltaCenter = double(pixelCenter) - mean;
	const double deltaLeft = double(pixelLeft) - mean;
	const double deltaRight = double(pixelRight) - mean;

	// sign == +1: we have a bright edge (dark window)
	// sign == -1: we have dark edge (bright window)
	const double sign = deltaCenter >= 0 ? 1.0 : -1.0;

	if (sign == 1.0)
	{
		// we have a bright edge

		// non maximum suppression
		if ((deltaLeft <= deltaCenter && deltaCenter > deltaRight) == false)
		{
			return 0.0;
		}
	}
	else
	{
		// we have a dark edge

		// non maximum suppression
		if ((deltaLeft >= deltaCenter && deltaCenter < deltaRight) == false)
		{
			return 0.0;
		}
	}

	const double secondPeakDelta = abs(int(pixelCenter - pixelRight)) > abs(int(pixelCenter - pixelLeft)) ? deltaLeft : deltaRight;

	const double peak = NumericD::abs(deltaCenter + secondPeakDelta) * 0.5;

	if (peak < minimalDelta)
	{
		return 0.0;
	}

	ocean_assert(rms > NumericD::eps());
	const double response = sign * peak / rms;

	return response;
}

template <bool tSeparateResidual, bool tSignedSquaredResponse>
double TestLineDetectorULF::rmsStepEdgeResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int windowSize, double* decisionDelta)
{
	if (x <= windowSize || x >= yFrame.width() - windowSize - 1u)
	{
		return 0.0;
	}

	double responseLeft = rmsStepEdgeResponseWithoutNonMaximumSuppression<tSeparateResidual>(yFrame, x - 1u, y, windowSize);
	double responseCenter = rmsStepEdgeResponseWithoutNonMaximumSuppression<tSeparateResidual>(yFrame, x + 0u, y, windowSize);
	double responseRight = rmsStepEdgeResponseWithoutNonMaximumSuppression<tSeparateResidual>(yFrame, x + 1u, y, windowSize);

	if constexpr (tSignedSquaredResponse)
	{
		responseLeft = NumericD::copySign(NumericD::sqr(responseLeft), responseLeft);
		responseCenter = NumericD::copySign(NumericD::sqr(responseCenter), responseCenter);
		responseRight = NumericD::copySign(NumericD::sqr(responseRight), responseRight);
	}

	if (decisionDelta)
	{
		*decisionDelta = std::min(NumericD::abs(responseLeft - responseCenter), NumericD::abs(responseRight - responseCenter));
	}

	if (responseCenter > 0.0)
	{
		if (int(responseLeft) < int(responseCenter) && int(responseCenter) >= int(responseRight)) // we need to apply an integer-based comparison as optimized ULF applies integer precision
		{
			return responseCenter;
		}
	}
	else if (responseCenter < 0.0)
	{
		if (int(responseLeft) > int(responseCenter) && int(responseCenter) <= int(responseRight))
		{
			return responseCenter;
		}
	}

	return 0.0;
}

template <bool tSeparateResidual>
double TestLineDetectorULF::rmsStepEdgeResponseWithoutNonMaximumSuppression(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int windowSize)
{
	constexpr unsigned int stepSize = 1u;
	constexpr unsigned int stepSize_2 = stepSize / 2u;

	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(windowSize >= 1u);
	ocean_assert(yFrame.width() >= windowSize * 2u + stepSize);

	ocean_assert(x < yFrame.width() && y < yFrame.height());

	if (x < windowSize + stepSize_2 || x >= yFrame.width() - (windowSize + stepSize_2))
	{
		return 0.0;
	}

	const uint8_t* pixel = yFrame.constpixel<uint8_t>(x, y);

	// rms = max(1, sqrt(1/n * sum[(mean - yi)^2]));

	double meanRight = 0.0;
	double meanLeft = 0.0;
	for (unsigned int n = 0u; n < windowSize; ++n)
	{
		meanRight += double(*(pixel + stepSize_2 + 1u + n));
		meanLeft += double(*(pixel - int(stepSize_2 + 1u + n)));
	}
	meanRight /= double(windowSize);
	meanLeft /= double(windowSize);

	double sumRight = 0.0;
	double sumLeft = 0.0;
	for (unsigned int n = 0u; n < windowSize; ++n)
	{
		sumRight += NumericD::sqr(meanRight - double(*(pixel + stepSize_2 + 1u + n)));
		sumLeft += NumericD::sqr(meanLeft - double(*(pixel - int(stepSize_2 + 1u + n))));
	}

	ocean_assert(sumRight >= 0.0);
	ocean_assert(sumLeft >= 0.0);

	double rms = 0.0;

	if constexpr (tSeparateResidual)
	{
		const double rmsRight = NumericD::sqrt(sumRight / double(windowSize));
		const double rmsLeft = NumericD::sqrt(sumLeft / double(windowSize));

		rms = (rmsRight + rmsLeft) * 0.5;
	}
	else
	{
		const double rmsRightSqr = sumRight / double(windowSize);
		const double rmsLeftSqr = sumLeft / double(windowSize);

		rms = NumericD::sqrt((rmsRightSqr + rmsLeftSqr) * 0.5);
	}

	rms = std::max(1.0, rms); // we ensure a minimal rms

	// response = (meanL - meanR) / rms

	ocean_assert(NumericD::isNotEqualEps(rms));
	const double response = ((meanLeft - meanRight) * 4.0) / rms; // the factor 4 is an explicit scaling factor to increase the response to a reasonable inteber number

	return response;
}

double TestLineDetectorULF::sdStepEdgeResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int stepSize, const unsigned int windowSize)
{
	const unsigned int stepSize_2 = stepSize / 2u; // 0 in case 'stepSize == 1'

	if (x < windowSize + stepSize_2 || x >= yFrame.width() - windowSize - stepSize_2)
	{
		return 0.0;
	}

	const double responseLeft = sdStepEdgeResponseWithoutNonMaximumSuppression(yFrame, x - 1u, y, stepSize, windowSize);
	const double responseCenter = sdStepEdgeResponseWithoutNonMaximumSuppression(yFrame, x + 0u, y, stepSize, windowSize);
	const double responseRight = sdStepEdgeResponseWithoutNonMaximumSuppression(yFrame, x + 1u, y, stepSize, windowSize);

	if (responseCenter > 0.0)
	{
		if (int(responseLeft) < int(responseCenter) && int(responseCenter) >= int(responseRight))
		{
			return responseCenter;
		}
	}
	else if (responseCenter < 0.0)
	{
		if (int(responseLeft) > int(responseCenter) && int(responseCenter) <= int(responseRight))
		{
			return responseCenter;
		}
	}

	return 0.0;
}

double TestLineDetectorULF::sdStepEdgeResponseWithoutNonMaximumSuppression(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int stepSize, const unsigned int windowSize)
{
	const unsigned int stepSize_2 = stepSize / 2u; // 0 in case 'stepSize == 1'

	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(yFrame.width() >= windowSize * 2u + stepSize);

	ocean_assert(x < yFrame.width() && y < yFrame.height());

	ocean_assert(windowSize >= 1u);

	if (x < windowSize + stepSize_2 || x >= yFrame.width() - (windowSize + stepSize_2))
	{
		return 0.0;
	}

	const uint8_t* center = yFrame.constpixel<uint8_t>(x, y);
	const uint8_t* left = center - windowSize - stepSize_2;
	const uint8_t* right = left + windowSize + stepSize;

	double leftSum = 0.0;
	double rightSum = 0.0;

	for (unsigned int n = 0u; n < windowSize; ++n)
	{
		leftSum += *(left + n);
		rightSum += *(right + n);
	}

	const double response = leftSum - rightSum;

	return response;
}

}

}

}

}
