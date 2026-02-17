/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestMotion.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/Motion.h"
#include "ocean/cv/SumAbsoluteDifferences.h"
#include "ocean/cv/SumSquareDifferences.h"
#include "ocean/cv/ZeroMeanSumSquareDifferences.h"

#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestMotion::test(const double testDuration, Worker& /*worker*/, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Motion test");
	Log::info() << " ";

	if (selector.shouldRun("motionmirroredborder1"))
	{
		testResult = testMotionMirroredBorder<1u>(testDuration);

		Log::info() << "\n-\n";
	}

	if (selector.shouldRun("motionmirroredborder2"))
	{
		testResult = testMotionMirroredBorder<2u>(testDuration);

		Log::info() << "\n-\n";
	}

	if (selector.shouldRun("motionmirroredborder3"))
	{
		testResult = testMotionMirroredBorder<3u>(testDuration);

		Log::info() << "\n-\n";
	}

	if (selector.shouldRun("motionmirroredborder4"))
	{
		testResult = testMotionMirroredBorder<4u>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestMotion, MotionMirroredBorder1ChannelPatchSize5)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<1u, 5u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder1ChannelPatchSize7)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<1u, 7u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder1ChannelPatchSize9)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<1u, 9u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder1ChannelPatchSize15)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<1u, 15u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder1ChannelPatchSize31)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<1u, 31u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder1ChannelPatchSize63)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<1u, 63u>(GTEST_TEST_DURATION)));
}


TEST(TestMotion, MotionMirroredBorder2ChannelPatchSize5)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<2u, 5u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder2ChannelPatchSize7)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<2u, 7u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder2ChannelPatchSize9)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<2u, 9u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder2ChannelPatchSize15)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<2u, 15u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder2ChannelPatchSize31)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<2u, 31u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder2ChannelPatchSize63)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<2u, 63u>(GTEST_TEST_DURATION)));
}


TEST(TestMotion, MotionMirroredBorder3ChannelPatchSize5)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<3u, 5u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder3ChannelPatchSize7)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<3u, 7u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder3ChannelPatchSize9)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<3u, 9u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder3ChannelPatchSize15)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<3u, 15u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder3ChannelPatchSize31)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<3u, 31u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder3ChannelPatchSize63)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<3u, 63u>(GTEST_TEST_DURATION)));
}


TEST(TestMotion, MotionMirroredBorder4ChannelPatchSize5)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<4u, 5u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder4ChannelPatchSize7)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<4u, 7u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder4ChannelPatchSize9)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<4u, 9u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder4ChannelPatchSize15)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<4u, 15u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder4ChannelPatchSize31)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<4u, 31u>(GTEST_TEST_DURATION)));
}

TEST(TestMotion, MotionMirroredBorder4ChannelPatchSize63)
{
	EXPECT_TRUE((TestMotion::testMotionMirroredBorder<4u, 63u>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <unsigned int tChannels>
bool TestMotion::testMotionMirroredBorder(const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testMotionMirroredBorder<tChannels, 5u>(testDuration));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testMotionMirroredBorder<tChannels, 7u>(testDuration));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testMotionMirroredBorder<tChannels, 9u>(testDuration));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testMotionMirroredBorder<tChannels, 15u>(testDuration));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testMotionMirroredBorder<tChannels, 31u>(testDuration));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testMotionMirroredBorder<tChannels, 63u>(testDuration));

	return validation.succeeded();
}

template <unsigned int tChannels, unsigned int tSize>
bool TestMotion::testMotionMirroredBorder(const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid patch size!");
	ocean_assert(testDuration > 0.0);

	const double testDuration_3 = testDuration / 3.0;

	Log::info() << "Motion point " << tChannels * 8u << " bit " << tChannels << " channel " << tSize << "x" << tSize << " test (with mirrored border):";
	Log::info() << " ";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, (testMotionMirroredBorder<CV::SumAbsoluteDifferences, tChannels, tSize>(1920u, 1080u, 1920u, 1080u, testDuration_3)));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, (testMotionMirroredBorder<CV::SumSquareDifferences, tChannels, tSize>(1920u, 1080u, 1920u, 1080u, testDuration_3)));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, (testMotionMirroredBorder<CV::ZeroMeanSumSquareDifferences, tChannels, tSize>(1920u, 1080u, 1920u, 1080u, testDuration_3)));

	return validation.succeeded();
}

template <typename TMetric, unsigned int tChannels, unsigned int tPatchSize>
bool TestMotion::testMotionMirroredBorder(const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

#ifdef OCEAN_USE_GTEST
	constexpr unsigned int constIterations = 10u;
#else
	constexpr unsigned int constIterations = 1000u;
#endif

	if (typeid(TMetric) == typeid(CV::SumAbsoluteDifferences))
	{
		Log::info() << "... for " << width0 << "x" << height0 << ", " << width1 << "x" << height1 << " frames, with SAD, " << constIterations << " iterations:";
	}
	else if (typeid(TMetric) == typeid(CV::SumSquareDifferences))
	{
		Log::info() << "... for " << width0 << "x" << height0 << ", " << width1 << "x" << height1 << " frames, with SSD, " << constIterations << " iterations:";
	}
	else
	{
		ocean_assert(typeid(TMetric) == typeid(CV::ZeroMeanSumSquareDifferences));
		Log::info() << "... for " << width0 << "x" << height0 << ", " << width1 << "x" << height1 << " frames, with Zero-Mean-SSD, " << constIterations << " iterations:";
	}

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	constexpr unsigned int radiusSteps = 4u;

	HighPerformanceStatistic performanceRadius[radiusSteps];

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int radius = 1u; radius <= radiusSteps; ++radius)
		{
			const bool performanceIteration = radius < radiusSteps;

			HighPerformanceStatistic& performance = performanceRadius[radius - 1u];

			const unsigned int radiusX = performanceIteration ? radius : RandomI::random(randomGenerator, 7u);
			const unsigned int radiusY = performanceIteration ? radius : RandomI::random(randomGenerator, radiusX == 0u ? 1u : 0u, 7u);

			const unsigned int testWidth0 = performanceIteration ? width0 : RandomI::random(randomGenerator, tPatchSize, width0);
			const unsigned int testHeight0 = performanceIteration ? height0 : RandomI::random(randomGenerator, tPatchSize, height0);

			const unsigned int testWidth1 = performanceIteration ? width1 : RandomI::random(randomGenerator, tPatchSize, width1);
			const unsigned int testHeight1 = performanceIteration ? height1 : RandomI::random(randomGenerator, tPatchSize, height1);

			const unsigned int frame0PaddingValue = RandomI::random(randomGenerator, 1u, 100u);
			const unsigned int frame0PaddingElements = frame0PaddingValue * RandomI::random(randomGenerator, 1u);

			const unsigned int frame1PaddingValue = RandomI::random(randomGenerator, 1u, 100u);
			const unsigned int frame1PaddingElements = frame1PaddingValue * RandomI::random(randomGenerator, 1u);

			Frame frame0(FrameType(testWidth0, testHeight0, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), frame0PaddingElements);
			Frame frame1(FrameType(frame0, testWidth1, testHeight1), frame1PaddingElements);

			CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
			CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

			CV::PixelPositions positions0;
			positions0.reserve(constIterations);

			CV::PixelPositions roughPositions1;
			roughPositions1.reserve(constIterations);

			while (positions0.size() < constIterations)
			{
				const unsigned int position0X = RandomI::random(randomGenerator, frame0.width() - 1u);
				const unsigned int position0Y = RandomI::random(randomGenerator, frame0.height() - 1u);
				positions0.emplace_back(position0X, position0Y);

				const unsigned int roughPosition1X = RandomI::random(randomGenerator, frame1.width() - 1u);
				const unsigned int roughPosition1Y = RandomI::random(randomGenerator, frame1.height() - 1u);
				roughPositions1.emplace_back(roughPosition1X, roughPosition1Y);
			}

			Indices32 metrics(constIterations);
			CV::PixelPositions positions1(constIterations);

			performance.start();

			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				positions1[n] = CV::MotionT<TMetric>::template pointMotionInFrameMirroredBorder<tChannels, tPatchSize>(frame0.constdata<uint8_t>(), frame1.constdata<uint8_t>(), frame0.width(), frame0.height(), frame1.width(), frame1.height(), positions0[n], radiusX, radiusY, frame0.paddingElements(), frame1.paddingElements(), roughPositions1[n], &metrics[n]);
			}

			performance.stop();

			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				const CV::PixelPosition& position0 = positions0[n];
				const CV::PixelPosition& roughPosition1 = roughPositions1[n];

				unsigned int testSqrDistanceBest = (unsigned int)(-1);
				uint32_t testBest = (unsigned int)(-1);
				CV::PixelPosition testPosition;

				for (int yy1 = int(roughPosition1.y()) - int(radiusY); yy1 <= int(roughPosition1.y() + radiusY); ++yy1)
				{
					for (int xx1 = int(roughPosition1.x()) - int(radiusX); xx1 <= int(roughPosition1.x() + radiusX); ++xx1)
					{
						if (yy1 >= 0 && yy1 < int(frame1.height()) && xx1 >= 0 && xx1 < int(frame1.width()))
						{
							const uint32_t ssdTest = TMetric::template patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(frame0.constdata<uint8_t>(), frame1.constdata<uint8_t>(), frame0.width(), frame0.height(), frame1.width(), frame1.height(), position0.x(), position0.y(), xx1, yy1, frame0.paddingElements(), frame1.paddingElements());

							const CV::PixelPosition candidatePosition(xx1, yy1);

							if (ssdTest < testBest || (ssdTest == testBest && candidatePosition.sqrDistance(roughPosition1) < testSqrDistanceBest))
							{
								testBest = ssdTest;
								testPosition = candidatePosition;
								testSqrDistanceBest = testPosition.sqrDistance(roughPosition1);
							}
						}
					}
				}

				const uint32_t& metric = metrics[n];

				if (testBest != metric)
				{
					OCEAN_SET_FAILED(validation);
				}
				else
				{
					const CV::PixelPosition& position1 = positions1[n];

					if (testPosition != position1)
					{
						const unsigned int sqrDistance = roughPosition1.sqrDistance(position1);
						const unsigned int testSqrDistance = roughPosition1.sqrDistance(testPosition);

						if (sqrDistance != testSqrDistance)
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	for (unsigned int radius = 1u; radius < radiusSteps; ++radius)
	{
		const HighPerformanceStatistic& performance = performanceRadius[radius - 1u];

		Log::info() << constIterations << " Radius " << radius << " performance: [" << performance.bestMseconds() << ", " << performance.medianMseconds() << ", " << performance.worstMseconds() << "] ms";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
