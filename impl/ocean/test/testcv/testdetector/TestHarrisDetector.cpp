/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestHarrisDetector.h"
#include "ocean/test/testcv/testdetector/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameFilterSobel.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include <algorithm>
#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestHarrisDetector::test(const Frame& frame, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Harris corner detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Frame yFrame;
	if (frame.isValid() && !CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		ocean_assert(false && "This must never happen!");
		Log::info() << "Harris corner detector test FAILED!";

		return false;
	}

	allSucceeded = testThreshold(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPixelAccuracy(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPixelAccuracyCorners(1280u, 720u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSubPixelAccuracy(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSubFrameDetection(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCheckerboardDetection(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHarrisVotePixel(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHarrisVoteFrame(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Harris corner detector test succeeded.";
	}
	else
	{
		Log::info() << "Harris corner detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHarrisDetector, Threshold)
{
	EXPECT_TRUE(TestHarrisDetector::testThreshold(GTEST_TEST_DURATION));
}

TEST(TestHarrisDetector, PixelAccuracy)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testPixelAccuracy(GTEST_TEST_DURATION, worker));
}

TEST(TestHarrisDetector, PixelAccuracyCorners)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testPixelAccuracyCorners(1280u, 720u, GTEST_TEST_DURATION, worker));
}

TEST(TestHarrisDetector, SubPixelAccuracy)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testSubPixelAccuracy(GTEST_TEST_DURATION, worker));
}

TEST(TestHarrisDetector, SubframeDetection)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testSubFrameDetection(GTEST_TEST_DURATION, worker));
}

TEST(TestHarrisDetector, CheckerboardDetection)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testCheckerboardDetection(GTEST_TEST_DURATION, worker));
}

TEST(TestHarrisDetector, HarrisVotePixel)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testHarrisVotePixel(GTEST_TEST_DURATION, worker));
}

TEST(TestHarrisDetector, HarrisVoteFrame)
{
	Worker worker;
	EXPECT_TRUE(TestHarrisDetector::testHarrisVoteFrame(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestHarrisDetector::testThreshold(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Testing Harris detector threshold:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int threshold = RandomI::random(512u);

		const int32_t internalThreshold = CV::Detector::HarrisCornerDetector::determineInternalThreshold(threshold);

		const int32_t testInternalThreshold = int32_t((threshold * threshold) / 8u) * int32_t((threshold * threshold) / 8u);

		if (internalThreshold != testInternalThreshold)
		{
			allSucceeded = false;
		}

		const int32_t externalThreshold = CV::Detector::HarrisCornerDetector::determineThreshold(testInternalThreshold);

		if (externalThreshold < 0 || std::abs(externalThreshold - int32_t(threshold)) > 4)
		{
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

bool TestHarrisDetector::testPixelAccuracy(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Testing Harris detector with pixel accuracy:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const unsigned int threshold : {15u, 30u, 50u, 70u})
	{
		Log::info() << " ";
		Log::info() << "... with threshold " << threshold << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		size_t foundFeaturesPerformanceIteration = size_t(-1);

		for (const bool performanceIteration : {true, false})
		{
			const Timestamp startTimestamp(true);

			do
			{
				size_t foundFeaturesSinglecore = size_t(-1);
				size_t foundFeaturesMulticore = size_t(-1);

				Frame yFrame;

				if (performanceIteration)
				{
					if (yFrameTest.isValid())
					{
						yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
					}
					else
					{
						yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u);
						ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
					}
				}
				else
				{
					const unsigned int width = RandomI::random(10u, 1920u);
					const unsigned int height = RandomI::random(7u, 1080u);

					yFrame = Utilities::createRandomFrameWithFeatures(width, height, 2u);
					ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
				}

				for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
				{
					Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
					HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

					size_t& foundFeatures = useWorker ? foundFeaturesMulticore : foundFeaturesSinglecore;

					CV::Detector::HarrisCorners features;

					performance.startIf(performanceIteration);
					CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), threshold, true, features, false, useWorker);
					performance.stopIf(performanceIteration);

					if (performanceIteration)
					{
						ocean_assert(foundFeatures == size_t(-1) || foundFeatures == features.size());
						foundFeatures = features.size();

						if (foundFeaturesPerformanceIteration == size_t(-1))
						{
							// we store the features only for the very first iteration, as we may use random images in each iteration
							foundFeaturesPerformanceIteration = features.size();
						}
					}

					if (yFrame.pixels() >= 100u * 100u && !validatePixelAccuracy(yFrame, threshold, features))
					{
						allSucceeded = false;
					}
				}

				if (maxWorkerIterations != 1u && foundFeaturesSinglecore != foundFeaturesMulticore)
				{
					ocean_assert(false && "This must never happen!");
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Found " << foundFeaturesPerformanceIteration << " Harris features";

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

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

bool TestHarrisDetector::testPixelAccuracyCorners(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 20u && height >= 20u);

	Log::info() << "Testing Harris detector for four corners with pixel accuracy:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		CV::CVUtilities::randomizeFrame(yFrame, false);
		yFrame.setValue(0xFF);

		// we create a black rectangle and detect the four corners

		const unsigned int left = RandomI::random(5u, width / 2u);
		const unsigned int top = RandomI::random(5u, height / 2u);

		const unsigned int right = RandomI::random(left + 5u, width - 6u);
		const unsigned int bottom = RandomI::random(top + 5u, height - 6u);

		yFrame.subFrame(left, top, right - left + 1u, bottom - top + 1u).setValue(0x00);

		CV::Detector::HarrisCorners corners;
		CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), 80u, true, corners, false, &worker);
		ocean_assert(corners.size() == 4);

		bool foundTopLeft = false;
		for (size_t n = 0; n < corners.size(); ++n)
		{
			if (corners[n].observation().x() == Scalar(left) && corners[n].observation().y() == Scalar(top))
			{
				foundTopLeft = true;
			}
		}

		bool foundTopRight = false;
		for (size_t n = 0; n < corners.size(); ++n)
		{
			if (corners[n].observation().x() == Scalar(right) && corners[n].observation().y() == Scalar(top))
			{
				foundTopRight = true;
			}
		}

		bool foundBottomLeft = false;
		for (size_t n = 0; n < corners.size(); ++n)
		{
			if (corners[n].observation().x() == Scalar(left) && corners[n].observation().y() == Scalar(bottom))
			{
				foundBottomLeft = true;
			}
		}

		bool foundBottomRight = false;
		for (size_t n = 0; n < corners.size(); ++n)
		{
			if (corners[n].observation().x() == Scalar(right) && corners[n].observation().y() == Scalar(bottom))
			{
				foundBottomRight = true;
			}
		}

		if (!foundTopLeft || !foundTopRight || !foundBottomLeft || !foundBottomRight)
		{
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

bool TestHarrisDetector::testSubPixelAccuracy(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Testing Harris detector with sub-pixel accuracy:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const unsigned int threshold : {15u, 30u, 50u, 70u})
	{
		Log::info() << " ";
		Log::info() << "... with threshold " << threshold << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		size_t foundFeaturesPerformanceIteration = size_t(-1);

		for (const bool performanceIteration : {true, false})
		{
			const Timestamp startTimestamp(true);

			do
			{
				size_t foundFeaturesSinglecore = size_t(-1);
				size_t foundFeaturesMulticore = size_t(-1);

				Frame yFrame;

				if (performanceIteration)
				{
					if (yFrameTest.isValid())
					{
						yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
					}
					else
					{
						yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u);
						ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
					}
				}
				else
				{
					const unsigned int width = RandomI::random(10u, 1920u);
					const unsigned int height = RandomI::random(7u, 1080u);

					yFrame = Utilities::createRandomFrameWithFeatures(width, height, 2u);
					ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
				}

				for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
				{
					Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
					HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

					size_t& foundFeatures = useWorker ? foundFeaturesMulticore : foundFeaturesSinglecore;

					CV::Detector::HarrisCorners features;

					performance.startIf(performanceIteration);
					CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), threshold, true, features, true, useWorker);
					performance.stopIf(performanceIteration);

					if (performanceIteration)
					{
						ocean_assert(foundFeatures == size_t(-1) || foundFeatures == features.size());
						foundFeatures = features.size();

						if (foundFeaturesPerformanceIteration == size_t(-1))
						{
							// we store the features only for the very first iteration, as we may use random images in each iteration
							foundFeaturesPerformanceIteration = features.size();
						}
					}
				}

				if (maxWorkerIterations != 1u && foundFeaturesSinglecore != foundFeaturesMulticore)
				{
					ocean_assert(false && "This must never happen!");
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Found " << foundFeaturesPerformanceIteration << " Harris features";

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

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

bool TestHarrisDetector::testSubFrameDetection(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-frame detection test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			Frame yFrame;

			if (performanceIteration)
			{
				if (yFrameTest.isValid())
				{
					yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
				}
				else
				{
					yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u);
					ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
				}
			}
			else
			{
				const unsigned int width = RandomI::random(10u * 3u, 1920u);
				const unsigned int height = RandomI::random(7u * 3u, 1080u);

				yFrame = Utilities::createRandomFrameWithFeatures(width, height, 2u);
				ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
			}

			const unsigned int subFrameLeft = RandomI::random(yFrame.width() - 10u);
			const unsigned int subFrameTop = RandomI::random(yFrame.height() - 7u);

			const unsigned int subFrameWidth = RandomI::random(10u, yFrame.width() - subFrameLeft);
			const unsigned int subFrameHeight = RandomI::random(7u, yFrame.height() - subFrameTop);

			ocean_assert(subFrameLeft + subFrameWidth <= yFrame.width());
			ocean_assert(subFrameTop + subFrameHeight <= yFrame.height());

			Worker* useWorker = RandomI::random(1u) == 0u ? &worker : nullptr;

			CV::Detector::HarrisCorners cornersPixelAccuracy;
			if (!CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, 20u, true, cornersPixelAccuracy, false, useWorker))
			{
				allSucceeded = false;
			}

			CV::Detector::HarrisCorners cornersSubPixelAccuracy;
			if (!CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, 20u, true, cornersSubPixelAccuracy, true, useWorker))
			{
				allSucceeded = false;
			}

			if (cornersPixelAccuracy.size() != cornersSubPixelAccuracy.size())
			{
				allSucceeded = false;
			}
			else
			{
				std::set<CV::PixelPosition> pixelPositions;

				for (CV::Detector::HarrisCorners::const_iterator i = cornersPixelAccuracy.begin(); i != cornersPixelAccuracy.end(); ++i)
				{
					pixelPositions.insert(CV::PixelPosition((unsigned int)(i->observation().x()), (unsigned int)(i->observation().y())));
				}

				for (unsigned int n = 0u; n < cornersSubPixelAccuracy.size(); ++n)
				{
					const unsigned int x = Numeric::round32(cornersSubPixelAccuracy[n].observation().x());
					const unsigned int y = Numeric::round32(cornersSubPixelAccuracy[n].observation().y());

					bool found = false;
					for (int yy = -1; !found && yy <= 1; ++yy)
					{
						for (int xx = -1; !found && xx <= 1; ++xx)
						{
							if (pixelPositions.find(CV::PixelPosition(x + xx, y + yy)) != pixelPositions.end())
							{
								found = true;
							}
						}
					}

					if (!found)
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

bool TestHarrisDetector::testCheckerboardDetection(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Checkerboard detection test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int horizontalElements = RandomI::random(2u, 64u);
		const unsigned int verticalElements = RandomI::random(2u, 64u);

		const unsigned int elementWidth = RandomI::random(8u, 30u);
		const unsigned int elementHeight = RandomI::random(8u, 30u);

		const unsigned int width = horizontalElements * elementWidth;
		const unsigned int height = verticalElements * elementHeight;

		const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		const uint8_t bright = uint8_t(RandomI::random(180, 255));
		const uint8_t dark = uint8_t(RandomI::random(0, 90));

		const Frame checkerboardImage = CV::CVUtilities::createCheckerboardImage(width, height, horizontalElements, verticalElements, paddingElements, bright, dark);

		const bool frameIsDistorted = RandomI::random(1u) == 0u;
		const bool determineExactPosition = RandomI::random(1u) == 0u;

		Worker* useWorker = RandomI::random(1u) == 0u ? &worker : nullptr;

		CV::Detector::HarrisCorners corners;
		if (!CV::Detector::HarrisCornerDetector::detectCorners(checkerboardImage, 30u, frameIsDistorted, corners, determineExactPosition, useWorker))
		{
			allSucceeded = false;
		}

		if (corners.size() == (horizontalElements - 1u) * (verticalElements - 1u))
		{
			std::set<IndexPair32> indexPairs;

			for (const CV::Detector::HarrisCorner& corner : corners)
			{
				const Vector2& location = corner.observation();

				ocean_assert(elementWidth != 0u && elementHeight != 0u);
				const unsigned int xIndex = ((unsigned int)(location.x()) + elementWidth / 2u) / elementWidth;
				const unsigned int yIndex = ((unsigned int)(location.y()) + elementHeight / 2u) / elementHeight;

				indexPairs.insert(std::make_pair(xIndex, yIndex));

				// last pixel of left element: elementWidth - 1u
				// first pixel of right element: elementWidth + 0u
				// center between both elements: elementWidth - 0.5
				const Vector2 idealLocation = Vector2(Scalar(elementWidth * xIndex) - Scalar(0.5), Scalar(elementHeight * yIndex) - Scalar(0.5));

				if (location.distance(idealLocation) >= 2)
				{
					// the location of the detected corner is not accurate
					allSucceeded = false;
				}
			}

			if (indexPairs.size() != corners.size())
			{
				// we have detected a corner several times
				allSucceeded = false;
			}
		}
		else
		{
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

bool TestHarrisDetector::testHarrisVotePixel(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Harris vote pixel test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 7u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 7u, 1080u);

		Frame yFrame = Utilities::createRandomFrameWithFeatures(width, height, 2u, &randomGenerator);
		ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);

		Frame sobelResponses = CV::CVUtilities::randomizedFrame(FrameType(yFrame, FrameType::genericPixelFormat<int8_t, 2u>()), &randomGenerator);

		const Frame copySobelResponses(sobelResponses, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 1u>(yFrame.constdata<uint8_t>(), sobelResponses.data<int8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), sobelResponses.paddingElements(), &worker);

		Frame squaredSobelResponse = CV::CVUtilities::randomizedFrame(FrameType(3u, 3u, FrameType::genericPixelFormat<int32_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copySquaredSobelResponse(squaredSobelResponse, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		for (unsigned int y = 2u; y < yFrame.height() - 2u; ++y)
		{
			for (unsigned int x = 2u; x < yFrame.width() - 2u; ++x)
			{
				// we determine the squared sobel responses for a 3x3 neighborhood

				for (unsigned int yy = 0u; yy < 3u; ++yy)
				{
					for (unsigned int xx = 0u; xx < 3u; ++xx)
					{
						const unsigned int frameX = x + xx - 1u;
						const unsigned int frameY = y + yy - 1u;

						CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 8, false>(yFrame.constpixel<uint8_t>(frameX, frameY), yFrame.width(), squaredSobelResponse.pixel<int32_t>(xx, yy), yFrame.paddingElements());
					}
				}

				std::array<int32_t, 3> pixelVotes;

				// sobel responses
				pixelVotes[0] = CV::Detector::HarrisCornerDetector::harrisVotePixel(sobelResponses.constpixel<int8_t>(x, y), sobelResponses.width(), sobelResponses.paddingElements());

				// squared sobel responses
				pixelVotes[1] = CV::Detector::HarrisCornerDetector::harrisVotePixel(squaredSobelResponse.constpixel<int32_t>(1u, 1u), squaredSobelResponse.width(), squaredSobelResponse.paddingElements());

				// y-frame
				pixelVotes[2] = CV::Detector::HarrisCornerDetector::harrisVotePixel(yFrame.constdata<uint8_t>(), yFrame.width(), x, y, yFrame.paddingElements());

				const int32_t testVote = harrisVote3x3<false>(yFrame, x, y);
				const int32_t testVoteRounded = harrisVote3x3<true>(yFrame, x, y);

				for (size_t n = 0; n < pixelVotes.size(); ++n)
				{
					if (pixelVotes[n] != testVote && pixelVotes[n] != testVoteRounded)
					{
						allSucceeded = false;
					}
				}
			}
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(sobelResponses, copySobelResponses))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(squaredSobelResponse, copySquaredSobelResponse))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Response validation: succeeded.";
	}
	else
	{
		Log::info() << "Response validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHarrisDetector::testHarrisVoteFrame(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Harris vote frame test:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			Frame yFrame;

			if (performanceIteration)
			{
				if (yFrameTest.isValid())
				{
					yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
				}
				else
				{
					yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u);
					ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
				}
			}
			else
			{
				const unsigned int width = RandomI::random(10u, 1920u);
				const unsigned int height = RandomI::random(7u, 1080u);

				yFrame = Utilities::createRandomFrameWithFeatures(width, height, 2u);
				ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
			}

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

				Frame votesFrame = CV::CVUtilities::randomizedFrame(FrameType(yFrame, FrameType::genericPixelFormat<int32_t, 1u>()));

				const Frame copyVotesFrame(votesFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const bool setBorderPixels = RandomI::random(1u) == 0u;

				CV::Detector::HarrisCornerDetector::harrisVotesFrame(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), votesFrame.data<int32_t>(), votesFrame.paddingElements(), useWorker, setBorderPixels);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(votesFrame, copyVotesFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				for (unsigned int y = 2u; y < yFrame.height() - 2u; ++y)
				{
					for (unsigned int x = 2u; x < yFrame.width() - 2u; ++x)
					{
						const int32_t vote = votesFrame.constpixel<int32_t>(x, y)[0];

						const int32_t testVote = harrisVote3x3<false>(yFrame, x, y);
						const int32_t testVoteRounded = harrisVote3x3<true>(yFrame, x, y);

						if (vote != testVote && vote != testVoteRounded)
						{
							allSucceeded = false;
						}

						const int32_t pixelVote = CV::Detector::HarrisCornerDetector::harrisVotePixel(yFrame.constdata<uint8_t>(), yFrame.width(), x, y, yFrame.paddingElements());

						if (pixelVote != vote)
						{
							allSucceeded = false;
						}
					}
				}

				if (setBorderPixels)
				{
					for (unsigned int x = 0u; x < votesFrame.width(); ++x)
					{
						if (votesFrame.constpixel<int32_t>(x, 0u)[0] != 0 || votesFrame.constpixel<int32_t>(x, 1u)[0] != 0)
						{
							allSucceeded = false;
						}

						if (votesFrame.constpixel<int32_t>(x, votesFrame.height() - 2u)[0] != 0 || votesFrame.constpixel<int32_t>(x, votesFrame.height() - 1u)[0] != 0)
						{
							allSucceeded = false;
						}
					}

					for (unsigned int y = 0u; y < votesFrame.height(); ++y)
					{
						if (votesFrame.constpixel<int32_t>(0u, y)[0] != 0 || votesFrame.constpixel<int32_t>(1u, y)[0] != 0)
						{
							allSucceeded = false;
						}

						if (votesFrame.constpixel<int32_t>(votesFrame.width() - 2u, y)[0] != 0 || votesFrame.constpixel<int32_t>(votesFrame.width() - 1u, y)[0] != 0)
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Response validation: succeeded.";
	}
	else
	{
		Log::info() << "Response validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHarrisDetector::validatePixelAccuracy(const Frame& yFrame, const unsigned int threshold, const CV::Detector::HarrisCorners& features)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.width() >= 10u && yFrame.height() >= 7u);

	if (!yFrame.isValid() || yFrame.width() < 10u || yFrame.height() < 7u)
	{
		return false;
	}

	Frame voteFrame(FrameType(yFrame, FrameType::genericPixelFormat<int32_t, 1u>()));

	CV::Detector::HarrisCornerDetector::harrisVotesFrame(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), voteFrame.data<int32_t>(), voteFrame.paddingElements());

	for (unsigned int y = 2u; y < yFrame.height() - 2u; ++y)
	{
		for (unsigned int x = 2u; x < yFrame.width() - 2u; ++x)
		{
			const int32_t v0 = voteFrame.constpixel<int32_t>(x, y)[0];
			const int32_t v1 = harrisVote3x3<false>(yFrame, x, y);

			const int32_t tolerance = std::max(std::max(std::abs(v0), std::abs(v1)) * 5 / 100, 5);
			const int32_t difference = std::abs(v0 - v1);

			if (difference > tolerance)
			{
				return false;
			}
		}
	}

	CV::Detector::HarrisCorners testFeatures;
	const unsigned int internalThreshold = (threshold * threshold / 8u) * (threshold * threshold / 8u);

	if (int32_t(internalThreshold) != CV::Detector::HarrisCornerDetector::determineInternalThreshold(threshold))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	for (unsigned int y = 3u; y < yFrame.height() - 3u; ++y)
	{
		for (unsigned int x = 3u; x < yFrame.width() - 3u; ++x)
		{
			const int vote = voteFrame.constpixel<int32_t>(x, y)[0];

			if (vote >= int(internalThreshold))
			{
				bool foundHigherOrEqual = false;
				for (unsigned int xx = x - 1u; !foundHigherOrEqual && xx <= x + 1u; ++xx)
				{
					for (unsigned int yy = y - 1u; !foundHigherOrEqual && yy <= y + 1u; ++yy)
					{
						if (xx != x || yy != y)
						{
							if (voteFrame.constpixel<int32_t>(xx, yy)[0] >= vote)
							{
								foundHigherOrEqual = true;
							}
						}
					}
				}

				if (!foundHigherOrEqual)
				{
					testFeatures.emplace_back(Vector2(Scalar(x), Scalar(y)), CV::Detector::PointFeature::DS_UNDISTORTED, Scalar(vote));
				}
			}
		}
	}

	const size_t toleranceSameNumber = std::max(size_t(20), std::max(features.size(), testFeatures.size()) * 15 / 100);

	if (abs(int(features.size()) - int(testFeatures.size())) > int(toleranceSameNumber))
	{
		return false;
	}

	CV::Detector::HarrisCorners featuresCopy(features);

	std::sort(testFeatures.begin(), testFeatures.end(), sortHarris);
	std::sort(featuresCopy.begin(), featuresCopy.end(), sortHarris);

	unsigned int identicalFeatures = 0u;

	size_t nC = 0;
	size_t nT = 0;

	while (nC < featuresCopy.size() && nT < testFeatures.size())
	{
		const CV::Detector::HarrisCorner& featureCopy = featuresCopy[nC];
		const CV::Detector::HarrisCorner& featureTest = testFeatures[nT];

		if (featureCopy.observation().distance(featureTest.observation()) < 2)
		{
			const Scalar tolerance = std::max(Scalar(25), std::max(featureCopy.strength(), featureTest.strength()) * Scalar(0.45));

			if (std::abs(featureCopy.strength() - featureTest.strength()) <= tolerance)
			{
				++identicalFeatures;
			}

			++nC;
			++nT;

			continue;
		}

		if (sortHarris(featureCopy, featureTest))
		{
			++nC;
		}
		else
		{
			++nT;
		}
	}

	const unsigned int toleranceIdenticalFeatures = (unsigned int)std::min(testFeatures.size(), featuresCopy.size()) * 75u / 100u;

	if (identicalFeatures < toleranceIdenticalFeatures)
	{
		return false;
	}

	return true;
}

template <bool tRoundedDivision>
int TestHarrisDetector::horizontalGradient(const Frame& yFrame, const unsigned int x, const unsigned int y)
{
	ocean_assert(yFrame.isValid());

	ocean_assert(x >= 1u && x + 1u < yFrame.width());
	ocean_assert(y >= 1u && y + 1u < yFrame.height());

	const uint8_t* const yData = yFrame.constdata<uint8_t>();
	const unsigned int strideElements = yFrame.strideElements();

	// -1 0 1
	// -2 0 2
	// -1 0 1

	const int value = yData[(y - 1) * strideElements + (x + 1)] - yData[(y - 1) * strideElements + (x - 1)]
						+ 2 * yData[y * strideElements + (x + 1)] - 2 * yData[y * strideElements + (x - 1)]
						+ yData[(y + 1) * strideElements + (x + 1)] - yData[(y + 1) * strideElements + (x - 1)];

	const int normalizedValue = tRoundedDivision ? roundedDivision(value, 8u) : (value / 8);
	ocean_assert(abs(normalizedValue) <= 127);

	return normalizedValue;
}

template <bool tRoundedDivision>
int TestHarrisDetector::verticalGradient(const Frame& yFrame, const unsigned int x, const unsigned int y)
{
	ocean_assert(yFrame.isValid());

	ocean_assert(x >= 1u && x + 1u < yFrame.width());
	ocean_assert(y >= 1u && y + 1u < yFrame.height());

	const uint8_t* const yData = yFrame.constdata<uint8_t>();
	const unsigned int strideElements = yFrame.strideElements();

	// -1 -2 -1
	//  0  0  0
	//  1  2  1

	const int value = yData[(y + 1) * strideElements + (x - 1)] + 2 * yData[(y + 1) * strideElements + x] + yData[(y + 1) * strideElements + (x + 1)]
						- yData[(y - 1) * strideElements + (x - 1)] - 2 * yData[(y - 1) * strideElements + x] - yData[(y - 1) * strideElements + (x + 1)];

	const int normalizedValue = tRoundedDivision ? roundedDivision(value, 8u) : (value / 8);
	ocean_assert(abs(normalizedValue) <= 127);

	return normalizedValue;
}

template <bool tRoundedDivision>
int TestHarrisDetector::harrisVote3x3(const Frame& yFrame, const unsigned int x, const unsigned int y)
{
	ocean_assert(yFrame.isValid());

	ocean_assert(yFrame.width() >= 10u && yFrame.height() >= 7u);

	ocean_assert(x >= 2u && x + 2u < yFrame.width());
	ocean_assert(y >= 2u && y + 2u < yFrame.height());

	int ixx = 0;
	int iyy = 0;
	int ixy = 0;

	for (unsigned int xx = x - 1u; xx <= x + 1u; ++xx)
	{
		for (unsigned int yy = y - 1u; yy <= y + 1u; ++yy)
		{
			const int hg = horizontalGradient<tRoundedDivision>(yFrame, xx, yy);
			const int vg = verticalGradient<tRoundedDivision>(yFrame, xx, yy);

			ixx += hg * hg;
			iyy += vg * vg;
			ixy += hg * vg;
		}
	}

	// vote = det - k * trace^2
	// det: ixx * iyy - ixy * ixy
	// trace^2: (ixx + iyy)
	// k = 0.05

	if constexpr (tRoundedDivision)
	{
		const int ixx_8 = roundedDivision(ixx, 8u);
		const int iyy_8 = roundedDivision(iyy, 8u);
		const int ixy_8 = roundedDivision(ixy, 8u);

		const int det = ixx_8 * iyy_8 - ixy_8 * ixy_8;
		const int trace = (ixx_8 + iyy_8) * (ixx_8 + iyy_8);

		const int vote = det - roundedDivision(trace * 3, 64u);

		return vote;
	}
	else
	{
		const int det = (ixx / 8) * (iyy / 8) - (ixy / 8) * (ixy / 8);
		const int trace = ((ixx + iyy) / 8) * ((ixx + iyy) / 8);

		return det - ((trace * 3) / 64);
	}
}

bool TestHarrisDetector::sortHarris(const CV::Detector::HarrisCorner& a, const CV::Detector::HarrisCorner& b)
{
	return a.observation().x() < b.observation().x() || (a.observation().x() == b.observation().x() && a.observation().y() < b.observation().y());
}

}

}

}

}
