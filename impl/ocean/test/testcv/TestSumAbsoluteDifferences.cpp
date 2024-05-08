/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestSumAbsoluteDifferences.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestSumAbsoluteDifferences::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test sum of absolute differences:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDifferenceBetweenFramesWithOnePlane(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDifferenceBetweenFramesWithSeveralPlanes(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Sum of absolute differences test succeeded.";
	}
	else
	{
		Log::info() << "Sum of absolute differences test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSumAbsoluteDifferences, testDifferenceBetweenFramesWithOnePlane)
{
	EXPECT_TRUE(TestSumAbsoluteDifferences::testDifferenceBetweenFramesWithOnePlane(GTEST_TEST_DURATION));
}

TEST(TestSumAbsoluteDifferences, testDifferenceBetweenFramesWithSeveralPlanes)
{
	EXPECT_TRUE(TestSumAbsoluteDifferences::testDifferenceBetweenFramesWithSeveralPlanes(GTEST_TEST_DURATION));
}

TEST(TestSumAbsoluteDifferences, Patch8BitPerChannel)
{
	EXPECT_TRUE(TestSumAbsoluteDifferences::testPatch8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumAbsoluteDifferences, Buffer8BitPerChannel)
{
	EXPECT_TRUE(TestSumAbsoluteDifferences::testBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumAbsoluteDifferences, PatchBuffer8BitPerChannel)
{
	EXPECT_TRUE(TestSumAbsoluteDifferences::testPatchBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumAbsoluteDifferences, PatchMirroredBorder8BitPerChannel)
{
	EXPECT_TRUE(TestSumAbsoluteDifferences::testPatchMirroredBorder8BitPerChannel(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSumAbsoluteDifferences::testDifferenceBetweenFramesWithOnePlane(const double testDuration)
{
	Log::info() << "Testing SAD between frames with one plane:";
	Log::info() << " ";

	bool allSucceeded = true;

	const std::vector<FrameType::DataType> testDataTypes =
	{
		FrameType::DT_SIGNED_INTEGER_8,
		FrameType::DT_UNSIGNED_INTEGER_8,

		FrameType::DT_SIGNED_INTEGER_16,
		FrameType::DT_UNSIGNED_INTEGER_16,

		FrameType::DT_SIGNED_INTEGER_32,
		FrameType::DT_UNSIGNED_INTEGER_32,

		FrameType::DT_SIGNED_INTEGER_64,
		FrameType::DT_UNSIGNED_INTEGER_64,

		FrameType::DT_SIGNED_FLOAT_32,
		FrameType::DT_SIGNED_FLOAT_64,
	};

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::DataType dataType = RandomI::random(testDataTypes);

		switch (dataType)
		{
			case FrameType::DT_SIGNED_INTEGER_8:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<int8_t>() && allSucceeded;
				break;

			case FrameType::DT_UNSIGNED_INTEGER_8:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<uint8_t>() && allSucceeded;
				break;

			case FrameType::DT_SIGNED_INTEGER_16:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<int16_t>() && allSucceeded;
				break;

			case FrameType::DT_UNSIGNED_INTEGER_16:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<uint16_t>() && allSucceeded;
				break;

			case FrameType::DT_SIGNED_INTEGER_32:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<int32_t>() && allSucceeded;
				break;

			case FrameType::DT_UNSIGNED_INTEGER_32:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<uint32_t>() && allSucceeded;
				break;

			case FrameType::DT_SIGNED_INTEGER_64:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<long long>() && allSucceeded;
				break;

			case FrameType::DT_UNSIGNED_INTEGER_64:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<unsigned long long>() && allSucceeded;
				break;

			case FrameType::DT_SIGNED_FLOAT_32:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<float>() && allSucceeded;
				break;

			case FrameType::DT_SIGNED_FLOAT_64:
				allSucceeded = testDifferenceBetweenFramesWithOnePlane<double>() && allSucceeded;
				break;

			default:
				ocean_assert(false && "Invalid data type!");
				allSucceeded = false;
		};
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "SAD between frames test succeeded.";
	}
	else
	{
		Log::info() << "SAD between frames test FAILED!";
	}

	return allSucceeded;
}

bool TestSumAbsoluteDifferences::testDifferenceBetweenFramesWithSeveralPlanes(const double testDuration)
{
	Log::info() << "Testing SAD between frames with several planes:";
	Log::info() << " ";

	bool allSucceeded = true;

	const FrameType::PixelFormats testPixelFormats =
	{
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_YUV24,
		FrameType::FORMAT_YUVA32,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_Y_VU12,
		FrameType::FORMAT_Y_U_V24,
		FrameType::FORMAT_Y_U_V12
	};

	const std::vector<FrameType::PixelOrigin> pixelOrigins = {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT};

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, testPixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, pixelOrigins);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 2048u) * FrameType::widthMultiple(pixelFormat);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2048u) * FrameType::heightMultiple(pixelFormat);

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);
		ocean_assert(frameType.isValid());

		Indices32 paddingElements0;
		Indices32 paddingElements1;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				paddingElements0.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
			}
		}

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				paddingElements1.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
			}
		}

		ocean_assert(paddingElements0.empty() || paddingElements0.size() == frameType.numberPlanes());
		ocean_assert(paddingElements1.empty() || paddingElements1.size() == frameType.numberPlanes());

		Frame frame0(frameType, paddingElements0);
		Frame frame1(frameType, paddingElements1);

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		Indices32 absoluteDifferences;

		if (CV::SumAbsoluteDifferences::determine(frame0, frame1, absoluteDifferences))
		{
			size_t differenceIndex = 0;

			for (unsigned int planeIndex = 0u; planeIndex < frame0.numberPlanes(); ++planeIndex)
			{
				for (unsigned int planeChannelIndex = 0u; planeChannelIndex < frame0.planeChannels(planeIndex); ++planeChannelIndex)
				{
					uint64_t testSAD = 0ull;

					for (unsigned int y = 0u; y < frame0.planeHeight(planeIndex); ++y)
					{
						for (unsigned int x = 0u; x < frame0.planeWidth(planeIndex); ++x)
						{
							const uint8_t* pixel0 = frame0.constpixel<uint8_t>(x, y, planeIndex);
							const uint8_t* pixel1 = frame1.constpixel<uint8_t>(x, y, planeIndex);

							int32_t difference = int32_t(pixel0[planeChannelIndex]) - int32_t(pixel1[planeChannelIndex]);

							testSAD += NumericT<int32_t>::secureAbs(difference);
						}
					}

					if (differenceIndex < absoluteDifferences.size())
					{
						const uint64_t absoluteDifference = uint64_t(absoluteDifferences[differenceIndex]);

						if (absoluteDifference != testSAD)
						{
							allSucceeded = false;
						}

						++differenceIndex;
					}
					else
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}
				}
			}
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "SAD between frames test succeeded.";
	}
	else
	{
		Log::info() << "SAD between frames test FAILED!";
	}

	return allSucceeded;
}

bool TestSumAbsoluteDifferences::testPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SAD between two patches:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatch8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

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

bool TestSumAbsoluteDifferences::testBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SAD between two buffers:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testBuffer8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 63u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestSumAbsoluteDifferences::testPatchBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SAD between a patch and a buffer:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestSumAbsoluteDifferences::testPatchMirroredBorder8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SAD between two patches with mirrored border:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <typename T>
bool TestSumAbsoluteDifferences::testDifferenceBetweenFramesWithOnePlane()
{
	const unsigned int channels = RandomI::random(1u, 5u);

	switch (channels)
	{
		case 1u:
			return testDifferenceBetweenFramesWithOnePlane<T, 1u>();

		case 2u:
			return testDifferenceBetweenFramesWithOnePlane<T, 2u>();

		case 3u:
			return testDifferenceBetweenFramesWithOnePlane<T, 3u>();

		case 4u:
			return testDifferenceBetweenFramesWithOnePlane<T, 4u>();

		case 5u:
			return testDifferenceBetweenFramesWithOnePlane<T, 5u>();
	};

	ocean_assert(false && "Invalid channel number");
	return false;
}

template <typename T, unsigned int tChannels>
bool TestSumAbsoluteDifferences::testDifferenceBetweenFramesWithOnePlane()
{
	static_assert(tChannels >= 1u && tChannels <= 5u, "Invalid channel number");

	RandomGenerator randomGenerator;

	const unsigned int width = RandomI::random(randomGenerator, 1u, 1000u);
	const unsigned int height = RandomI::random(randomGenerator, 1u, 1000u);

	const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
	const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

	Frame frameA(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
	Frame frameB(FrameType(frameA, width, height), targetPaddingElements);

	// we apply a custom randomizing to avoid too extreme values

	const int minValueRange = SignedTyper<T>::isSigned ? -127 : 0;
	const int maxValueRange = SignedTyper<T>::isSigned ? 127 : 255;

	T* const dataA = frameA.data<T>();
	for (unsigned int n = 0u; n < frameA.strideElements() * frameA.height(); ++n)
	{
		dataA[n] = T(RandomI::random(randomGenerator, minValueRange, maxValueRange));
	}

	T* const dataB = frameB.data<T>();
	for (unsigned int n = 0u; n < frameB.strideElements() * frameB.height(); ++n)
	{
		dataB[n] = T(RandomI::random(randomGenerator, minValueRange, maxValueRange));
	}

	typename AbsoluteDifferenceValueTyper<T>::Type absoluteDifferences[tChannels];

	CV::SumAbsoluteDifferences::determine<T, tChannels>(frameA.constdata<T>(), frameB.constdata<T>(), width, height, absoluteDifferences, frameA.paddingElements(), frameB.paddingElements());

	double testAbsoluteDifferences[tChannels];

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		testAbsoluteDifferences[n] = 0.0;
	}

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const pixelA = frameA.constpixel<T>(x, y);
			const T* const pixelB = frameB.constpixel<T>(x, y);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const double absoluteDifference = NumericD::abs(double(pixelA[n]) - double(pixelB[n]));

				testAbsoluteDifferences[n] += absoluteDifference;
			}
		}
	}

	const double threshold = 1.0;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		const double absoluteDifference = double(absoluteDifferences[n]);

		if (NumericD::isNotEqual(absoluteDifference, testAbsoluteDifferences[n], threshold))
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestSumAbsoluteDifferences::testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
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

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = tPatchSize_2;
		centersY0[0] = tPatchSize_2;
		centersX1[0] = tPatchSize_2;
		centersY1[0] = tPatchSize_2;

		centersX0[1] = width0 - tPatchSize_2 - 1u;
		centersY0[1] = height0 - tPatchSize_2 - 1u;
		centersX1[1] = width1 - tPatchSize_2 - 1u;
		centersY1[1] = height1 - tPatchSize_2 - 1u;

		for (unsigned int n = 2u; n < locations; ++n)
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
						resultsNaive[n] = CV::SumAbsoluteDifferencesBase::patch8BitPerChannel(data0, data1, tChannels, tPatchSize, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumAbsoluteDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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

						const unsigned int patch0StrideElements = frame0.strideElements();
						const unsigned int patch1StrideElements = frame1.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const patch1 = data1 + (centersY1[n] - tPatchSize_2) * patch1StrideElements + (centersX1[n] - tPatchSize_2) * tChannels;

							resultsSSE[n] = CV::SumAbsoluteDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements);
						}
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

						const unsigned int patch0StrideElements = frame0.strideElements();
						const unsigned int patch1StrideElements = frame1.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const patch1 = data1 + (centersY1[n] - tPatchSize_2) * patch1StrideElements + (centersX1[n] - tPatchSize_2) * tChannels;

							resultsNEON[n] = CV::SumAbsoluteDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements);
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
						resultsDefault[n] = CV::SumAbsoluteDifferences::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			uint32_t sadTest = 0u;

			const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, centersY0[n] - tPatchSize_2);
			const uint8_t* pixel1 = frame1.constpixel<uint8_t>(centersX1[n] - tPatchSize_2, centersY1[n] - tPatchSize_2);

			for (unsigned int y = 0u; y < tPatchSize; ++y)
			{
				for (unsigned int i = 0u; i < tChannels * tPatchSize; ++i)
				{
					const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
					sadTest += uint32_t(abs(value));
				}

				pixel0 += frame0.strideElements();
				pixel1 += frame1.strideElements();
			}

			if (!resultsNaive.empty() && resultsNaive[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != sadTest)
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

template <unsigned int tChannels, unsigned int tPixels>
bool TestSumAbsoluteDifferences::testBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid pixels");

	ocean_assert(width * height >= tPixels);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tBufferSize = tChannels * tPixels;

	Log::info() << "... with " << tChannels << " channels and " << tPixels << " pixels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 offsets0(locations);
	Indices32 offsets1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		Frame frame0(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame frame1(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		offsets0[0] = 0;
		offsets1[0] = 0;
		
		offsets0[1] = height * frame0.strideElements() - tBufferSize;
		offsets1[1] = height * frame1.strideElements() - tBufferSize;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			offsets0[n] = RandomI::random(randomGenerator, height * frame0.strideElements() - tBufferSize);
			offsets1[n] = RandomI::random(randomGenerator, height * frame1.strideElements() - tBufferSize);
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
						resultsNaive[n] = CV::SumAbsoluteDifferencesBase::buffer8BitPerChannel(data0 + offsets0[n], data1 + offsets1[n], tBufferSize);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumAbsoluteDifferencesBase::buffer8BitPerChannelTemplate<tBufferSize>(data0 + offsets0[n], data1 + offsets1[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPixels >= 8u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsSSE[n] = CV::SumAbsoluteDifferencesSSE::buffer8BitPerChannel<tBufferSize>(data0 + offsets0[n], data1 + offsets1[n]);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tPixels >= 8u)
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsNEON[n] = CV::SumAbsoluteDifferencesNEON::buffer8BitPerChannel<tBufferSize>(data0 + offsets0[n], data1 + offsets1[n]);
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
						resultsDefault[n] = CV::SumAbsoluteDifferences::buffer8BitPerChannel<tChannels, tPixels>(data0 + offsets0[n], data1 + offsets1[n]);
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
			uint32_t sadTest = 0u;

			const uint8_t* const pixel0 = data0 + offsets0[n];
			const uint8_t* const pixel1 = data1 + offsets1[n];

			for (unsigned int i = 0u; i < tBufferSize; ++i)
			{
				const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
				sadTest += uint32_t(abs(value));
			}

			if (!resultsNaive.empty() && resultsNaive[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != sadTest)
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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestSumAbsoluteDifferences::testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tBufferSize = tChannels * tPatchSize * tPatchSize;

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
	Indices32 offsets1(locations);

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

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = tPatchSize_2;
		centersY0[0] = tPatchSize_2;
		offsets1[0] = 0;

		centersX0[1] = width0 - tPatchSize_2 - 1u;
		centersY0[1] = height0 - tPatchSize_2 - 1u;
		offsets1[1] = height1 * frame1.strideElements() - tBufferSize;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, tPatchSize_2, width0 - tPatchSize_2 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, tPatchSize_2, height0 - tPatchSize_2 - 1u);

			offsets1[n] = RandomI::random(randomGenerator, height1 * frame1.strideElements() - tBufferSize);
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
						resultsNaive[n] = CV::SumAbsoluteDifferencesBase::patchBuffer8BitPerChannel(data0, tChannels, tPatchSize, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumAbsoluteDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
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

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const buffer1 = data1 + offsets1[n];

							resultsSSE[n] = CV::SumAbsoluteDifferencesSSE::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements);
						}
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

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const buffer1 = data1 + offsets1[n];

							resultsNEON[n] = CV::SumAbsoluteDifferencesNEON::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements);
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
						resultsDefault[n] = CV::SumAbsoluteDifferences::patchBuffer8BitPerChannel<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
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
			uint32_t sadTest = 0u;

			const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, centersY0[n] - tPatchSize_2);
			const uint8_t* pixel1 = frame1.constdata<uint8_t>() + offsets1[n];

			for (unsigned int y = 0u; y < tPatchSize; ++y)
			{
				for (unsigned int i = 0u; i < tChannels * tPatchSize; ++i)
				{
					const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
					sadTest += uint32_t(abs(value));
				}

				pixel0 += frame0.strideElements();
				pixel1 += tChannels * tPatchSize;
			}

			if (!resultsNaive.empty() && resultsNaive[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != sadTest)
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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestSumAbsoluteDifferences::testPatchMirroredBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << "):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

	Indices32 resultsTemplate;
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

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = 0u;
		centersY0[0] = 0u;
		centersX1[0] = 0u;
		centersY1[0] = 0u;

		centersX0[1] = width0 - 1u;
		centersY0[1] = height0 - 1u;
		centersX1[1] = width1 - 1u;
		centersY1[1] = height1 - 1u;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, 0u, width0 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, 0u, height0 - 1u);

			centersX1[n] = RandomI::random(randomGenerator, 0u, width1 - 1u);
			centersY1[n] = RandomI::random(randomGenerator, 0u, height1 - 1u);
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_TEMPLATE, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumAbsoluteDifferencesBase::patchMirroredBorder8BitPerChannelTemplate<tChannels>(data0, data1, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::SumAbsoluteDifferences::patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			uint32_t sadTest = 0u;

			int y1 = int(centersY1[n]) - int(tPatchSize_2);
			for (int y0 = int(centersY0[n]) - int(tPatchSize_2); y0 <= int(centersY0[n]) + int(tPatchSize_2); ++y0)
			{
				const unsigned int yMirror0 = y0 + CV::CVUtilities::mirrorOffset(y0, height0);
				const unsigned int yMirror1 = y1 + CV::CVUtilities::mirrorOffset(y1, height1);

				int x1 = int(centersX1[n]) - int(tPatchSize_2);
				for (int x0 = int(centersX0[n]) - int(tPatchSize_2); x0 <= int(centersX0[n]) + int(tPatchSize_2); ++x0)
				{
					const unsigned int xMirror0 = x0 + CV::CVUtilities::mirrorOffset(x0, width0);
					const unsigned int xMirror1 = x1 + CV::CVUtilities::mirrorOffset(x1, width1);

					const uint8_t* const pixel0 = frame0.constpixel<uint8_t>(xMirror0, yMirror0);
					const uint8_t* const pixel1 = frame1.constpixel<uint8_t>(xMirror1, yMirror1);

					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						const int32_t value = int32_t(pixel0[c]) - int32_t(pixel1[c]);
						sadTest += uint32_t(abs(value));
					}

					++x1;
				}

				++y1;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != sadTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != sadTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
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
