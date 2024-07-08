/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestUtilities.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Utilities test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCopyPixel<uint8_t, 1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixel<uint8_t, 2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixel<uint8_t, 3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixel<uint8_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCopyPixel<int16_t, 1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixel<int16_t, 2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixel<int16_t, 3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixel<int16_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyPixelWithIndex<uint8_t, 1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithIndex<uint8_t, 2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithIndex<uint8_t, 3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithIndex<uint8_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCopyPixelWithIndex<uint16_t, 1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithIndex<uint16_t, 2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithIndex<uint16_t, 3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithIndex<uint16_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyPixelWithPosition<uint8_t, 1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithPosition<uint8_t, 2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithPosition<uint8_t, 3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithPosition<uint8_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCopyPixelWithPosition<uint16_t, 1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithPosition<uint16_t, 2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithPosition<uint16_t, 3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyPixelWithPosition<uint16_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Utilities test succeeded.";
	}
	else
	{
		Log::info() << "Utilities test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestUtilities, CopyPixel_uint8_1)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint8_t, 1u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixel_uint8_2)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint8_t, 2u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixel_uint8_3)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint8_t, 3u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixel_uint8_4)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint8_t, 4u>(GTEST_TEST_DURATION)));
}


TEST(TestUtilities, CopyPixel_uint16_1)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint16_t, 1u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixel_uint16_2)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint16_t, 2u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixel_uint16_3)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint16_t, 3u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixel_uint16_4)
{
	EXPECT_TRUE((TestUtilities::testCopyPixel<uint16_t, 4u>(GTEST_TEST_DURATION)));
}


TEST(TestUtilities, CopyPixelWithIndex_uint8_1)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint8_t, 1u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithIndex_uint8_2)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint8_t, 2u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithIndex_uint8_3)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint8_t, 3u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithIndex_uint8_4)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint8_t, 4u>(GTEST_TEST_DURATION)));
}


TEST(TestUtilities, CopyPixelWithIndex_uint16_1)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint16_t, 1u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithIndex_uint16_2)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint16_t, 2u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithIndex_uint16_3)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint16_t, 3u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithIndex_uint16_4)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithIndex<uint16_t, 4u>(GTEST_TEST_DURATION)));
}


TEST(TestUtilities, CopyPixelWithPosition_uint8_1)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint8_t, 1u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithPosition_uint8_2)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint8_t, 2u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithPosition_uint8_3)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint8_t, 3u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithPosition_uint8_4)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint8_t, 4u>(GTEST_TEST_DURATION)));
}


TEST(TestUtilities, CopyPixelWithPosition_uint16_1)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint16_t, 1u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithPosition_uint16_2)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint16_t, 2u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithPosition_uint16_3)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint16_t, 3u>(GTEST_TEST_DURATION)));
}

TEST(TestUtilities, CopyPixelWithPosition_uint16_4)
{
	EXPECT_TRUE((TestUtilities::testCopyPixelWithPosition<uint16_t, 4u>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T, unsigned int tChannels>
bool TestUtilities::testCopyPixel(const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Copy pixel with " << TypeNamer::name<T>() << ", and " << tChannels << " channels test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing one pixel memory

			std::array<T, tChannels> sourcePixel;
			std::array<T, tChannels + 1u> targetPixel;

			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				sourcePixel[channelIndex] = T(RandomI::random(randomGenerator, 255));
				targetPixel[channelIndex] = T(RandomI::random(randomGenerator, 255));
			}

			const T targetPadding = T(RandomI::random(randomGenerator, 255));

			targetPixel[tChannels] = targetPadding;

			CV::CVUtilities::copyPixel<T, tChannels>(targetPixel.data(), sourcePixel.data());

			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				if (targetPixel[channelIndex] != sourcePixel[channelIndex])
				{
					allSucceeded = false;
				}
			}

			if (targetPixel[tChannels] != targetPadding)
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}
		}

		{
			// testing frame

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

			const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int xTarget = RandomI::random(width - 1u);
				const unsigned int yTarget = RandomI::random(height - 1u);

				const unsigned int xSource = RandomI::random(width - 1u);
				const unsigned int ySource = RandomI::random(height - 1u);

				CV::CVUtilities::copyPixel<T, tChannels>(frame.pixel<T>(xTarget, yTarget), frame.constpixel<T>(xSource, ySource));

				for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
				{
					const T sourceValue = frame.constpixel<T>(xSource, ySource)[channelIndex];
					const T targetValue = frame.constpixel<T>(xTarget, yTarget)[channelIndex];

					if (sourceValue != targetValue)
					{
						allSucceeded = false;
					}
				}
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
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

template <typename T, unsigned int tChannels>
bool TestUtilities::testCopyPixelWithIndex(const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Copy pixel with index for " << TypeNamer::name<T>() << ", and " << tChannels << " channels test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing one pixel memory

			std::array<T, tChannels> sourcePixel;
			std::array<T, tChannels + 1u> targetPixel;

			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				sourcePixel[channelIndex] = T(RandomI::random(randomGenerator, 255));
				targetPixel[channelIndex] = T(RandomI::random(randomGenerator, 255));
			}

			const T targetPadding = T(RandomI::random(randomGenerator, 255));

			targetPixel[tChannels] = targetPadding;

			CV::CVUtilities::copyPixel<T, tChannels>(targetPixel.data(), sourcePixel.data(), 0u, 0u);

			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				if (targetPixel[channelIndex] != sourcePixel[channelIndex])
				{
					allSucceeded = false;
				}
			}

			if (targetPixel[tChannels] != targetPadding)
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}
		}

		{
			// testing frame

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			Frame source(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
			Frame target(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));

			ocean_assert(source.isContinuous() && target.isContinuous());

			CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
			CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int xTarget = RandomI::random(width - 1u);
				const unsigned int yTarget = RandomI::random(height - 1u);

				const unsigned int xSource = RandomI::random(width - 1u);
				const unsigned int ySource = RandomI::random(height - 1u);

				const unsigned int indexTarget = yTarget * target.width() + xTarget;
				const unsigned int indexSource = ySource * source.width() + xSource;

				CV::CVUtilities::copyPixel<T, tChannels>(target.data<T>(), source.constdata<T>(), indexTarget, indexSource);

				for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
				{
					const T sourceValue = source.constpixel<T>(xSource, ySource)[channelIndex];
					const T targetValue = target.constpixel<T>(xTarget, yTarget)[channelIndex];

					if (sourceValue != targetValue)
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

template <typename T, unsigned int tChannels>
bool TestUtilities::testCopyPixelWithPosition(const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Copy pixel with position for " << TypeNamer::name<T>() << ", and " << tChannels << " channels test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing one pixel memory

			std::array<T, tChannels> sourcePixel;
			std::array<T, tChannels + 1u> targetPixel;

			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				sourcePixel[channelIndex] = T(RandomI::random(randomGenerator, 255));
				targetPixel[channelIndex] = T(RandomI::random(randomGenerator, 255));
			}

			const T targetPadding = T(RandomI::random(randomGenerator, 255));

			targetPixel[tChannels] = targetPadding;

			CV::CVUtilities::copyPixel<T, tChannels>(targetPixel.data(), sourcePixel.data(), 0u, 0u, 0u, 0u, 1u, 1u, 0u, 0u);

			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				if (targetPixel[channelIndex] != sourcePixel[channelIndex])
				{
					allSucceeded = false;
				}
			}

			if (targetPixel[tChannels] != targetPadding)
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}
		}

		{
			// testing frame

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame source(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			Frame target(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);

			CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
			CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);

			const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int xTarget = RandomI::random(width - 1u);
				const unsigned int yTarget = RandomI::random(height - 1u);

				const unsigned int xSource = RandomI::random(width - 1u);
				const unsigned int ySource = RandomI::random(height - 1u);

				CV::CVUtilities::copyPixel<T, tChannels>(target.data<T>(), source.constdata<T>(), xTarget, yTarget, xSource, ySource, target.width(), source.width(), target.paddingElements(), source.paddingElements());

				for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
				{
					const T sourceValue = source.constpixel<T>(xSource, ySource)[channelIndex];
					const T targetValue = target.constpixel<T>(xTarget, yTarget)[channelIndex];

					if (sourceValue != targetValue)
					{
						allSucceeded = false;
					}
				}
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
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

}

}

}
