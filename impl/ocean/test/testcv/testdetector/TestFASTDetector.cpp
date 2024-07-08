/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestFASTDetector.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"

#include <algorithm>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestFASTDetector::test(const Frame& frame, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   FAST detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Frame yFrame;

	if (frame.isValid())
	{
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This must never happen!");
			Log::info() << "FAST detector test FAILED!";

			return false;
		}
	}
	else
	{
		yFrame = CV::CVUtilities::randomizedFrame(FrameType(1280u, 720u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	}

	allSucceeded = testStandardStrength(yFrame, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPreciseStrength(yFrame, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FAST detector test succeeded.";
	}
	else
	{
		Log::info() << "FAST detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFASTDetector, StandardStrength)
{
	const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(1280u, 720u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	Worker worker;
	EXPECT_TRUE(TestFASTDetector::testStandardStrength(yFrame, GTEST_TEST_DURATION, worker));
}

TEST(TestFASTDetector, PreciseStrength)
{
	const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(1280u, 720u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	Worker worker;
	EXPECT_TRUE(TestFASTDetector::testPreciseStrength(yFrame, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFASTDetector::testStandardStrength(const Frame& yFrame, const double testDuration, Worker& worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.width() >= 7 && yFrame.height() >= 7u);
	ocean_assert(testDuration > 0.0);

	if (!yFrame.isValid() || yFrame.width() < 7u || yFrame.height() < 7u)
	{
		return false;
	}

	Log::info() << "Testing FAST detector with standard strength:";

	bool allSucceeded = true;

	for (const unsigned int threshold : {20u, 30u, 90u, 150u})
	{
		Log::info() << " ";
		Log::info() << "... with threshold " << threshold << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		size_t foundFeatures = size_t(-1);

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (bool randomIteration : {false, true})
				{
					Frame testFrame(yFrame, Frame::ACM_USE_KEEP_LAYOUT);

					if (randomIteration)
					{
						const unsigned int testWidth = RandomI::random(9u, 1280u);
						const unsigned int testHeight = RandomI::random(9u, 720u);

						testFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
					}

					CV::Detector::FASTFeatures features;

					constexpr bool preciseScoring = false;

					performance.startIf(!randomIteration);
						const bool localResult = CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(testFrame, threshold, false, preciseScoring, features, useWorker);
					performance.stopIf(!randomIteration);

					if (!localResult)
					{
						allSucceeded = false;
					}

					if (foundFeatures == size_t(-1))
					{
						ocean_assert(!randomIteration);
						foundFeatures = size_t(features.size());
					}

					std::sort(features.begin(), features.end(), sortPoints);

					if (!validate(testFrame, threshold, features))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Found " << foundFeatures << " FAST features";
		Log::info() << "Single-core performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << ", median: " << performanceSinglecore.medianMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1) << "x";
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

bool TestFASTDetector::testPreciseStrength(const Frame& yFrame, const double testDuration, Worker& worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.width() >= 7 && yFrame.height() >= 7u);
	ocean_assert(testDuration > 0.0);

	if (!yFrame.isValid() || yFrame.width() < 7u || yFrame.height() < 7u)
	{
		return false;
	}

	Log::info() << "Testing FAST detector with precise strength:";

	bool allSucceeded = true;

	for (const unsigned int threshold : {20u, 30u, 90u, 150u})
	{
		Log::info() << " ";
		Log::info() << "... with threshold " << threshold << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		size_t foundFeatures = size_t(-1);

		CV::Detector::FASTFeatures singlecoreFeatures;
		CV::Detector::FASTFeatures multicoreFeatures;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (bool randomIteration : {false, true})
				{
					Frame testFrame(yFrame, Frame::ACM_USE_KEEP_LAYOUT);

					if (randomIteration)
					{
						const unsigned int testWidth = RandomI::random(9u, 1280u);
						const unsigned int testHeight = RandomI::random(9u, 720u);

						testFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
					}

					CV::Detector::FASTFeatures features;

					constexpr bool preciseScoring = true;

					performance.startIf(!randomIteration);
						const bool localResult = CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(testFrame, threshold, false, preciseScoring, features, useWorker);
					performance.stopIf(!randomIteration);

					if (!localResult)
					{
						allSucceeded = false;
					}

					if (foundFeatures == size_t(-1))
					{
						ocean_assert(!randomIteration);
						foundFeatures = size_t(features.size());
					}

					if (!randomIteration)
					{
						if (singlecoreFeatures.empty() && !useWorker)
						{
							singlecoreFeatures = std::move(features);
						}

						if (multicoreFeatures.empty() && useWorker)
						{
							multicoreFeatures = std::move(features);
						}
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		if (singlecoreFeatures.size() != multicoreFeatures.size())
		{
			allSucceeded = false;
		}
		else
		{
			std::sort(singlecoreFeatures.begin(), singlecoreFeatures.end(), sortPoints);
			std::sort(multicoreFeatures.begin(), multicoreFeatures.end(), sortPoints);

			for (unsigned int n = 0u; n < multicoreFeatures.size(); ++n)
			{
				if (singlecoreFeatures[n] != multicoreFeatures[n] || singlecoreFeatures[n].strength() != multicoreFeatures[n].strength())
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << "Found " << foundFeatures << " FAST features";
		Log::info() << "Single-core performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << ", median: " << performanceSinglecore.medianMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1) << "x";
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

bool TestFASTDetector::validate(const Frame& yFrame, const unsigned int threshold, const CV::Detector::FASTFeatures& features)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.width() >= 7 && yFrame.height() >= 7u);

	if (!yFrame.isValid() || yFrame.width() < 7u || yFrame.height() < 7u)
	{
		return false;
	}

	CV::Detector::FASTFeatures validationFeatures;

	Frame strengthFrame(FrameType(yFrame, FrameType::FORMAT_Y32));

	const unsigned int correctionValue = 16u * 255u * threshold;

	for (unsigned int y = 3; y < yFrame.height() - 3u; ++y)
	{
		for (unsigned int x = 3; x < yFrame.width() - 3u; ++x)
		{
			const uint8_t centerValue = yFrame.constpixel<uint8_t>(x, y)[0];
			const unsigned int centerHigh = centerValue + threshold;
			const int centerLow = int(centerValue) - int(threshold);

			unsigned int circleValues[16];

			circleValues[ 0] = yFrame.constpixel<uint8_t>(x + 0, y - 3)[0];
			circleValues[ 1] = yFrame.constpixel<uint8_t>(x + 1, y - 3)[0];
			circleValues[ 2] = yFrame.constpixel<uint8_t>(x + 2, y - 2)[0];
			circleValues[ 3] = yFrame.constpixel<uint8_t>(x + 3, y - 1)[0];
			circleValues[ 4] = yFrame.constpixel<uint8_t>(x + 3, y - 0)[0];
			circleValues[ 5] = yFrame.constpixel<uint8_t>(x + 3, y + 1)[0];
			circleValues[ 6] = yFrame.constpixel<uint8_t>(x + 2, y + 2)[0];
			circleValues[ 7] = yFrame.constpixel<uint8_t>(x + 1, y + 3)[0];
			circleValues[ 8] = yFrame.constpixel<uint8_t>(x + 0, y + 3)[0];
			circleValues[ 9] = yFrame.constpixel<uint8_t>(x - 1, y + 3)[0];
			circleValues[10] = yFrame.constpixel<uint8_t>(x - 2, y + 2)[0];
			circleValues[11] = yFrame.constpixel<uint8_t>(x - 3, y + 1)[0];
			circleValues[12] = yFrame.constpixel<uint8_t>(x - 3, y + 0)[0];
			circleValues[13] = yFrame.constpixel<uint8_t>(x - 3, y - 1)[0];
			circleValues[14] = yFrame.constpixel<uint8_t>(x - 2, y - 2)[0];
			circleValues[15] = yFrame.constpixel<uint8_t>(x - 1, y - 3)[0];

			bool enough = false;

			int strength = 0;

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				unsigned int counter = 0u;

				for (unsigned int c = i; c < i + 12u; ++c)
				{
					if (circleValues[c % 16u] > centerHigh)
					{
						++counter;
					}
				}

				if (counter >= 12u)
				{
					enough = true;

					for (unsigned int c = 0u; c < 16u; ++c)
					{
						strength += int(circleValues[c] - centerValue);
					}

					break;
				}

			}

			if (!enough)
			{
				for (unsigned int i = 0u; i < 16u; ++i)
				{
					unsigned int counter = 0u;

					for (unsigned int c = i; c < i + 12u; ++c)
					{
						if (int(circleValues[c % 16u]) < centerLow)
						{
							++counter;
						}
					}

					if (counter >= 12u)
					{
						enough = true;

						for (unsigned int c = 0u; c < 16u; ++c)
						{
							strength += int(centerValue - circleValues[c]);
						}

						break;
					}
				}
			}

			ocean_assert(correctionValue + strength >= 0);
			strengthFrame.pixel<uint32_t>(x, y)[0] = correctionValue + strength;
		}
	}

	for (unsigned int y = 4u; y < yFrame.height() - 4u; ++y)
	{
		for (unsigned int x = 4u; x < yFrame.width() - 4u; ++x)
		{
			uint32_t reference = strengthFrame.constpixel<uint32_t>(x, y)[0];

			if (reference > strengthFrame.constpixel<uint32_t>(x - 1, y - 1)[0] && reference > strengthFrame.constpixel<uint32_t>(x, y - 1)[0] && reference > strengthFrame.constpixel<uint32_t>(x + 1, y - 1)[0]
						&& reference > strengthFrame.constpixel<uint32_t>(x - 1, y)[0] && reference > strengthFrame.constpixel<uint32_t>(x + 1, y)[0]
						&& reference > strengthFrame.constpixel<uint32_t>(x - 1, y + 1)[0] && reference > strengthFrame.constpixel<uint32_t>(x, y + 1)[0] && reference > strengthFrame.constpixel<uint32_t>(x + 1, y + 1)[0])
			{
				validationFeatures.push_back(CV::Detector::FASTFeature(Vector2(Scalar(x), Scalar(y)), CV::Detector::FASTFeature::DS_INVALID, Scalar(reference)));
			}
		}
	}

	if (validationFeatures.size() != features.size())
	{
		return false;
	}

	sort(validationFeatures.begin(), validationFeatures.end(), sortPoints);

	for (size_t n = 0; n < features.size(); ++n)
	{
		if (features[n] != validationFeatures[n])
		{
			return false;
		}
	}

	return true;
}

bool TestFASTDetector::sortPoints(const CV::Detector::FASTFeature& first, const CV::Detector::FASTFeature& second)
{
	if (first.observation().y() < second.observation().y())
	{
		return true;
	}

	if (first.observation().y() == second.observation().y())
	{
		return first.observation().x() < second.observation().x();
	}

	return false;
}

}

}

}

}
