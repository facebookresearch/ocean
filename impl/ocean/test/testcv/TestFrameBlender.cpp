/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameBlender.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameBlender.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameBlender::test(const double testDuration, Worker& worker)
{
	Log::info() << "---   Frame blender test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstantAlpha(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparateAlphaChannel<true>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testSeparateAlphaChannel<false>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBlend<true>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBlend<false>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBlendWithConstantValue<true>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testBlendWithConstantValue<false>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame blender test succeeded.";
	}
	else
	{
		Log::info() << "Frame blender test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameBlender, ConstantAlpha)
{
	Worker worker;
	EXPECT_TRUE(TestFrameBlender::testConstantAlpha(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameBlender, SeparateAlphaChannel_Transparency0xFF)
{
	Worker worker;
	EXPECT_TRUE((TestFrameBlender::testSeparateAlphaChannel<true>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameBlender, SeparateAlphaChannel_Opaque0xFF)
{
	Worker worker;
	EXPECT_TRUE((TestFrameBlender::testSeparateAlphaChannel<false>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameBlender, Blend_Transparency0xFF)
{
	Worker worker;
	EXPECT_TRUE((TestFrameBlender::testBlend<true>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameBlender, Blend_Opaque0xFF)
{
	Worker worker;
	EXPECT_TRUE((TestFrameBlender::testBlend<false>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameBlender, BlendWithConstantValue_Transparency0xFF)
{
	Worker worker;
	EXPECT_TRUE((TestFrameBlender::testBlendWithConstantValue<true>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameBlender, BlendWithConstantValue_Opaque0xFF)
{
	Worker worker;
	EXPECT_TRUE((TestFrameBlender::testBlendWithConstantValue<false>(GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestFrameBlender::testConstantAlpha(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test constant alpha value:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const FrameType frameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const uint8_t alphaValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		switch (sourceFrame.channels())
		{
			case 1u:
				CV::FrameBlender::blend8BitPerChannel<1u, true>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaValue, sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 2u:
				CV::FrameBlender::blend8BitPerChannel<2u, true>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaValue, sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 3u:
				CV::FrameBlender::blend8BitPerChannel<3u, true>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaValue, sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 4u:
				CV::FrameBlender::blend8BitPerChannel<4u, true>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaValue, sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 5u:
				CV::FrameBlender::blend8BitPerChannel<5u, true>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaValue, sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			default:
				ocean_assert(false && "Invalid channels!");
				allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			allSucceeded = false;
			break;
		}

		for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
			{
				const uint8_t* sourcePixel = sourceFrame.constpixel<uint8_t>(x, y);
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, y);
				const uint8_t* copyTargetPixel = copyTargetFrame.constpixel<uint8_t>(x, y);

				for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
				{
					const int value = (int(sourcePixel[c]) * (255 - int(alphaValue)) + int(copyTargetPixel[c]) * int(alphaValue) + 127) / 255;

					if (value != int(targetPixel[c]))
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

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testSeparateAlphaChannel(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test separate alpha channel function with 0xFF interpreted as fully " << (tTransparentIs0xFF ? "transparent" : "opaque");
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSeparateAlphaChannelFullFrame<tTransparentIs0xFF>(testDuration, worker) && allSucceeded;

	allSucceeded = testSeparateAlphaChannelSubFrame<tTransparentIs0xFF>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	return allSucceeded;
}

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testBlend(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test blend function with 0xFF interpreted as fully " << (tTransparentIs0xFF ? "transparent" : "opaque");
	Log::info() << " ";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_BGRA32,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_YUVA32,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_ARGB32
	};

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		ocean_assert(FrameType::formatHasAlphaChannel(pixelFormat));

		const FrameType::PixelFormat pixelFormatWithoutAlpha = FrameType::formatRemoveAlphaChannel(pixelFormat);

		// test the YA16 pixel format
		allSucceeded = testFullFrame<tTransparentIs0xFF>(pixelFormat, pixelFormatWithoutAlpha, width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testFullFrame<tTransparentIs0xFF>(pixelFormat, pixelFormat, width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << "Testing sub-regions:";
	Log::info() << " ";

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		ocean_assert(FrameType::formatHasAlphaChannel(pixelFormat));

		const FrameType::PixelFormat pixelFormatWithoutAlpha = FrameType::formatRemoveAlphaChannel(pixelFormat);

		// test the YA16 pixel format
		allSucceeded = testSubFrame<tTransparentIs0xFF>(pixelFormat, pixelFormatWithoutAlpha, width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testSubFrame<tTransparentIs0xFF>(pixelFormat, pixelFormat, width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	return allSucceeded;
}

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testBlendWithConstantValue(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test constant value blend function with 0xFF interpreted as fully " << (tTransparentIs0xFF ? "transparent" : "opaque");
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		const Frame alphaFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame valueFrame = CV::CVUtilities::randomizedFrame(FrameType(1u, 1u, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		switch (targetFrame.channels())
		{
			case 1u:
				CV::FrameBlender::blend8BitPerChannel<1u, tTransparentIs0xFF>(alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaFrame.width(), alphaFrame.height(), valueFrame.constdata<uint8_t>(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 2u:
				CV::FrameBlender::blend8BitPerChannel<2u, tTransparentIs0xFF>(alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaFrame.width(), alphaFrame.height(), valueFrame.constdata<uint8_t>(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 3u:
				CV::FrameBlender::blend8BitPerChannel<3u, tTransparentIs0xFF>(alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaFrame.width(), alphaFrame.height(), valueFrame.constdata<uint8_t>(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 4u:
				CV::FrameBlender::blend8BitPerChannel<4u, tTransparentIs0xFF>(alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaFrame.width(), alphaFrame.height(), valueFrame.constdata<uint8_t>(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 5u:
				CV::FrameBlender::blend8BitPerChannel<5u, tTransparentIs0xFF>(alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), alphaFrame.width(), alphaFrame.height(), valueFrame.constdata<uint8_t>(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			default:
				ocean_assert(false && "Invalid channels!");
				allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			allSucceeded = false;
			break;
		}

		for (unsigned int y = 0u; y < targetFrame.height(); ++y)
		{
			const uint8_t* valuePixel = valueFrame.constdata<uint8_t>();

			for (unsigned int x = 0u; x < targetFrame.width(); ++x)
			{
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, y);
				const uint8_t* copyTargetPixel = copyTargetFrame.constpixel<uint8_t>(x, y);

				const uint8_t alphaValue = *alphaFrame.constpixel<uint8_t>(x, y);

				for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
				{
					const int alphaFactor = tTransparentIs0xFF ? int(alphaValue) : 255 - int(alphaValue);

					const int value = (int(valuePixel[c]) * (255 - alphaFactor) + int(copyTargetPixel[c]) * alphaFactor + 127) / 255;

					if (value != int(targetPixel[c]))
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

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testSeparateAlphaChannelSubFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a sub frame";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int targetWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame alphaFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const unsigned int width = RandomI::random(randomGenerator, 1u, std::min(sourceWidth, targetWidth));
		const unsigned int height = RandomI::random(randomGenerator, 1u, std::min(sourceHeight, targetHeight));

		const unsigned int sourceLeft = RandomI::random(randomGenerator, sourceWidth - width);
		const unsigned int sourceTop = RandomI::random(randomGenerator, sourceHeight - height);

		const unsigned int targetLeft = RandomI::random(randomGenerator, targetWidth - width);
		const unsigned int targetTop = RandomI::random(randomGenerator, targetHeight - height);

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		switch (sourceFrame.channels())
		{
			case 1u:
				CV::FrameBlender::blend8BitPerChannel<1u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 2u:
				CV::FrameBlender::blend8BitPerChannel<2u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 3u:
				CV::FrameBlender::blend8BitPerChannel<3u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 4u:
				CV::FrameBlender::blend8BitPerChannel<4u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 5u:
				CV::FrameBlender::blend8BitPerChannel<5u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			default:
				ocean_assert(false && "Invalid channels!");
				allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			allSucceeded = false;
			break;
		}

		for (unsigned int y = 0u; y < targetFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < targetFrame.width(); ++x)
			{
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, y);
				const uint8_t* copyTargetPixel = copyTargetFrame.constpixel<uint8_t>(x, y);

				if (x >= targetLeft && x < targetLeft + width
						&& y >= targetTop && y < targetTop + height)
				{
					const unsigned int sx = x - targetLeft + sourceLeft;
					const unsigned int sy = y - targetTop + sourceTop;

					const uint8_t* sourcePixel = sourceFrame.constpixel<uint8_t>(sx, sy);
					const uint8_t alphaValue = *alphaFrame.constpixel<uint8_t>(sx, sy);

					for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
					{
						const int alphaFactor = tTransparentIs0xFF ? int(alphaValue) : 255 - int(alphaValue);

						const int value = (int(sourcePixel[c]) * (255 - alphaFactor) + int(copyTargetPixel[c]) * alphaFactor + 127) / 255;

						if (value != int(targetPixel[c]))
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
					{
						if (targetPixel[c] != copyTargetPixel[c])
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
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testSeparateAlphaChannelFullFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a full frame";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame alphaFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame.frameType(), FrameType::FORMAT_Y8), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(sourceFrame.frameType(), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		switch (sourceFrame.channels())
		{
			case 1u:
				CV::FrameBlender::blend8BitPerChannel<1u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 2u:
				CV::FrameBlender::blend8BitPerChannel<2u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 3u:
				CV::FrameBlender::blend8BitPerChannel<3u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 4u:
				CV::FrameBlender::blend8BitPerChannel<4u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			case 5u:
				CV::FrameBlender::blend8BitPerChannel<5u, tTransparentIs0xFF>(sourceFrame.constdata<uint8_t>(), alphaFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), alphaFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				break;

			default:
				ocean_assert(false && "Invalid channels!");
				allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			allSucceeded = false;
			break;
		}

		for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
			{
				const uint8_t* sourcePixel = sourceFrame.constpixel<uint8_t>(x, y);
				const uint8_t alphaValue = *alphaFrame.constpixel<uint8_t>(x, y);
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, y);
				const uint8_t* copyTargetPixel = copyTargetFrame.constpixel<uint8_t>(x, y);

				for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
				{
					const int alphaFactor = tTransparentIs0xFF ? int(alphaValue) : 255 - int(alphaValue);

					const int value = (int(sourcePixel[c]) * (255 - alphaFactor) + int(copyTargetPixel[c]) * alphaFactor + 127) / 255;

					if (value != int(targetPixel[c]))
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

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testSubFrame(const FrameType::PixelFormat sourcePixelFormat, const FrameType::PixelFormat targetPixelFormat, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << targetWidth << "x" << targetHeight << " with " << FrameType::translatePixelFormat(sourcePixelFormat) << " -> " << FrameType::translatePixelFormat(targetPixelFormat);

	ocean_assert(FrameType::formatHasAlphaChannel(sourcePixelFormat));

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool useWorker : {false, true})
		{
			const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, targetWidth);
			const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, targetHeight);

			const Frame sourceFrameWithAlpha = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(targetWidth, targetHeight, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame targetFrameCopy(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const unsigned int width = RandomI::random(1u, sourceWidth);
			const unsigned int height = RandomI::random(1u, sourceHeight);

			const unsigned int targetLeft = RandomI::random(0u, targetWidth - width);
			const unsigned int targetTop = RandomI::random(0u, targetHeight - height);

			const unsigned int sourceLeft = RandomI::random(0u, sourceWidth - width);
			const unsigned int sourceTop = RandomI::random(0u, sourceHeight - height);

			allSucceeded = CV::FrameBlender::blend<tTransparentIs0xFF, CV::FrameBlender::ATM_CONSTANT>(sourceFrameWithAlpha, targetFrame, sourceLeft, sourceTop, targetLeft, targetTop, width, height, useWorker ? &worker : nullptr) && allSucceeded;

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				allSucceeded = false;
				break;
			}

			if (!validateBlendResult<tTransparentIs0xFF>(sourceFrameWithAlpha, targetFrameCopy, targetFrame, sourceLeft, sourceTop, targetLeft, targetTop, width, height))
			{
				allSucceeded = false;
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

template <bool tTransparentIs0xFF>
bool TestFrameBlender::testFullFrame(const FrameType::PixelFormat sourcePixelFormat, const FrameType::PixelFormat targetPixelFormat, const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << width << "x" << height << " with " << FrameType::translatePixelFormat(sourcePixelFormat) << " -> " << FrameType::translatePixelFormat(targetPixelFormat);

	RandomGenerator randomGenerator;

	ocean_assert(FrameType::formatHasAlphaChannel(sourcePixelFormat));

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int widthToUse = performanceIteration ? width : RandomI::random(randomGenerator, 1u, width);
				const unsigned int heightToUse = performanceIteration ? height : RandomI::random(randomGenerator, 1u, height);

				const Frame sourceFrameWithAlpha = CV::CVUtilities::randomizedFrame(FrameType(widthToUse, heightToUse, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(widthToUse, heightToUse, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame targetFrameCopy(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					if (!CV::FrameBlender::blend<tTransparentIs0xFF, CV::FrameBlender::ATM_CONSTANT>(sourceFrameWithAlpha, targetFrame, useWorker))
					{
						allSucceeded = false;
					}
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameCopy))
				{
					ocean_assert(false && "Invalid padding memory!");
					allSucceeded = false;
					break;
				}

				if (!validateBlendResult<tTransparentIs0xFF>(sourceFrameWithAlpha, targetFrameCopy, targetFrame, 0u, 0u, 0u, 0u, widthToUse, heightToUse))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <bool tTransparentIs0xFF>
bool TestFrameBlender::validateBlendResult(const Frame& sourceWithAlpha, const Frame& target, const Frame& blendResult, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height)
{
	ocean_assert(sourceWithAlpha.isValid() && target.isValid() && blendResult.isValid());

	if (target.frameType() != blendResult.frameType())
	{
		return false;
	}

	bool isLastChannel = false;
	if (!FrameType::formatHasAlphaChannel(sourceWithAlpha.pixelFormat(), &isLastChannel))
	{
		return false;
	}

	const unsigned int sourceAlphaChannelIndex = isLastChannel ? sourceWithAlpha.channels() - 1u : 0u;

	unsigned int targetAlphaChannelIndex = (unsigned int)(-1);

	isLastChannel = false;
	if (FrameType::formatHasAlphaChannel(target.pixelFormat(), &isLastChannel))
	{
		targetAlphaChannelIndex = isLastChannel ? target.channels() - 1u : 0u;
	}

	const unsigned int sourceColorChannelOffset = sourceAlphaChannelIndex + 1u;
	const unsigned int targetColorChannelOffset = targetAlphaChannelIndex != (unsigned int)(-1) ? targetAlphaChannelIndex + 1u : 0u;

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		for (unsigned int x = 0u; x < target.width(); ++x)
		{
			const uint8_t* const targetPixel = target.constpixel<uint8_t>(x, y);
			const uint8_t* const resultPixel = blendResult.constpixel<uint8_t>(x, y);

			if (x < targetLeft || x >= targetLeft + width || y < targetTop || y >= targetTop + height)
			{
				// in this area nothing changed

				for (unsigned int n = 0u; n < target.channels(); ++n)
				{
					if (resultPixel[n] != targetPixel[n])
					{
						return false;
					}
				}
			}
			else
			{
				const unsigned int tx = x - targetLeft;
				const unsigned int ty = y - targetTop;
				ocean_assert(tx < width && ty < height);

				const unsigned int sx = sourceLeft + tx;
				const unsigned int sy = sourceTop + ty;
				ocean_assert(sx < sourceWithAlpha.width() && sy <= sourceWithAlpha.height());

				const uint8_t* const sourcePixel = sourceWithAlpha.constpixel<uint8_t>(sx, sy);

				for (unsigned int n = 0u; n < sourceWithAlpha.channels() - 1u; ++n)
				{
					const unsigned int sourceColorChannelIndex = (sourceColorChannelOffset + n) % sourceWithAlpha.channels();
					const unsigned int targetColorChannelIndex = (targetColorChannelOffset + n) % target.channels();

					const uint8_t alpha = tTransparentIs0xFF ? 0xFF - sourcePixel[sourceAlphaChannelIndex] : sourcePixel[sourceAlphaChannelIndex];

					const unsigned int value = (sourcePixel[sourceColorChannelIndex] * alpha + targetPixel[targetColorChannelIndex] * (0xFF - alpha) + 127u) / 255u;
					ocean_assert(value <= 255u);

					if (resultPixel[targetColorChannelIndex] != uint8_t(value))
					{
						return false;
					}
				}

				if (targetAlphaChannelIndex != (unsigned int)(-1))
				{
					if (targetPixel[targetAlphaChannelIndex] != resultPixel[targetAlphaChannelIndex])
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

}

}

}
