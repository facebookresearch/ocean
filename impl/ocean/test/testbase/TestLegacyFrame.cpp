// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testbase/TestLegacyFrame.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestLegacyFrame::test(const double testDuration)
{
	Log::info() << "---   LegacyFrame test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRowPixel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSet() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSize(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testGenericPixelFormat() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testChannelNumber() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testWidthMultiple() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHeightMultiple() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNumberPlanes() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHaveIntersectingMemory(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPixelFormatUniqueness() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testArePixelFormatsCompatible(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTranslatePixelFormat() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMoveConstructorFrame(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyConstructorFrame(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame test succeeded.";
	}
	else
	{
		Log::info() << "Frame test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestLegacyFrame, Row)
{
	EXPECT_TRUE(TestLegacyFrame::testRowPixel(GTEST_TEST_DURATION));
}

TEST(TestLegacyFrame, Set)
{
	EXPECT_TRUE(TestLegacyFrame::testSet());
}

TEST(TestLegacyFrame, Size)
{
	EXPECT_TRUE(TestLegacyFrame::testSize(GTEST_TEST_DURATION));
}

TEST(TestLegacyFrame, GenericPixelFormat)
{
	EXPECT_TRUE(TestLegacyFrame::testGenericPixelFormat());
}

TEST(TestLegacyFrame, ChannelNumber)
{
	EXPECT_TRUE(TestLegacyFrame::testChannelNumber());
}

TEST(TestLegacyFrame, WidthMultiple)
{
	EXPECT_TRUE(TestLegacyFrame::testWidthMultiple());
}

TEST(TestLegacyFrame, HeightMultiple)
{
	EXPECT_TRUE(TestLegacyFrame::testHeightMultiple());
}

TEST(TestLegacyFrame, NumberPlanes)
{
	EXPECT_TRUE(TestLegacyFrame::testNumberPlanes());
}

TEST(TestLegacyFrame, HaveIntersectingMemory)
{
	EXPECT_TRUE(TestLegacyFrame::testHaveIntersectingMemory(GTEST_TEST_DURATION));
}

TEST(TestLegacyFrame, PixelFormatUniqueness)
{
	EXPECT_TRUE(TestLegacyFrame::testPixelFormatUniqueness());
}

TEST(TestLegacyFrame, ArePixelFormatsCompatible)
{
	EXPECT_TRUE(TestLegacyFrame::testArePixelFormatsCompatible(GTEST_TEST_DURATION));
}

TEST(TestLegacyFrame, TranslatePixelFormat)
{
	EXPECT_TRUE(TestLegacyFrame::testTranslatePixelFormat());
}

TEST(TestLegacyFrame, MoveConstructorFrame)
{
	EXPECT_TRUE(TestLegacyFrame::testMoveConstructorFrame(GTEST_TEST_DURATION));
}

TEST(TestLegacyFrame, CopyConstructorFrame)
{
	EXPECT_TRUE(TestLegacyFrame::testCopyConstructorFrame(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestLegacyFrame::testRowPixel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing the pixel() and row() data access function:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 100u);
		const unsigned int height = RandomI::random(1u, 100u);

		std::array<LegacyFrame, 4u> framesUC =
		{{
			LegacyFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT))
		}};

		std::array<LegacyFrame, 4u> framesF =
		{{
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 1u), FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 2u), FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 3u), FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 4u), FrameType::ORIGIN_UPPER_LEFT))
		}};

		std::array<LegacyFrame, 4u> framesLL =
		{{
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_INTEGER_64, 1u), FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_INTEGER_64, 2u), FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_INTEGER_64, 3u), FrameType::ORIGIN_UPPER_LEFT)),
			LegacyFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_INTEGER_64, 4u), FrameType::ORIGIN_UPPER_LEFT))
		}};

		const unsigned int pixels = width * height;

		// 1 channel frames:
		for (unsigned int n = 0u; n < pixels * 1u; ++n)
		{
			framesUC[0].data<uint8_t>()[n] = (uint8_t)(n);
			framesF[0].data<float>()[n] = float(n);
			framesLL[0].data<long long>()[n] = (long long)(n);
		}

		// 2 channel frames:
		for (unsigned int n = 0u; n < pixels * 2u; ++n)
		{
			framesUC[1].data<uint8_t>()[n] = (uint8_t)(n);
			framesF[1].data<float>()[n] = float(n);
			framesLL[1].data<long long>()[n] = (long long)(n);
		}

		// 3 channel frames:
		for (unsigned int n = 0u; n < pixels * 3u; ++n)
		{
			framesUC[2].data<uint8_t>()[n] = (uint8_t)(n);
			framesF[2].data<float>()[n] = float(n);
			framesLL[2].data<long long>()[n] = (long long)(n);
		}

		// 4 channel frames:
		for (unsigned int n = 0u; n < pixels * 4u; ++n)
		{
			framesUC[3].data<uint8_t>()[n] = (uint8_t)(n);
			framesF[3].data<float>()[n] = float(n);
			framesLL[3].data<long long>()[n] = (long long)(n);
		}

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int y = 0u; y < height; ++y)
			{
				const unsigned int pixelIndex = y * width + x;

				// frames with unsigned char as data type
				for (unsigned int channels = 1u; channels <= 4u; ++channels)
				{
					LegacyFrame& frame = framesUC[channels - 1u];

					unsigned char* const frameRow = frame.row(y);
					const unsigned char* const frameRowConst = frame.constrow(y);

					unsigned char* const framePixel = frame.pixel(x, y);
					const unsigned char* const framePixelConst = frame.constpixel(x, y);

					if (frameRow != frameRowConst || framePixel != framePixelConst)
					{
						allSucceeded = false;
					}

					if (frameRow != frame.data<uint8_t>() + frame.width() * y * channels)
					{
						allSucceeded = false;
					}

					if (frameRowConst != frame.constdata<uint8_t>() + frame.width() * y * channels)
					{
						allSucceeded = false;
					}

					if (frameRow != frame.pixel(0u, y) || frameRowConst != frame.constpixel(0u, y))
					{
						allSucceeded = false;
					}

					for (unsigned int channel = 0u; channel < channels; ++channel)
					{
						const unsigned char value = (unsigned char)(pixelIndex * channels + channel);

						if (framePixel[channel] != value || framePixelConst[channel] != value)
						{
							allSucceeded = false;
						}
					}
				}

				// frames with float as data type
				for (unsigned int channels = 1u; channels <= 4u; ++channels)
				{
					LegacyFrame& frame = framesF[channels - 1u];

					float* const frameRow = frame.row<float>(y);
					const float* const frameRowConst = frame.constrow<float>(y);

					float* const framePixel = frame.pixel<float>(x, y);
					const float* const framePixelConst = frame.constpixel<float>(x, y);

					if (frameRow != frameRowConst || framePixel != framePixelConst)
					{
						allSucceeded = false;
					}

					if (frameRow != frame.data<float>() + frame.width() * y * channels)
					{
						allSucceeded = false;
					}

					if (frameRowConst != frame.constdata<float>() + frame.width() * y * channels)
					{
						allSucceeded = false;
					}

					if (frameRow != frame.pixel<float>(0u, y) || frameRowConst != frame.constpixel<float>(0u, y))
					{
						allSucceeded = false;
					}

					for (unsigned int channel = 0u; channel < channels; ++channel)
					{
						const float value = float(pixelIndex * channels + channel);

						if (framePixel[channel] != value || framePixelConst[channel] != value)
						{
							allSucceeded = false;
						}
					}
				}

				// frames with long long as data type
				for (unsigned int channels = 1u; channels <= 4u; ++channels)
				{
					LegacyFrame& frame = framesLL[channels - 1u];

					long long* const frameRow = frame.row<long long>(y);
					const long long* const frameRowConst = frame.constrow<long long>(y);

					long long* const framePixel = frame.pixel<long long>(x, y);
					const long long* const framePixelConst = frame.constpixel<long long>(x, y);

					if (frameRow != frameRowConst || framePixel != framePixelConst)
					{
						allSucceeded = false;
					}

					if (frameRow != frame.data<long long>() + frame.width() * y * channels)
					{
						allSucceeded = false;
					}

					if (frameRowConst != frame.constdata<long long>() + frame.width() * y * channels)
					{
						allSucceeded = false;
					}

					if (frameRow != frame.pixel<long long>(0u, y) || frameRowConst != frame.constpixel<long long>(0u, y))
					{
						allSucceeded = false;
					}

					for (unsigned int channel = 0u; channel < channels; ++channel)
					{
						const long long value = (long long)(pixelIndex * channels + channel);

						if (framePixel[channel] != value || framePixelConst[channel] != value)
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

bool TestLegacyFrame::testSet()
{
	bool allSucceeded = true;

	LegacyFrame invalidFrame;
	allSucceeded = validateFrame(invalidFrame, false, false, false, Timestamp(), 0) && allSucceeded;

	const FrameType frameType = FrameType(160, 120, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);
	const FrameType smallerFrameType = FrameType(160, 120, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
	const FrameType largerFrameType = FrameType(160, 120, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT);

	const unsigned int frameSize = 160u * 120u * 3u;
	const unsigned int smallerFrameSize = 160u * 120u * 1u;
	const unsigned int largerFrameSize = 160u * 120u * 4u;
	const Timestamp frameTimestamp(true);

	LegacyFrame originalFrame(frameType, frameTimestamp);
	allSucceeded = validateFrame(originalFrame, true, true, false, frameTimestamp, frameSize) && allSucceeded;

	LegacyFrame writableFrame(originalFrame, frameTimestamp, originalFrame.data<uint8_t>(), false);
	allSucceeded = validateFrame(originalFrame, true, true, false, frameTimestamp, frameSize) && allSucceeded;

	LegacyFrame readOnlyFrame(originalFrame, frameTimestamp, originalFrame.constdata<uint8_t>(), false);
	allSucceeded = validateFrame(readOnlyFrame, true, false, true, frameTimestamp, frameSize) && allSucceeded;

	LegacyFrame copiedFrame(originalFrame, frameTimestamp, originalFrame.data<uint8_t>(), true);
	allSucceeded = validateFrame(copiedFrame, true, true, false, frameTimestamp, frameSize) && allSucceeded;

	Log::info() << "Testing frame set with simple objections";

	// Valid frame type, invalid frame:
	allSucceeded = testSet(LegacyFrame(), frameType, true, true, false, Timestamp(), frameSize) && allSucceeded;

	/* Same frame type */

	// Same frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, frameType, true, true, false, frameTimestamp, frameSize) && allSucceeded;

	// Same frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, frameType, true, false, false, frameTimestamp, frameSize) && allSucceeded;

	// Same frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, frameType, true, false, true , frameTimestamp, frameSize) && allSucceeded;

	/* Smaller frame type */

	// Smaller frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, smallerFrameType, true, true, false, Timestamp(), smallerFrameSize) && allSucceeded;

	// Smaller frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, smallerFrameType, true, false, false, Timestamp(), smallerFrameSize) && allSucceeded;

	// Smaller frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, true, false, true, Timestamp(), smallerFrameSize) && allSucceeded;

	/* Larger frame type */

	// Larger frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, largerFrameType, false, true, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;

	// Larger frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, largerFrameType, false, true, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;

	// Larger frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, largerFrameType, false, true, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;

	/* Invalid frame type */

	// Invalid frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, FrameType(), false, false, false, Timestamp(), 0) && allSucceeded;

	// Invalid frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, FrameType(), false, false, false, Timestamp(), 0) && allSucceeded;

	// Invalid frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, FrameType(), false, false, false, Timestamp(), 0) && allSucceeded;

	Log::info() << "Testing frame set with force options";

	// Valid frame type, invalid frame:
	allSucceeded = testSet(LegacyFrame(), frameType, false, false, true, true, false, Timestamp(), frameSize) && allSucceeded;
	allSucceeded = testSet(LegacyFrame(), frameType, false, true , true, true, false, Timestamp(), frameSize) && allSucceeded;
	allSucceeded = testSet(LegacyFrame(), frameType, true , false, true, true, false, Timestamp(), frameSize) && allSucceeded;
	allSucceeded = testSet(LegacyFrame(), frameType, true , true , true, true, false, Timestamp(), frameSize) && allSucceeded;

	/* Same frame type */

	// Same frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, frameType, false, false, true, true, false, frameTimestamp, frameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, frameType, false, true , true, true, false, frameTimestamp, frameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, frameType, true , false, true, true, false, frameTimestamp, frameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, frameType, true , true , true, true, false, frameTimestamp, frameSize) && allSucceeded;

	// Same frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, frameType, false, false, true, false, false, frameTimestamp, frameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, frameType, false, true , true, false, false, frameTimestamp, frameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, frameType, true , false, true, true , false, Timestamp(), frameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, frameType, true , true , true, true , false, Timestamp(), frameSize) && allSucceeded;

	// Same frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, frameType, false, false, true, false, true , frameTimestamp, frameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, frameType, false, true , true, true , false, Timestamp(), frameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, frameType, true , false, true, true , false, Timestamp(), frameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, frameType, true , true , true, true , false, Timestamp(), frameSize) && allSucceeded;

	/* Smaller frame type */

	// Smaller frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, smallerFrameType, false, false, true, true, false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, smallerFrameType, false, true , true, true, false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, smallerFrameType, true , false, true, true, false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, smallerFrameType, true , true , true, true, false, Timestamp(), smallerFrameSize) && allSucceeded;

	// Smaller frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, smallerFrameType, false, false, true, false, false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, smallerFrameType, false, true , true, false, false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, smallerFrameType, true , false, true, true , false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, smallerFrameType, true , true , true, true , false, Timestamp(), smallerFrameSize) && allSucceeded;

	// Smaller frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, false, false, true, false, true , Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, false, true , true, true , false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, true , false, true, true , false, Timestamp(), smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, true , true , true, true , false, Timestamp(), smallerFrameSize) && allSucceeded;

	/* Larger frame type */

	// Larger frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, largerFrameType, false, false, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, largerFrameType, false, true , true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, largerFrameType, true , false, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, largerFrameType, true , true , true, true, false, Timestamp(), largerFrameSize) && allSucceeded;

	// Larger frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, largerFrameType, false, false, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, largerFrameType, false, true , true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, largerFrameType, true , false, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, largerFrameType, true , true , true, true, false, Timestamp(), largerFrameSize) && allSucceeded;

	// Larger frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, largerFrameType, false, false, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, largerFrameType, false, true , true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, largerFrameType, true , false, true, true, false, Timestamp(), largerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, largerFrameType, true , true , true, true, false, Timestamp(), largerFrameSize) && allSucceeded;

	/* Invalid frame type */

	// Invalid frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, FrameType(), false, false, false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(copiedFrame, FrameType(), false, true , false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(copiedFrame, FrameType(), true , false, false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(copiedFrame, FrameType(), true , true , false, false, false, Timestamp(), 0) && allSucceeded;

	// Invalid frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, FrameType(), false, false, false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(writableFrame, FrameType(), false, true , false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(writableFrame, FrameType(), true , false, false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(writableFrame, FrameType(), true , true , false, false, false, Timestamp(), 0) && allSucceeded;

	// Invalid frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, FrameType(), false, false, false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, FrameType(), false, true , false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, FrameType(), true , false, false, false, false, Timestamp(), 0) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, FrameType(), true , true , false, false, false, Timestamp(), 0) && allSucceeded;

	Log::info() << "Testing frame set with frame data initialization";

	LegacyFrame dataFrame(largerFrameType);
	unsigned char* data = dataFrame.data<uint8_t>();
	const unsigned char* const dataEnd = data + dataFrame.size();

	while (data < dataEnd)
	{
		*data++ = (unsigned char)(RandomI::random(0xFFu));
	}

	// Valid frame type, invalid frame:
	allSucceeded = testSet(LegacyFrame(), frameType, frameTimestamp, dataFrame, false, false, true, false, false, frameSize) && allSucceeded;
	allSucceeded = testSet(LegacyFrame(), frameType, frameTimestamp, dataFrame, false, true , true, true , false, frameSize) && allSucceeded;
	allSucceeded = testSet(LegacyFrame(), frameType, frameTimestamp, dataFrame, true , false, true, false, true , frameSize) && allSucceeded;
	allSucceeded = testSet(LegacyFrame(), frameType, frameTimestamp, dataFrame, true , true , true, true , false, frameSize) && allSucceeded;

	/* Same frame type */

	// Same frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, frameType, frameTimestamp, dataFrame, false, false, true, false, false, frameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, frameType, frameTimestamp, dataFrame, false, true , true, true , false, frameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, frameType, frameTimestamp, dataFrame, true , false, true, false, true , frameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, frameType, frameTimestamp, dataFrame, true , true , true, true , false, frameSize) && allSucceeded;

	// Same frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, frameType, frameTimestamp, dataFrame, false, false, true, false, false, frameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, frameType, frameTimestamp, dataFrame, false, true , true, false, false, frameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, frameType, frameTimestamp, dataFrame, true , false, true, false, true , frameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, frameType, frameTimestamp, dataFrame, true , true , true, false, false, frameSize) && allSucceeded;

	// Same frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, frameType, frameTimestamp, dataFrame, false, false, true, false, false, frameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, frameType, frameTimestamp, dataFrame, false, true , true, true , false, frameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, frameType, frameTimestamp, dataFrame, true , false, true, false, true , frameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, frameType, frameTimestamp, dataFrame, true , true , true, true , false, frameSize) && allSucceeded;

	/* Smaller frame type */

	// Smaller frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, smallerFrameType, frameTimestamp, dataFrame, false, false, true, false, false, smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, smallerFrameType, frameTimestamp, dataFrame, false, true , true, true , false, smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, smallerFrameType, frameTimestamp, dataFrame, true , false, true, false, true , smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, smallerFrameType, frameTimestamp, dataFrame, true , true , true, true , false, smallerFrameSize) && allSucceeded;

	// Smaller frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, smallerFrameType, frameTimestamp, dataFrame, false, false, true, false, false, smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, smallerFrameType, frameTimestamp, dataFrame, false, true , true, false, false, smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, smallerFrameType, frameTimestamp, dataFrame, true , false, true, false, true , smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, smallerFrameType, frameTimestamp, dataFrame, true , true , true, false , false, smallerFrameSize) && allSucceeded;

	// Smaller frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, frameTimestamp, dataFrame, false, false, true, false, false, smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, frameTimestamp, dataFrame, false, true , true, true, false, smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, frameTimestamp, dataFrame, true , false, true, false, true , smallerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, smallerFrameType, frameTimestamp, dataFrame, true , true , true, true, false, smallerFrameSize) && allSucceeded;

	/* Larger frame type */

	// Larger frame type, writable frame owning its data:
	allSucceeded = testSet(copiedFrame, largerFrameType, frameTimestamp, dataFrame, false, false, true, false, false, largerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, largerFrameType, frameTimestamp, dataFrame, false, true , true, true , false, largerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, largerFrameType, frameTimestamp, dataFrame, true , false, true, false, true , largerFrameSize) && allSucceeded;
	allSucceeded = testSet(copiedFrame, largerFrameType, frameTimestamp, dataFrame, true , true , true, true , false, largerFrameSize) && allSucceeded;

	// Larger frame type, writable frame not owning its data:
	allSucceeded = testSet(writableFrame, largerFrameType, frameTimestamp, dataFrame, false, false, true, false, false, largerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, largerFrameType, frameTimestamp, dataFrame, false, true , true, true , false, largerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, largerFrameType, frameTimestamp, dataFrame, true , false, true, false, true , largerFrameSize) && allSucceeded;
	allSucceeded = testSet(writableFrame, largerFrameType, frameTimestamp, dataFrame, true , true , true, true , false, largerFrameSize) && allSucceeded;

	// Larger frame type, read-only frame not owning its data:
	allSucceeded = testSet(readOnlyFrame, largerFrameType, frameTimestamp, dataFrame, false, false, true, false, false, largerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, largerFrameType, frameTimestamp, dataFrame, false, true , true, true , false, largerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, largerFrameType, frameTimestamp, dataFrame, true , false, true, false, true , largerFrameSize) && allSucceeded;
	allSucceeded = testSet(readOnlyFrame, largerFrameType, frameTimestamp, dataFrame, true , true , true, true , false, largerFrameSize) && allSucceeded;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestLegacyFrame::testSize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Size test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1920u);
		const unsigned int pixels = width * height;

		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		{
			// for pixel format Y8

			const FrameType frameType(width, height, FrameType::FORMAT_Y8, pixelOrigin);
			const LegacyFrame frame(frameType);

			const unsigned int channels = 1u;
			const unsigned int bytesPerElement = 1u;

			const unsigned int bytes = pixels * channels * bytesPerElement;

			if (frameType.frameTypeSize() != bytes)
			{
				allSucceeded = false;
			}

			if (frame.size() != bytes)
			{
				allSucceeded = false;
			}
		}

		{
			// for pixel format Y10

			const FrameType frameType(width, height, FrameType::FORMAT_Y10, pixelOrigin);
			const LegacyFrame frame(frameType);

			const unsigned int channels = 1u;
			const unsigned int bytesPerElement = 2u;

			const unsigned int bytes = pixels * channels * bytesPerElement;

			if (frameType.frameTypeSize() != bytes)
			{
				allSucceeded = false;
			}

			if (frame.size() != bytes)
			{
				allSucceeded = false;
			}
		}

		{
			// for pixel format RGB24

			const FrameType frameType(width, height, FrameType::FORMAT_RGB24, pixelOrigin);
			const LegacyFrame frame(frameType);

			const unsigned int channels = 3u;
			const unsigned int bytesPerElement = 1u;

			const unsigned int bytes = pixels * channels * bytesPerElement;

			if (frameType.frameTypeSize() != bytes)
			{
				allSucceeded = false;
			}

			if (frame.size() != bytes)
			{
				allSucceeded = false;
			}
		}

		{
			// for pixel format 5 channels, 32 bit float

			const FrameType frameType(width, height, FrameType::genericPixelFormat<float, 5u>(), pixelOrigin);
			const LegacyFrame frame(frameType);

			const unsigned int channels = 5u;
			const unsigned int bytesPerElement = 4u;

			const unsigned int bytes = pixels * channels * bytesPerElement;

			if (frameType.frameTypeSize() != bytes)
			{
				allSucceeded = false;
			}

			if (frame.size() != bytes)
			{
				allSucceeded = false;
			}
		}

		{
			// for pixel format Y32

			const FrameType frameType(width, height, FrameType::FORMAT_Y32, pixelOrigin);
			const LegacyFrame frame(frameType);

			const unsigned int channels = 1u;
			const unsigned int bytesPerElement = 4u;

			const unsigned int bytes = pixels * channels * bytesPerElement;

			if (frameType.frameTypeSize() != bytes)
			{
				allSucceeded = false;
			}

			if (frame.size() != bytes)
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

bool TestLegacyFrame::testGenericPixelFormat()
{
	Log::info() << "Generic pixel format test:";

	bool allSucceeded = true;

	if (FrameType::dataType<unsigned char>() != FrameType::DT_UNSIGNED_INTEGER_8 || FrameType::dataType<uint8_t>() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<signed char>() != FrameType::DT_SIGNED_INTEGER_8 || FrameType::dataType<int8_t>() != FrameType::DT_SIGNED_INTEGER_8)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<unsigned short>() != FrameType::DT_UNSIGNED_INTEGER_16 || FrameType::dataType<uint16_t>() != FrameType::DT_UNSIGNED_INTEGER_16)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<short>() != FrameType::DT_SIGNED_INTEGER_16 || FrameType::dataType<int16_t>() != FrameType::DT_SIGNED_INTEGER_16)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<unsigned int>() != FrameType::DT_UNSIGNED_INTEGER_32 || FrameType::dataType<uint32_t>() != FrameType::DT_UNSIGNED_INTEGER_32)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<int>() != FrameType::DT_SIGNED_INTEGER_32 || FrameType::dataType<int32_t>() != FrameType::DT_SIGNED_INTEGER_32)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<unsigned long long>() != FrameType::DT_UNSIGNED_INTEGER_64 || FrameType::dataType<uint64_t>() != FrameType::DT_UNSIGNED_INTEGER_64)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<long long>() != FrameType::DT_SIGNED_INTEGER_64 || FrameType::dataType<int64_t>() != FrameType::DT_SIGNED_INTEGER_64)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<float>() != FrameType::DT_SIGNED_FLOAT_32)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<double>() != FrameType::DT_SIGNED_FLOAT_64)
	{
		allSucceeded = false;
	}

	if (FrameType::dataType<std::string>() != FrameType::DT_UNDEFINED)
	{
		allSucceeded = false;
	}

	if (FrameType::formatIsPureGeneric(FrameType::FORMAT_Y8)
			|| FrameType::formatIsPureGeneric(FrameType::FORMAT_Y10)
			|| FrameType::formatIsPureGeneric(FrameType::FORMAT_Y32)
			|| FrameType::formatIsPureGeneric(FrameType::FORMAT_RGB24)
			|| FrameType::formatIsPureGeneric(FrameType::FORMAT_BGRA32)
			|| FrameType::formatIsPureGeneric(FrameType::FORMAT_Y_U_V12))
	{
		allSucceeded = false;
	}

	if (FrameType::formatIsPureGeneric(FrameType::genericPixelFormat<float, 3u>()) == false
			|| FrameType::formatIsPureGeneric(FrameType::genericPixelFormat<FrameType::DT_SIGNED_INTEGER_8, 2u>()) == false
			|| FrameType::formatIsPureGeneric(FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_16, 4u)) == false
			|| FrameType::formatIsPureGeneric(FrameType::PixelFormat(FrameType::GenericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value)) == false)
	{
		allSucceeded = false;
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

bool TestLegacyFrame::testChannelNumber()
{
	Log::info() << "Channel number test:";

	bool allSucceeded = true;

	typedef std::pair<FrameType::PixelFormat, uint32_t> TestPair;
	typedef std::vector<TestPair> TestPairs;

	const TestPairs testPairs =
	{
		TestPair(FrameType::FORMAT_ABGR32, 4u),
		TestPair(FrameType::FORMAT_ARGB32, 4u),
		TestPair(FrameType::FORMAT_BGR24, 3u),
		TestPair(FrameType::FORMAT_BGR32, 4u), // although BGR32 has three usable channels only, it is defined to be a 4-channel pixel format
		TestPair(FrameType::FORMAT_BGR4444, 4u), // although FORMAT_BGR4444 has three usable channels only, it is defined to be a 4-channel pixel format
		TestPair(FrameType::FORMAT_BGR5551, 3u),
		TestPair(FrameType::FORMAT_BGR565, 3u),
		TestPair(FrameType::FORMAT_BGRA32, 4u),
		TestPair(FrameType::FORMAT_BGRA4444, 4u),
		TestPair(FrameType::FORMAT_BGGR10_PACKED, 3u),
		TestPair(FrameType::FORMAT_RGB24, 3u),
		TestPair(FrameType::FORMAT_RGB32, 4u), // although RGB32 has three usable channels only, it is defined to be a 4-channel pixel format
		TestPair(FrameType::FORMAT_RGB4444, 4u), // although FORMAT_RGB4444 has three usable channels only, it is defined to be a 4-channel pixel format
		TestPair(FrameType::FORMAT_RGB5551, 3u),
		TestPair(FrameType::FORMAT_RGB565, 3u),
		TestPair(FrameType::FORMAT_RGBA32, 4u),
		TestPair(FrameType::FORMAT_RGBA4444, 4u),
		TestPair(FrameType::FORMAT_RGBT32, 4u),
		TestPair(FrameType::FORMAT_RGGB10_PACKED, 3u),
		TestPair(FrameType::FORMAT_YUV24, 3u),
		TestPair(FrameType::FORMAT_YUVA32, 4u),
		TestPair(FrameType::FORMAT_YUVT32, 4u),
		TestPair(FrameType::FORMAT_YVU24, 3u),
		TestPair(FrameType::FORMAT_UYVY16, 3u),
		TestPair(FrameType::FORMAT_YUYV16, 3u),
		TestPair(FrameType::FORMAT_Y16, 1u),
		TestPair(FrameType::FORMAT_Y32, 1u),
		TestPair(FrameType::FORMAT_Y64, 1u),
		TestPair(FrameType::FORMAT_YA16, 2u),
		TestPair(FrameType::FORMAT_RGB48, 3u),
		TestPair(FrameType::FORMAT_RGBA64, 4u),
		TestPair(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_U_V24_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y8_LIMITED_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y8_FULL_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y10, 1u),
		TestPair(FrameType::FORMAT_Y10_PACKED, 1u),
		TestPair(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_UV12_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_VU12_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_U_V12_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_V_U12_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_F32, 1u),
		TestPair(FrameType::FORMAT_F64, 1u),
	};

	// ensuring that we have covered all pixel formats

	if (testPairs.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	for (const TestPair& testPair : testPairs)
	{
		const FrameType::PixelFormat& pixelFormat = testPair.first;

		if (FrameType::channels(pixelFormat) != testPair.second)
		{
			allSucceeded = false;
		}

		const unsigned int widthMultiples = FrameType::widthMultiple(pixelFormat);
		const unsigned int heightMultiples = FrameType::heightMultiple(pixelFormat);

		const FrameType frameType(RandomI::random(1u, 1920u) * widthMultiples, RandomI::random(1u, 1080u) * heightMultiples, pixelFormat, RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT}));

		if (frameType.channels() != testPair.second)
		{
			allSucceeded = false;
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

bool TestLegacyFrame::testWidthMultiple()
{
	Log::info() << "Width multiple test:";

	bool allSucceeded = true;

	typedef std::pair<FrameType::PixelFormat, uint32_t> TestPair;
	typedef std::vector<TestPair> TestPairs;

	const TestPairs testPairs =
	{
		TestPair(FrameType::FORMAT_ABGR32, 1u),
		TestPair(FrameType::FORMAT_ARGB32, 1u),
		TestPair(FrameType::FORMAT_BGR24, 1u),
		TestPair(FrameType::FORMAT_BGR32, 1u),
		TestPair(FrameType::FORMAT_BGR4444, 1u),
		TestPair(FrameType::FORMAT_BGR5551, 1u),
		TestPair(FrameType::FORMAT_BGR565, 1u),
		TestPair(FrameType::FORMAT_BGRA32, 1u),
		TestPair(FrameType::FORMAT_BGRA4444, 1u),
		TestPair(FrameType::FORMAT_BGGR10_PACKED, 4u),
		TestPair(FrameType::FORMAT_RGB24, 1u),
		TestPair(FrameType::FORMAT_RGB32, 1u),
		TestPair(FrameType::FORMAT_RGB4444, 1u),
		TestPair(FrameType::FORMAT_RGB5551, 1u),
		TestPair(FrameType::FORMAT_RGB565, 1u),
		TestPair(FrameType::FORMAT_RGBA32, 1u),
		TestPair(FrameType::FORMAT_RGBA4444, 1u),
		TestPair(FrameType::FORMAT_RGBT32, 1u),
		TestPair(FrameType::FORMAT_RGGB10_PACKED, 4u),
		TestPair(FrameType::FORMAT_YUV24, 1u),
		TestPair(FrameType::FORMAT_YUVA32, 1u),
		TestPair(FrameType::FORMAT_YUVT32, 1u),
		TestPair(FrameType::FORMAT_YVU24, 1u),
		TestPair(FrameType::FORMAT_UYVY16, 2u),
		TestPair(FrameType::FORMAT_YUYV16, 2u),
		TestPair(FrameType::FORMAT_Y16, 1u),
		TestPair(FrameType::FORMAT_Y32, 1u),
		TestPair(FrameType::FORMAT_Y64, 1u),
		TestPair(FrameType::FORMAT_YA16, 1u),
		TestPair(FrameType::FORMAT_RGB48, 1u),
		TestPair(FrameType::FORMAT_RGBA64, 1u),
		TestPair(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y_U_V24_FULL_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y8_LIMITED_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y8_FULL_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y10, 1u),
		TestPair(FrameType::FORMAT_Y10_PACKED, 4u),
		TestPair(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_UV12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_VU12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_U_V12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_V_U12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_F32, 1u),
		TestPair(FrameType::FORMAT_F64, 1u),
	};

	// ensuring that we have covered all pixel formats

	if (testPairs.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	for (const TestPair& testPair : testPairs)
	{
		if (FrameType::widthMultiple(testPair.first) != testPair.second)
		{
			allSucceeded = false;
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

bool TestLegacyFrame::testHeightMultiple()
{
	Log::info() << "Height multiple test:";

	bool allSucceeded = true;

	typedef std::pair<FrameType::PixelFormat, uint32_t> TestPair;
	typedef std::vector<TestPair> TestPairs;

	const TestPairs testPairs =
	{
		TestPair(FrameType::FORMAT_ABGR32, 1u),
		TestPair(FrameType::FORMAT_ARGB32, 1u),
		TestPair(FrameType::FORMAT_BGR24, 1u),
		TestPair(FrameType::FORMAT_BGR32, 1u),
		TestPair(FrameType::FORMAT_BGR4444, 1u),
		TestPair(FrameType::FORMAT_BGR5551, 1u),
		TestPair(FrameType::FORMAT_BGR565, 1u),
		TestPair(FrameType::FORMAT_BGRA32, 1u),
		TestPair(FrameType::FORMAT_BGRA4444, 1u),
		TestPair(FrameType::FORMAT_BGGR10_PACKED, 2u),
		TestPair(FrameType::FORMAT_RGB24, 1u),
		TestPair(FrameType::FORMAT_RGB32, 1u),
		TestPair(FrameType::FORMAT_RGB4444, 1u),
		TestPair(FrameType::FORMAT_RGB5551, 1u),
		TestPair(FrameType::FORMAT_RGB565, 1u),
		TestPair(FrameType::FORMAT_RGBA32, 1u),
		TestPair(FrameType::FORMAT_RGBA4444, 1u),
		TestPair(FrameType::FORMAT_RGBT32, 1u),
		TestPair(FrameType::FORMAT_RGGB10_PACKED, 2u),
		TestPair(FrameType::FORMAT_YUV24, 1u),
		TestPair(FrameType::FORMAT_YUVA32, 1u),
		TestPair(FrameType::FORMAT_YUVT32, 1u),
		TestPair(FrameType::FORMAT_YVU24, 1u),
		TestPair(FrameType::FORMAT_UYVY16, 1u),
		TestPair(FrameType::FORMAT_YUYV16, 1u),
		TestPair(FrameType::FORMAT_Y16, 1u),
		TestPair(FrameType::FORMAT_Y32, 1u),
		TestPair(FrameType::FORMAT_Y64, 1u),
		TestPair(FrameType::FORMAT_YA16, 1u),
		TestPair(FrameType::FORMAT_RGB48, 1u),
		TestPair(FrameType::FORMAT_RGBA64, 1u),
		TestPair(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y_U_V24_FULL_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y8_LIMITED_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y8_FULL_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y10, 1u),
		TestPair(FrameType::FORMAT_Y10_PACKED, 1u),
		TestPair(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_UV12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_VU12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_U_V12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_V_U12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_F32, 1u),
		TestPair(FrameType::FORMAT_F64, 1u)
	};

	// ensuring that we have covered all pixel formats

	if (testPairs.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	for (const TestPair& testPair : testPairs)
	{
		if (FrameType::heightMultiple(testPair.first) != testPair.second)
		{
			allSucceeded = false;
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

bool TestLegacyFrame::testNumberPlanes()
{
	Log::info() << "Number planes test:";

	bool allSucceeded = true;

	typedef std::pair<FrameType::PixelFormat, uint32_t> TestPair;
	typedef std::vector<TestPair> TestPairs;

	const TestPairs testPairs =
	{
		TestPair(FrameType::FORMAT_ABGR32, 1u),
		TestPair(FrameType::FORMAT_ARGB32, 1u),
		TestPair(FrameType::FORMAT_BGR24, 1u),
		TestPair(FrameType::FORMAT_BGR32, 1u),
		TestPair(FrameType::FORMAT_BGR4444, 1u),
		TestPair(FrameType::FORMAT_BGR5551, 1u),
		TestPair(FrameType::FORMAT_BGR565, 1u),
		TestPair(FrameType::FORMAT_BGRA32, 1u),
		TestPair(FrameType::FORMAT_BGRA4444, 1u),
		TestPair(FrameType::FORMAT_BGGR10_PACKED, 1u),
		TestPair(FrameType::FORMAT_RGB24, 1u),
		TestPair(FrameType::FORMAT_RGB32, 1u),
		TestPair(FrameType::FORMAT_RGB4444, 1u),
		TestPair(FrameType::FORMAT_RGB5551, 1u),
		TestPair(FrameType::FORMAT_RGB565, 1u),
		TestPair(FrameType::FORMAT_RGBA32, 1u),
		TestPair(FrameType::FORMAT_RGBA4444, 1u),
		TestPair(FrameType::FORMAT_RGBT32, 1u),
		TestPair(FrameType::FORMAT_RGGB10_PACKED, 1u),
		TestPair(FrameType::FORMAT_YUV24, 1u),
		TestPair(FrameType::FORMAT_YUVA32, 1u),
		TestPair(FrameType::FORMAT_YUVT32, 1u),
		TestPair(FrameType::FORMAT_YVU24, 1u),
		TestPair(FrameType::FORMAT_UYVY16, 1u),
		TestPair(FrameType::FORMAT_YUYV16, 1u),
		TestPair(FrameType::FORMAT_Y16, 1u),
		TestPair(FrameType::FORMAT_Y32, 1u),
		TestPair(FrameType::FORMAT_Y64, 1u),
		TestPair(FrameType::FORMAT_YA16, 1u),
		TestPair(FrameType::FORMAT_RGB48, 1u),
		TestPair(FrameType::FORMAT_RGBA64, 1u),
		TestPair(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_U_V24_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y8_LIMITED_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y8_FULL_RANGE, 1u),
		TestPair(FrameType::FORMAT_Y10, 1u),
		TestPair(FrameType::FORMAT_Y10_PACKED, 1u),
		TestPair(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_UV12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_VU12_FULL_RANGE, 2u),
		TestPair(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_U_V12_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, 3u),
		TestPair(FrameType::FORMAT_Y_V_U12_FULL_RANGE, 3u),
		TestPair(FrameType::FORMAT_F32, 1u),
		TestPair(FrameType::FORMAT_F64, 1u)
	};

	// ensuring that we have covered all pixel formats

	if (testPairs.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	for (const TestPair& testPair : testPairs)
	{
		if (FrameType::numberPlanes(testPair.first) != testPair.second)
		{
			allSucceeded = false;
		}

		const unsigned int width = RandomI::random(1u, 1920u) * FrameType::widthMultiple(testPair.first);
		const unsigned int height = RandomI::random(1u, 1080u) * FrameType::heightMultiple(testPair.first);

		const FrameType frameType(width, height, testPair.first, RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT}));

		if (frameType.numberPlanes() != testPair.second)
		{
			allSucceeded = false;
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

bool TestLegacyFrame::testHaveIntersectingMemory(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersecting memory test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// test for frames each owning the memory

			const unsigned int widthA = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int widthB = RandomI::random(randomGenerator, 1u, 1920u);

			const unsigned int heightA = RandomI::random(randomGenerator, 1u, 1080u);
			const unsigned int heightB = RandomI::random(randomGenerator, 1u, 1080u);

			const FrameType::DataType dataTypeA = FrameType::DataType(RandomI::random(randomGenerator, (unsigned int)FrameType::DT_UNSIGNED_INTEGER_8, (unsigned int)FrameType::DT_SIGNED_FLOAT_64));
			const FrameType::DataType dataTypeB = FrameType::DataType(RandomI::random(randomGenerator, (unsigned int)FrameType::DT_UNSIGNED_INTEGER_8, (unsigned int)FrameType::DT_SIGNED_FLOAT_64));

			const FrameType::PixelFormat pixelFormatA = FrameType::genericPixelFormat(dataTypeA, RandomI::random(randomGenerator, 1u, 5u));
			const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(dataTypeB, RandomI::random(randomGenerator, 1u, 5u));

			const FrameType::PixelOrigin pixelOriginA = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
			const FrameType::PixelOrigin pixelOriginB = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const LegacyFrame frameA(FrameType(widthA, heightA, pixelFormatA, pixelOriginA));
			const LegacyFrame frameB(FrameType(widthB, heightB, pixelFormatB, pixelOriginB));

			// two individual frames never have intersecting memory

			if (frameA.haveIntersectingMemory(frameB))
			{
				allSucceeded = false;
			}
			if (frameB.haveIntersectingMemory(frameA))
			{
				allSucceeded = false;
			}

			// two identical frames always have intersecting memory

			if (frameA.haveIntersectingMemory(frameA) == false)
			{
				allSucceeded = false;
			}
			if (frameB.haveIntersectingMemory(frameB) == false)
			{
				allSucceeded = false;
			}
		}

		{
			// test for frames not owning the memory

			const unsigned int maximalWidth = 1920u;
			const unsigned int maximalHeight = 1080u;
			const unsigned int maximalChannels = 5;

			const unsigned int maximalFrameMemory = sizeof(unsigned char) * maximalWidth * maximalHeight * maximalChannels;

			Memory memory(size_t(maximalFrameMemory * 2u)); // we allocate twice as much memory

			const unsigned int widthA = RandomI::random(randomGenerator, 1u, maximalWidth);
			const unsigned int widthB = RandomI::random(randomGenerator, 1u, maximalWidth);

			const unsigned int heightA = RandomI::random(randomGenerator, 1u, maximalHeight);
			const unsigned int heightB = RandomI::random(randomGenerator, 1u, maximalHeight);

			const FrameType::PixelFormat pixelFormatA = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, RandomI::random(randomGenerator, 1u, maximalChannels));
			const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, RandomI::random(randomGenerator, 1u, maximalChannels));

			const FrameType::PixelOrigin pixelOriginA = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
			const FrameType::PixelOrigin pixelOriginB = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const FrameType frameTypeA(widthA, heightA, pixelFormatA, pixelOriginA);
			const FrameType frameTypeB(widthB, heightB, pixelFormatB, pixelOriginB);

			const size_t startFrameA = size_t(RandomI::random(randomGenerator, maximalFrameMemory)); // we have twice as much memory
			const size_t startFrameB = size_t(RandomI::random(randomGenerator, maximalFrameMemory)); // we have twice as much memory

			const LegacyFrame frameA(frameTypeA, (unsigned char*)memory.data() + startFrameA, false);
			const LegacyFrame frameB(frameTypeB, (unsigned char*)memory.data() + startFrameB, false);

			const LegacyFrame constFrameA(frameTypeA, (const unsigned char*)memory.data() + startFrameA, false);
			const LegacyFrame constFrameB(frameTypeB, (const unsigned char*)memory.data() + startFrameB, false);

			const size_t sizeFrameA = size_t(frameA.size());
			const size_t sizeFrameB = size_t(frameB.size());

			// memory: |                                                                                         |
			// frameA:        [offset          ]offset + size
			// frameB:                                        [offset          ]offset + size


			const size_t endFrameA = startFrameA + sizeFrameA;
			const size_t endFrameB = startFrameB + sizeFrameB;

			const size_t startIntersection = std::max(startFrameA, startFrameB);
			const size_t endIntersection = std::min(endFrameA, endFrameB);

			const bool memoryIsIntersecting = startIntersection < endIntersection;

			if (memoryIsIntersecting != frameA.haveIntersectingMemory(frameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != frameB.haveIntersectingMemory(frameA))
			{
				allSucceeded = false;
			}

			if (memoryIsIntersecting != frameA.haveIntersectingMemory(constFrameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != constFrameB.haveIntersectingMemory(frameA))
			{
				allSucceeded = false;
			}

			if (memoryIsIntersecting != constFrameA.haveIntersectingMemory(frameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != frameB.haveIntersectingMemory(constFrameA))
			{
				allSucceeded = false;
			}

			if (memoryIsIntersecting != constFrameA.haveIntersectingMemory(constFrameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != constFrameB.haveIntersectingMemory(constFrameA))
			{
				allSucceeded = false;
			}

			// identical frames always share the same memory

			if (frameA.haveIntersectingMemory(frameA) == false)
			{
				allSucceeded = false;
			}
			if (frameB.haveIntersectingMemory(frameB) == false)
			{
				allSucceeded = false;
			}

			if (constFrameA.haveIntersectingMemory(constFrameA) == false)
			{
				allSucceeded = false;
			}
			if (constFrameB.haveIntersectingMemory(constFrameB) == false)
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

bool TestLegacyFrame::testPixelFormatUniqueness()
{
	Log::info() << "Test pixel format uniqueness:";

	bool allSucceeded = true;

	const std::vector<FrameType::PixelFormat> pixelFormats = specifiedPixelFormats();
	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		allSucceeded = false;
	}

	typedef std::underlying_type<FrameType::PixelFormat>::type UnderlyingType;
	static_assert(std::is_same<UnderlyingType, uint64_t>::value, "Invalid data type!");

	std::set<UnderlyingType> uniquenessSet;

	for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
	{
		const UnderlyingType uniqueValue = UnderlyingType(pixelFormat) & UnderlyingType(0x000000000000FFFFull);

		if (uniqueValue == 0u)
		{
			allSucceeded = false;
		}

		uniquenessSet.insert(uniqueValue);
	}

	if (uniquenessSet.size() != pixelFormats.size())
	{
		allSucceeded = false;
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

bool TestLegacyFrame::testArePixelFormatsCompatible(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Are pixel formats compatible test:";

	bool allSucceeded = true;

	const std::vector<FrameType::PixelFormat> pixelFormats = specifiedPixelFormats();

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// identical pixel formats

			const unsigned int indexA = RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size()) - 1u);
			const unsigned int indexB = RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size()) - 1u);

			const FrameType::PixelFormat pixelFormatA = pixelFormats[indexA];
			const FrameType::PixelFormat pixelFormatB = pixelFormats[indexB];

			if (pixelFormatA == pixelFormatB)
			{
				if (!FrameType::arePixelFormatsCompatible(pixelFormatA, pixelFormatB) || !FrameType::arePixelFormatsCompatible(pixelFormatB, pixelFormatA))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (FrameType::arePixelFormatsCompatible(pixelFormatA, pixelFormatB) || FrameType::arePixelFormatsCompatible(pixelFormatB, pixelFormatA))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// two pure generic pixel formats

			const unsigned int dataTypeIndexA = RandomI::random(randomGenerator, (unsigned int)(FrameType::DT_UNSIGNED_INTEGER_8), (unsigned int)(FrameType::DT_SIGNED_FLOAT_64));
			const FrameType::DataType dataTypeA = FrameType::DataType(dataTypeIndexA);
			const unsigned int channelsA = RandomI::random(randomGenerator, 1u, 5u);

			const FrameType::PixelFormat pixelFormatA = FrameType::genericPixelFormat(dataTypeA, channelsA);
			ocean_assert(FrameType::formatIsPureGeneric(pixelFormatA));


			const unsigned int dataTypeIndexB = RandomI::random(randomGenerator, (unsigned int)(FrameType::DT_UNSIGNED_INTEGER_8), (unsigned int)(FrameType::DT_SIGNED_FLOAT_64));
			const FrameType::DataType dataTypeB = FrameType::DataType(dataTypeIndexB);
			const unsigned int channelsB = RandomI::random(randomGenerator, 1u, 5u);

			const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(dataTypeB, channelsB);
			ocean_assert(FrameType::formatIsPureGeneric(pixelFormatB));


			if (dataTypeA == dataTypeB && channelsA == channelsB)
			{
				if (!FrameType::arePixelFormatsCompatible(pixelFormatA, pixelFormatB) || !FrameType::arePixelFormatsCompatible(pixelFormatB, pixelFormatA))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (FrameType::arePixelFormatsCompatible(pixelFormatA, pixelFormatB) || FrameType::arePixelFormatsCompatible(pixelFormatB, pixelFormatA))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// one pure generic pixel format

			const unsigned int indexA = RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size()) - 1u);
			const FrameType::PixelFormat pixelFormatA = pixelFormats[indexA];

			if (FrameType::formatIsGeneric(pixelFormatA))
			{
				const FrameType::DataType dataTypeB = FrameType::dataType(pixelFormatA);
				const unsigned int channelsB = FrameType::channels(pixelFormatA);

				const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(dataTypeB, channelsB);

				if (!FrameType::arePixelFormatsCompatible(pixelFormatA, pixelFormatB) || !FrameType::arePixelFormatsCompatible(pixelFormatB, pixelFormatA))
				{
					allSucceeded = false;
				}
			}
			else
			{
				const unsigned int dataTypeIndexB = RandomI::random(randomGenerator, (unsigned int)(FrameType::DT_UNSIGNED_INTEGER_8), (unsigned int)(FrameType::DT_SIGNED_FLOAT_64));
				const FrameType::DataType dataTypeB = FrameType::DataType(dataTypeIndexB);
				const unsigned int channelsB = RandomI::random(randomGenerator, 1u, 5u);

				const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(dataTypeB, channelsB);
				ocean_assert(FrameType::formatIsPureGeneric(pixelFormatB));

				if (FrameType::arePixelFormatsCompatible(pixelFormatA, pixelFormatB) || FrameType::arePixelFormatsCompatible(pixelFormatB, pixelFormatA))
				{
					allSucceeded = false;
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

bool TestLegacyFrame::testTranslatePixelFormat()
{
	Log::info() << "Are pixel formats compatible test:";

	bool allSucceeded = true;

	const std::vector<FrameType::PixelFormat> pixelFormats = specifiedPixelFormats();

	for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
	{
		const std::string stringFormat = FrameType::translatePixelFormat(pixelFormat);
		const PixelFormat valueFormat = FrameType::translatePixelFormat(stringFormat);

		if (pixelFormat != valueFormat)
		{
			allSucceeded = false;
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

bool TestLegacyFrame::testMoveConstructorFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Move constructor from Frame test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const std::vector<FrameType::PixelFormat> pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_Y_U_V12,
		FrameType::FORMAT_F64,
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<float, 3u>(),
	};

	do
	{
		FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size() - 1))];
		FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * heightMultiple;

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);

		for (unsigned int nIteration = 0u; nIteration < 2u; ++nIteration)
		{
			const Timestamp timestamp = Timestamp(double(RandomI::random(randomGenerator, -100, 100)));

			Indices32 planePaddingElements;
			bool frameContainsPaddingElements = false;

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				for (unsigned int n = 0u; n < frameType.numberPlanes(); ++n)
				{
					planePaddingElements.push_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));

					if (planePaddingElements.back() != 0u)
					{
						frameContainsPaddingElements = true;
					}
				}
			}

			Frame originalFrame(frameType, planePaddingElements, timestamp);

			for (unsigned int planeIndex = 0u; planeIndex < originalFrame.numberPlanes(); ++planeIndex)
			{
				for (unsigned int y = 0u; y < originalFrame.planeHeight(planeIndex); ++y)
				{
					uint8_t* const planeRow = originalFrame.row<uint8_t>(y, planeIndex);

					for (unsigned int x = 0u; x < originalFrame.planeWidthBytes(planeIndex); ++x)
					{
						planeRow[x] = uint8_t(RandomI::random(randomGenerator, 255u));
					}
				}
			}

			if (originalFrame.isContinuous() == frameContainsPaddingElements)
			{
				allSucceeded = false;
			}

			if (nIteration == 0u)
			{
				// Move constructor

				for (unsigned int nSourceIsOwner = 0u; nSourceIsOwner < 2u; ++nSourceIsOwner)
				{
					const bool sourceIsOwner = nSourceIsOwner == 0u;

					Frame sourceFrame(originalFrame, sourceIsOwner ? Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA : Frame::ACM_USE_KEEP_LAYOUT);
					const void* sourceFrameData = sourceFrame.constdata<void>();

					LegacyFrame targetFrame(std::move(sourceFrame));

					if (sourceFrame.isValid() == (frameContainsPaddingElements == false && frameType.numberPlanes() == 1u))
					{
						allSucceeded = false;
					}

					if (!targetFrame.isValid())
					{
						allSucceeded = false;
					}

					if (targetFrame.frameType() != frameType)
					{
						allSucceeded = false;
					}

					if (targetFrame.timestamp() != timestamp)
					{
						allSucceeded = false;
					}

					if (frameContainsPaddingElements || frameType.numberPlanes() != 1u)
					{
						if (targetFrame.isOwner() == false)
						{
							allSucceeded = false;
						}

						unsigned int offset = 0u;

						for (unsigned int planeIndex = 0u; planeIndex < originalFrame.numberPlanes(); ++planeIndex)
						{
							const unsigned int widthBytes = originalFrame.planeWidthBytes(planeIndex);

							for (unsigned int y = 0u; y < originalFrame.planeHeight(planeIndex); ++y)
							{
								if (memcmp(reinterpret_cast<void*>(targetFrame.data<uint8_t>() + offset), originalFrame.row<void>(y, planeIndex), widthBytes) != 0)
								{
									allSucceeded = false;
								}

								offset += widthBytes;
							}
						}
					}
					else
					{
						if (targetFrame.constdata<void>() != sourceFrameData)
						{
							allSucceeded = false;
						}

						if (targetFrame.isOwner() != sourceIsOwner)
						{
							allSucceeded = false;
						}
					}
				}
			}
			else
			{
				ocean_assert(nIteration == 1u);

				// Move operator

				for (unsigned int nSourceIsOwner = 0u; nSourceIsOwner < 2u; ++nSourceIsOwner)
				{
					const bool sourceIsOwner = nSourceIsOwner == 0u;

					Frame sourceFrame(originalFrame, sourceIsOwner ? Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA : Frame::ACM_USE_KEEP_LAYOUT);
					const void* sourceFrameData = sourceFrame.constdata<void>();

					LegacyFrame targetFrame(frameType);

					targetFrame = std::move(sourceFrame);

					if (sourceFrame.isValid() == (frameContainsPaddingElements == false && frameType.numberPlanes() == 1u))
					{
						allSucceeded = false;
					}

					if (!targetFrame.isValid())
					{
						allSucceeded = false;
					}

					if (targetFrame.frameType() != frameType)
					{
						allSucceeded = false;
					}

					if (targetFrame.timestamp() != timestamp)
					{
						allSucceeded = false;
					}

					if (frameContainsPaddingElements || frameType.numberPlanes() != 1u)
					{
						if (targetFrame.isOwner() == false)
						{
							allSucceeded = false;
						}

						unsigned int offset = 0u;

						for (unsigned int planeIndex = 0u; planeIndex < originalFrame.numberPlanes(); ++planeIndex)
						{
							const unsigned int widthBytes = originalFrame.planeWidthBytes(planeIndex);

							for (unsigned int y = 0u; y < originalFrame.planeHeight(planeIndex); ++y)
							{
								if (memcmp(reinterpret_cast<void*>(targetFrame.data<uint8_t>() + offset), originalFrame.row<void>(y, planeIndex), widthBytes) != 0)
								{
									allSucceeded = false;
								}

								offset += widthBytes;
							}
						}
					}
					else
					{
						if (targetFrame.constdata<void>() != sourceFrameData)
						{
							allSucceeded = false;
						}

						if (targetFrame.isOwner() != sourceIsOwner)
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

bool TestLegacyFrame::testCopyConstructorFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Copy constructor from Frame test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const std::vector<FrameType::PixelFormat> pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_Y_U_V12,
		FrameType::FORMAT_F64,
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<float, 3u>(),
	};

	do
	{
		FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size() - 1))];
		FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * heightMultiple;

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);

		const Timestamp timestamp = Timestamp(double(RandomI::random(randomGenerator, -100, 100)));

		Indices32 planePaddingElements;
		bool frameContainsPaddingElements = false;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			for (unsigned int n = 0u; n < frameType.numberPlanes(); ++n)
			{
				planePaddingElements.push_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));

				if (planePaddingElements.back() != 0u)
				{
					frameContainsPaddingElements = true;
				}
			}
		}

		Frame originalFrame(frameType, planePaddingElements, timestamp);

		for (unsigned int planeIndex = 0u; planeIndex < originalFrame.numberPlanes(); ++planeIndex)
		{
			for (unsigned int y = 0u; y < originalFrame.planeHeight(planeIndex); ++y)
			{
				uint8_t* const planeRow = originalFrame.row<uint8_t>(y, planeIndex);

				for (unsigned int x = 0u; x < originalFrame.planeWidthBytes(planeIndex); ++x)
				{
					planeRow[x] = uint8_t(RandomI::random(randomGenerator, 255u));
				}
			}
		}

		for (unsigned int nSourceIsOwner = 0u; nSourceIsOwner < 2u; ++nSourceIsOwner)
		{
			const bool sourceIsOwner = nSourceIsOwner == 0u;

			Frame sourceFrame(originalFrame, sourceIsOwner ? Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA : Frame::ACM_USE_KEEP_LAYOUT);

			for (const LegacyFrame::FrameCopyMode copyMode : {LegacyFrame::FCM_USE_IF_POSSIBLE, LegacyFrame::FCM_COPY_REMOVE_PADDING_LAYOUT})
			{
				LegacyFrame targetFrame(sourceFrame, copyMode);

				if (!sourceFrame.isValid())
				{
					allSucceeded = false;
				}

				if (!targetFrame.isValid())
				{
					allSucceeded = false;
				}

				if (targetFrame.frameType() != frameType)
				{
					allSucceeded = false;
				}

				if (targetFrame.timestamp() != timestamp)
				{
					allSucceeded = false;
				}

				bool expectedIsOwner = false;

				if (frameContainsPaddingElements)
				{
					expectedIsOwner = true;
				}
				else
				{
					if (copyMode == FCM_USE_IF_POSSIBLE)
					{
						expectedIsOwner = frameType.numberPlanes() != 1u;
					}
					else
					{
						ocean_assert(copyMode == FCM_COPY_REMOVE_PADDING_LAYOUT);

						expectedIsOwner = true;
					}
				}

				if (targetFrame.isOwner() != expectedIsOwner)
				{
					allSucceeded = false;
				}

				const uint8_t* targetFrameData = targetFrame.data<uint8_t>();

				for (unsigned int planeIndex = 0u; planeIndex < originalFrame.numberPlanes(); ++planeIndex)
				{
					for (unsigned int y = 0u; y < originalFrame.planeHeight(planeIndex); ++y)
					{
						uint8_t* const planeRow = originalFrame.row<uint8_t>(y, planeIndex);

						if (memcmp(planeRow, targetFrameData, originalFrame.planeWidthBytes(planeIndex)) != 0)
						{
							allSucceeded = false;
						}

						targetFrameData += originalFrame.planeWidthBytes(planeIndex);
					}
				}

				if (targetFrameData != targetFrame.data<uint8_t>() + targetFrame.frameTypeSize())
				{
					allSucceeded = false;
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

bool TestLegacyFrame::testSet(const LegacyFrame& templateFrame, const FrameType& frameType, const bool forceOwner, const bool forceWritable, const bool isValidExpected, const bool isOwnerExpected, const bool isReadonlyExpected, const Timestamp& timestampExpected, const unsigned int sizeExpected)
{
	LegacyFrame frame;
	copyFrame(templateFrame, frame);
	frame.set(frameType, forceOwner, forceWritable);

	return validateFrame(frame, isValidExpected, isOwnerExpected, isReadonlyExpected, timestampExpected, sizeExpected);
}

bool TestLegacyFrame::testSet(const LegacyFrame& templateFrame, const FrameType& frameType, const bool isValidExpected, const bool isOwnerExpected, const bool isReadonlyExpected, const Timestamp& timestampExpected, const unsigned int sizeExpected)
{
	bool allSucceeded = true;

	LegacyFrame frame;
	copyFrame(templateFrame, frame);
	frame.set(frameType);

	allSucceeded = validateFrame(frame, isValidExpected, isOwnerExpected, isReadonlyExpected, timestampExpected, sizeExpected) && allSucceeded;

	copyFrame(templateFrame, frame);
	frame.set(frameType, false, false);

	allSucceeded = validateFrame(frame, isValidExpected, isOwnerExpected, isReadonlyExpected, timestampExpected, sizeExpected) && allSucceeded;

	return allSucceeded;
}

bool TestLegacyFrame::testSet(const LegacyFrame& templateFrame, const FrameType& frameType, const Timestamp& timestamp, LegacyFrame& dataFrame, const bool passConstData, const bool copyData, const bool isValidExpected, const bool isOwnerExpected, const bool isReadonlyExpected, const unsigned int sizeExpected)
{
	LegacyFrame frame;

	copyFrame(templateFrame, frame);

	if (passConstData)
		frame.set(frameType, timestamp, dataFrame.isValid() ? dataFrame.constdata<uint8_t>() : nullptr, copyData);
	else
		frame.set(frameType, timestamp, dataFrame.isValid() ? dataFrame.data<uint8_t>() : nullptr, copyData);

	return validateFrame(frame, isValidExpected, isOwnerExpected, isReadonlyExpected, timestamp, sizeExpected) && validateFrameData(frame, dataFrame);
}

void TestLegacyFrame::copyFrame(const LegacyFrame& frame, LegacyFrame& frameCopy)
{
	if (frame.isValid())
	{
		if (frame.isOwner())
		{
			frameCopy = LegacyFrame(frame, true);
		}
		else
		{
			frameCopy = LegacyFrame(frame, false);
		}
	}
	else
	{
		frameCopy = LegacyFrame();
	}
}

bool TestLegacyFrame::validateFrame(const LegacyFrame& frame, const bool isValid, const bool isOwner, const bool isReadonly, const Timestamp& timestamp, const unsigned int size)
{
	bool result;

	if (!isValid)
	{
		result = frame.isValid() == false && frame.isOwner() == isOwner && timestamp == frame.timestamp() && frame.size() == size;
	}
	else
	{
		result = frame.isValid() && frame.isOwner() == isOwner && frame.isReadOnly() == isReadonly && timestamp == frame.timestamp() && frame.size() == size;
	}

	ocean_assert(result);
	return result;
}

bool TestLegacyFrame::validateFrameData(const LegacyFrame& frame, const LegacyFrame& expectedFrame)
{
	bool result;

	if (frame.isValid() && expectedFrame.isValid() && frame.size() <= expectedFrame.size())
	{
		result = memcmp(frame.constdata<void>(), expectedFrame.constdata<void>(), frame.size()) == 0;
	}
	else
	{
		result = !frame.isValid() && !expectedFrame.isValid();
	}

	ocean_assert(result);
	return result;
}

std::vector<FrameType::PixelFormat> TestLegacyFrame::specifiedPixelFormats()
{
	const std::vector<FrameType::PixelFormat> pixelFormats =
	{
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_ARGB32,
		FrameType::FORMAT_BGR24,
		FrameType::FORMAT_BGR32,
		FrameType::FORMAT_BGR4444,
		FrameType::FORMAT_BGR5551,
		FrameType::FORMAT_BGR565,
		FrameType::FORMAT_BGRA32,
		FrameType::FORMAT_BGRA4444,
		FrameType::FORMAT_BGGR10_PACKED,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGB32,
		FrameType::FORMAT_RGB4444,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_RGB565,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_RGBA4444,
		FrameType::FORMAT_RGBT32,
		FrameType::FORMAT_RGGB10_PACKED,
		FrameType::FORMAT_Y_U_V12, // FORMAT_Y_U_V12_LIMITED_RANGE
		FrameType::FORMAT_YUV24,
		FrameType::FORMAT_YUVA32,
		FrameType::FORMAT_YUVT32,
		FrameType::FORMAT_Y_V_U12,
		FrameType::FORMAT_YVU24,
		FrameType::FORMAT_Y_UV12, // FORMAT_Y_UV12_LIMITED_RANGE
		FrameType::FORMAT_Y_VU12, // FORMAT_Y_VU12_LIMITED_RANGE
		FrameType::FORMAT_UYVY16,
		FrameType::FORMAT_YUYV16,
		FrameType::FORMAT_Y8, // FORMAT_Y8_FULL_RANGE
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y10_PACKED,
		FrameType::FORMAT_Y16,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_Y64,
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_RGB48,
		FrameType::FORMAT_RGBA64,
		FrameType::FORMAT_Y_U_V24, // FORMAT_Y_U_V24_LIMITED_RANGE
		FrameType::FORMAT_Y_U_V24_FULL_RANGE,
		FrameType::FORMAT_Y8_LIMITED_RANGE,
		FrameType::FORMAT_Y_UV12_FULL_RANGE,
		FrameType::FORMAT_Y_VU12_FULL_RANGE,
		FrameType::FORMAT_Y_U_V12_FULL_RANGE,
		FrameType::FORMAT_Y_V_U12_FULL_RANGE,
		FrameType::FORMAT_F32,
		FrameType::FORMAT_F64
	};

	ocean_assert(pixelFormats.size() == size_t(FrameType::FORMAT_END) - 1);

	return pixelFormats;
}

unsigned int TestLegacyFrame::averageBitsPerPixel(const FrameType::PixelFormat pixelFormat)
{
	typedef std::pair<FrameType::PixelFormat, uint32_t> TestPair;
	typedef std::vector<TestPair> TestPairs;

	const TestPairs testPairs = // **TODO** should be a map
	{
		TestPair(FrameType::FORMAT_ABGR32, 32u),
		TestPair(FrameType::FORMAT_ARGB32, 32u),
		TestPair(FrameType::FORMAT_BGR24, 24u),
		TestPair(FrameType::FORMAT_BGR32, 32u),
		TestPair(FrameType::FORMAT_BGR4444, 16u),
		TestPair(FrameType::FORMAT_BGR5551, 16u),
		TestPair(FrameType::FORMAT_BGR565, 16u),
		TestPair(FrameType::FORMAT_BGRA32, 32u),
		TestPair(FrameType::FORMAT_BGRA4444, 16u),
		TestPair(FrameType::FORMAT_RGB24, 24u),
		TestPair(FrameType::FORMAT_RGB32, 32u),
		TestPair(FrameType::FORMAT_RGB4444, 16u),
		TestPair(FrameType::FORMAT_RGB5551, 16u),
		TestPair(FrameType::FORMAT_RGB565, 16u),
		TestPair(FrameType::FORMAT_RGBA32, 32u),
		TestPair(FrameType::FORMAT_RGBA4444, 16u),
		TestPair(FrameType::FORMAT_RGBT32, 32u),
		TestPair(FrameType::FORMAT_YUV24, 24u),
		TestPair(FrameType::FORMAT_YUVA32, 32u),
		TestPair(FrameType::FORMAT_YUVT32, 32u),
		TestPair(FrameType::FORMAT_YVU24, 24u),
		TestPair(FrameType::FORMAT_UYVY16, 16u),
		TestPair(FrameType::FORMAT_YUYV16, 16u),
		TestPair(FrameType::FORMAT_Y16, 16u),
		TestPair(FrameType::FORMAT_Y32, 32u),
		TestPair(FrameType::FORMAT_Y64, 64u),
		TestPair(FrameType::FORMAT_YA16, 16u),
		TestPair(FrameType::FORMAT_RGB48, 48u),
		TestPair(FrameType::FORMAT_RGBA64, 64u),
		TestPair(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, 24u),
		TestPair(FrameType::FORMAT_Y_U_V24_FULL_RANGE, 24u),
		TestPair(FrameType::FORMAT_Y8_LIMITED_RANGE, 8u),
		TestPair(FrameType::FORMAT_Y8_FULL_RANGE, 8u),
		TestPair(FrameType::FORMAT_Y10, 16u),
		TestPair(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_UV12_FULL_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_VU12_FULL_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_U_V12_FULL_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, 12u),
		TestPair(FrameType::FORMAT_Y_V_U12_FULL_RANGE, 12u),
		TestPair(FrameType::FORMAT_F32, 32u),
		TestPair(FrameType::FORMAT_F64, 64u)
	};

	// ensuring that we have covered all pixel formats

	if (testPairs.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		return 0u;
	}

	if (FrameType::formatIsGeneric(pixelFormat))
	{
		return FrameType::bytesPerDataType(FrameType::dataType(pixelFormat)) * FrameType::channels(pixelFormat) * 8u;
	}

	for (const TestPair& testPair : testPairs)
	{
		if (testPair.first == pixelFormat)
		{
			return testPair.second;
		}
	}

	ocean_assert(false && "Missing pixel format!");
	return 0u;
}

}

}

}
