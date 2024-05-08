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

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestMotion::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Motion test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testMotionMirroredBorder<1u>(testDuration) && allSucceeded;

	Log::info() << "\n-\n";

	allSucceeded = testMotionMirroredBorder<2u>(testDuration) && allSucceeded;

	Log::info() << "\n-\n";

	allSucceeded = testMotionMirroredBorder<3u>(testDuration) && allSucceeded;

	Log::info() << "\n-\n";

	allSucceeded = testMotionMirroredBorder<4u>(testDuration) && allSucceeded;

	Log::info() << " ";


	if (allSucceeded)
	{
		Log::info() << "Motion test succeeded.";
	}
	else
	{
		Log::info() << "Motion test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	allSucceeded = testMotionMirroredBorder<tChannels, 5u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMotionMirroredBorder<tChannels, 7u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMotionMirroredBorder<tChannels, 9u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMotionMirroredBorder<tChannels, 15u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMotionMirroredBorder<tChannels, 31u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMotionMirroredBorder<tChannels, 63u>(testDuration) && allSucceeded;

	return allSucceeded;
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

	bool allSucceeded = true;

	allSucceeded = testMotionMirroredBorder<CV::SumAbsoluteDifferences, tChannels, tSize>(1920u, 1080u, 1920u, 1080u, testDuration_3) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMotionMirroredBorder<CV::SumSquareDifferences, tChannels, tSize>(1920u, 1080u, 1920u, 1080u, testDuration_3) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMotionMirroredBorder<CV::ZeroMeanSumSquareDifferences, tChannels, tSize>(1920u, 1080u, 1920u, 1080u, testDuration_3) && allSucceeded;

	return allSucceeded;
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

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

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

			const unsigned int frame0PaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
			const unsigned int frame1PaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

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
				positions0.emplace_back(RandomI::random(randomGenerator, frame0.width() - 1u), RandomI::random(randomGenerator, frame0.height() - 1u));
				roughPositions1.emplace_back(RandomI::random(randomGenerator, frame1.width() - 1u), RandomI::random(randomGenerator, frame1.height() - 1u));
			}

			Indices32 metrics(constIterations);
			CV::PixelPositions positions1(constIterations);

			performance.start();

			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				positions1[n] = CV::Motion<TMetric>::template pointMotionInFrameMirroredBorder<tChannels, tPatchSize>(frame0.constdata<uint8_t>(), frame1.constdata<uint8_t>(), frame0.width(), frame0.height(), frame1.width(), frame1.height(), positions0[n], radiusX, radiusY, frame0.paddingElements(), frame1.paddingElements(), roughPositions1[n], &metrics[n]);
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
					allSucceeded = false;
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
							allSucceeded = false;
						}
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	for (unsigned int radius = 1u; radius < radiusSteps; ++radius)
	{
		const HighPerformanceStatistic& performance = performanceRadius[radius - 1u];

		Log::info() << constIterations << " Radius " << radius << " performance: [" << performance.bestMseconds() << ", " << performance.medianMseconds() << ", " << performance.worstMseconds() << "] ms";
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

}

}

}
