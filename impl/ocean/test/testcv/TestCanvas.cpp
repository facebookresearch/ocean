/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestCanvas.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestCanvas::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test Canvas:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testColors() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLinePixelAccuracy(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPointNoFraction(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPointWithFraction(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Canvas test succeeded.";
	}
	else
	{
		Log::info() << "Canvas test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCanvas, Colors)
{
	EXPECT_TRUE(TestCanvas::testColors());
}

TEST(TestCanvas, LinePixelAccuracy)
{
	EXPECT_TRUE(TestCanvas::testLinePixelAccuracy(GTEST_TEST_DURATION));
}

TEST(TestCanvas, PointNoFraction)
{
	EXPECT_TRUE(TestCanvas::testPointNoFraction(GTEST_TEST_DURATION));
}

TEST(TestCanvas, PointWithFraction)
{
	EXPECT_TRUE(TestCanvas::testPointWithFraction(GTEST_TEST_DURATION));
}

#endif

bool TestCanvas::testColors()
{
	Log::info() << "Testing color functions:";

	bool allSucceeded = true;

	// BGR24
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGR24)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGR24)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGR24)[2] == 0x00u && allSucceeded;

	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGR24)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGR24)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGR24)[2] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGR24)[0] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGR24)[1] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGR24)[2] == 0x80u && allSucceeded;

	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGR24)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGR24)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGR24)[2] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGR24)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGR24)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGR24)[2] == 0x00u && allSucceeded;

	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGR24)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGR24)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGR24)[2] == 0x00u && allSucceeded;


	// BGRA32
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGRA32)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGRA32)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGRA32)[2] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_BGRA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGRA32)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGRA32)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGRA32)[2] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_BGRA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGRA32)[0] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGRA32)[1] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGRA32)[2] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_BGRA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGRA32)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGRA32)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGRA32)[2] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_BGRA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGRA32)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGRA32)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGRA32)[2] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_BGRA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGRA32)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGRA32)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGRA32)[2] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_BGRA32)[3] == 0xFFu && allSucceeded;


	// RGB24
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGB24)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGB24)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGB24)[2] == 0x00u && allSucceeded;

	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGB24)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGB24)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGB24)[2] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGB24)[0] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGB24)[1] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGB24)[2] == 0x80u && allSucceeded;

	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGB24)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGB24)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGB24)[2] == 0x00u && allSucceeded;

	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGB24)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGB24)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGB24)[2] == 0x00u && allSucceeded;

	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGB24)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGB24)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGB24)[2] == 0xFFu && allSucceeded;


	// RGBA32
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGBA32)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGBA32)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGBA32)[2] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_RGBA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGBA32)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGBA32)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGBA32)[2] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_RGBA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGBA32)[0] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGBA32)[1] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGBA32)[2] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_RGBA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGBA32)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGBA32)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGBA32)[2] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_RGBA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGBA32)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGBA32)[1] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGBA32)[2] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_RGBA32)[3] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGBA32)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGBA32)[1] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGBA32)[2] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_RGBA32)[3] == 0xFFu && allSucceeded;


	// Y8
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_Y8)[0] == 0x00u && allSucceeded;

	allSucceeded = CV::Canvas::white(FrameType::FORMAT_Y8)[0] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_Y8)[0] == 0x80u && allSucceeded;

	allSucceeded = CV::Canvas::red(FrameType::FORMAT_Y8)[0] == 76u && allSucceeded;

	allSucceeded = CV::Canvas::green(FrameType::FORMAT_Y8)[0] == 150u && allSucceeded;

	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_Y8)[0] == 29u && allSucceeded;


	// YA16
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_YA16)[0] == 0x00u && allSucceeded;
	allSucceeded = CV::Canvas::black(FrameType::FORMAT_YA16)[1] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::white(FrameType::FORMAT_YA16)[0] == 0xFFu && allSucceeded;
	allSucceeded = CV::Canvas::white(FrameType::FORMAT_YA16)[1] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_YA16)[0] == 0x80u && allSucceeded;
	allSucceeded = CV::Canvas::gray(FrameType::FORMAT_YA16)[1] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::red(FrameType::FORMAT_YA16)[0] == 76u && allSucceeded;
	allSucceeded = CV::Canvas::red(FrameType::FORMAT_YA16)[1] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::green(FrameType::FORMAT_YA16)[0] == 150u && allSucceeded;
	allSucceeded = CV::Canvas::green(FrameType::FORMAT_YA16)[1] == 0xFFu && allSucceeded;

	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_YA16)[0] == 29u && allSucceeded;
	allSucceeded = CV::Canvas::blue(FrameType::FORMAT_YA16)[1] == 0xFFu && allSucceeded;

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

bool TestCanvas::testLinePixelAccuracy(const double testDuration)
{
	Log::info() << "Testing line with pixel accuracy:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 5u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 5u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

		Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		frame.setValue(0xFF);

		const Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const int xStart = int(RandomI::random(randomGenerator, 0u, width - 1u));
		const int yStart = int(RandomI::random(randomGenerator, 0u, height - 1u));

		const int xEnd = int(RandomI::random(randomGenerator, 0u, width - 1u));
		const int yEnd = int(RandomI::random(randomGenerator, 0u, height - 1u));

		std::vector<uint8_t> color(channels);
		for (unsigned int n = 0u; n < channels; ++n)
		{
			color[n] = uint8_t(RandomI::random(randomGenerator, 0u, 254u /* 255 - 1 */));
		}

		const bool useColor = RandomI::random(randomGenerator, 1u) == 0u;

		switch (channels)
		{
			case 1u:
				CV::Canvas::line8BitPerChannel<1u>(frame.data<uint8_t>(), width, height, xStart, yStart, xEnd, yEnd, useColor ? color.data() : nullptr, frame.paddingElements());
				break;

			case 2u:
				CV::Canvas::line8BitPerChannel<2u>(frame.data<uint8_t>(), width, height, xStart, yStart, xEnd, yEnd, useColor ? color.data() : nullptr, frame.paddingElements());
				break;

			case 3u:
				CV::Canvas::line8BitPerChannel<3u>(frame.data<uint8_t>(), width, height, xStart, yStart, xEnd, yEnd, useColor ? color.data() : nullptr, frame.paddingElements());
				break;

			case 4u:
				CV::Canvas::line8BitPerChannel<4u>(frame.data<uint8_t>(), width, height, xStart, yStart, xEnd, yEnd, useColor ? color.data() : nullptr, frame.paddingElements());
				break;

			case 5u:
				CV::Canvas::line8BitPerChannel<5u>(frame.data<uint8_t>(), width, height, xStart, yStart, xEnd, yEnd, useColor ? color.data() : nullptr, frame.paddingElements());
				break;

			default:
				ocean_assert(false && "Invalid channel number!");
				return false;
		}

		// we gather all pixels belonging to the line

		std::unordered_set<CV::PixelPosition, CV::PixelPosition> pixelSet;
		pixelSet.insert(CV::PixelPosition((unsigned int)(xStart), (unsigned int)(yStart)));

		CV::Bresenham bresenham(xStart, yStart, xEnd, yEnd);

		int xi = xStart;
		int yi = yStart;

		while (xi != xEnd || yi != yEnd)
		{
			bresenham.findNext(xi, yi);

			pixelSet.insert(CV::PixelPosition((unsigned int)(xi), (unsigned int)(yi)));
		}

		ocean_assert(pixelSet.find(CV::PixelPosition((unsigned int)(xStart), (unsigned int)(yStart))) != pixelSet.end());
		ocean_assert(pixelSet.find(CV::PixelPosition((unsigned int)(xEnd), (unsigned int)(yEnd))) != pixelSet.end());

		if (useColor == false)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				color[n] = uint8_t(0u);
			}
		}

		std::vector<uint8_t> white(channels, 0xFF);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				const uint8_t* pixel = frame.constpixel<uint8_t>(x, y);

				if (pixelSet.find(CV::PixelPosition(x, y)) == pixelSet.end())
				{
					// the pixel does not belong to the line, so we expect a white pixel

					if (memcmp(pixel, white.data(), channels) != 0)
					{
						allSucceeded = false;
					}
				}
				else
				{
					// the pixel belongs to the line, so we expect a colored pixel

					if (memcmp(pixel, color.data(), channels) != 0)
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

bool TestCanvas::testPointNoFraction(const double testDuration)
{
	Log::info() << "Testing point without fraction:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 100u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 100u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		for (const CV::PixelCenter pixelCenter : {CV::PC_TOP_LEFT, CV::PC_CENTER})
		{
			for (const unsigned int pointSize : {1u, 3u, 5u, 7u, 9u, 11u, 13u, 15u})
			{
				std::vector<uint8_t> backgroundColor(channels);
				std::vector<uint8_t> foregroundColor(channels);

				for (uint8_t& value : backgroundColor)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				bool differentEnough = false;

				do
				{
					for (uint8_t& value : foregroundColor)
					{
						value = uint8_t(RandomI::random(randomGenerator, 255u));
					}

					for (size_t n = 0; n < backgroundColor.size(); ++n)
					{
						if (std::abs(int(foregroundColor[n]) - int(backgroundColor[n])) > 50)
						{
							differentEnough = true;
							break;
						}
					}
				}
				while (!differentEnough);

				const unsigned int xFull = RandomI::random(randomGenerator, pointSize * 2u, width - pointSize * 2u - 1u);
				const unsigned int yFull = RandomI::random(randomGenerator, pointSize * 2u, height - pointSize * 2u - 1u);

				{
					// testing point with perfect center, entirely visible in the frame

					const unsigned int pointSize_2 = pointSize / 2u;

					frame.setValue(backgroundColor.data(), backgroundColor.size());

					Scalar xFullWithPixelCenter = Scalar(xFull);
					Scalar yFullWithPixelCenter = Scalar(yFull);

					if (pixelCenter != CV::PC_TOP_LEFT)
					{
						// in case the pixel center is in the actual center of the pixel (not in the top-left corner), we need to apply a half-pixel offset

						xFullWithPixelCenter += Scalar(0.5);
						yFullWithPixelCenter += Scalar(0.5);
					}

					if (!CV::Canvas::Comfort::point(frame, Vector2(xFullWithPixelCenter, yFullWithPixelCenter), pixelCenter, pointSize, foregroundColor.data()))
					{
						allSucceeded = false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, frameCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const int radius = int(pointSize / 2u + 2u);

					DistanceMap distanceMap;

					for (int yOffset = -radius; yOffset <= radius; ++yOffset)
					{
						for (int xOffset = -radius; xOffset <= radius; ++xOffset)
						{
							const unsigned int xPosition = (unsigned int)(int(xFull) + xOffset);
							const unsigned int yPosition = (unsigned int)(int(yFull) + yOffset);
							ocean_assert(xPosition < width && yPosition < height);

							const uint8_t* pixel = frame.constpixel<uint8_t>(xPosition, yPosition);

							if (xOffset == 0 && yOffset == 0)
							{
								// we check that the center pixel has exactly the foreground color

								if (memcmp(pixel, foregroundColor.data(), sizeof(uint8_t) * foregroundColor.size()) != 0)
								{
									allSucceeded = false;
								}
							}
							else if (std::abs(xOffset) > int(pointSize_2) || std::abs(yOffset) > int(pointSize_2))
							{
								// we check that the pixel outside of the point radius is the background color

								if (memcmp(pixel, backgroundColor.data(), sizeof(uint8_t) * backgroundColor.size()) != 0)
								{
									allSucceeded = false;
								}
							}
							else
							{
								unsigned int xOffsetAbs = (unsigned int)(std::abs(xOffset));
								unsigned int yOffsetAbs = (unsigned int)(std::abs(yOffset));

								Utilities::sortHighestToFront2(xOffsetAbs, yOffsetAbs);

								const CV::PixelPosition absOffset(xOffsetAbs, yOffsetAbs);

								DistanceMap::const_iterator iDistance = distanceMap.find(absOffset);

								if (iDistance == distanceMap.cend())
								{
									distanceMap.emplace(absOffset, pixel);
								}
								else
								{
									// ensure that we get the same color for all distances

									for (unsigned int n = 0u; n < channels; ++n)
									{
										if (iDistance->second[n] != pixel[n])
										{
											allSucceeded = false;
										}
									}
								}
							}
						}
					}

					// ensuring that the upper left quadrant of the point is correct

					UnorderedIndexSet32 ssds;

					for (int yOffset = -radius; yOffset <= 0; ++yOffset)
					{
						for (int xOffset = -radius; xOffset <= 0; ++xOffset)
						{
							const unsigned int xPosition = (unsigned int)(int(xFull) + xOffset);
							const unsigned int yPosition = (unsigned int)(int(yFull) + yOffset);
							ocean_assert(xPosition < width && yPosition < height);

							const unsigned int ssdPixel = ssd(frame.constpixel<uint8_t>(xPosition, yPosition), frame.constpixel<uint8_t>(xFull, yFull), channels);

							if (xOffset != 0)
							{
								const unsigned int ssdRight = ssd(frame.constpixel<uint8_t>(xPosition + 1u, yPosition), frame.constpixel<uint8_t>(xFull, yFull), channels);

								if (ssdPixel < ssdRight)
								{
									allSucceeded = false;
								}
							}

							if (yOffset != 0)
							{
								const unsigned int ssdBottom = ssd(frame.constpixel<uint8_t>(xPosition, yPosition + 1u), frame.constpixel<uint8_t>(xFull, yFull), channels);

								if (ssdPixel < ssdBottom)
								{
									allSucceeded = false;
								}
							}

							if (xOffset != 0 && yOffset != 0)
							{
								const unsigned int ssdBottomRight = ssd(frame.constpixel<uint8_t>(xPosition + 1u, yPosition + 1u), frame.constpixel<uint8_t>(xFull, yFull), channels);

								if (ssdPixel < ssdBottomRight)
								{
									allSucceeded = false;
								}
							}

							ssds.emplace(ssdPixel);
						}
					}

					if (pointSize == 1)
					{
						if (ssds.size() != 2)
						{
							allSucceeded = false;
						}
					}
					else
					{
						if (ssds.size() < std::min(pointSize, 7u))
						{
							allSucceeded = false;
						}
					}
				}

				{
					// testing point with perfect center, partially visible in the frame

					Frame subFrame = CV::CVUtilities::randomizedFrame(FrameType(pointSize + 2u, pointSize + 2u, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					subFrame.setValue(backgroundColor.data(), backgroundColor.size());

					const Frame copySubFrame(subFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const int xPartial = RandomI::random(randomGenerator, -int(pointSize), int(subFrame.width() + pointSize));
					const int yPartial = RandomI::random(randomGenerator, -int(pointSize), int(subFrame.height() + pointSize));

					Scalar xPartialWithPixelCenter = Scalar(xPartial);
					Scalar yPartialWithPixelCenter = Scalar(yPartial);

					if (pixelCenter == CV::PC_CENTER)
					{
						// in case the pixel center is in the actual center of the pixel (not in the top-left corner), we need to apply a half-pixel offset

						xPartialWithPixelCenter += Scalar(0.5);
						yPartialWithPixelCenter += Scalar(0.5);
					}

					if (!CV::Canvas::Comfort::point(subFrame, Vector2(xPartialWithPixelCenter, yPartialWithPixelCenter), pixelCenter, pointSize, foregroundColor.data()))
					{
						allSucceeded = false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(subFrame, copySubFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const int xShift = xFull - xPartial;
					const int yShift = yFull - yPartial;

					for (unsigned int y = 0u; y < subFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < subFrame.width(); ++x)
						{
							const int xLookupFull = (int(x) + xShift);
							const int yLookupFull = (int(y) + yShift);

							if (xLookupFull >= 0 && xLookupFull < int(subFrame.width())
									&& yLookupFull >= 0 && yLookupFull < int(subFrame.height()))
							{
								if (memcmp(subFrame.constpixel<uint8_t>(x, y), frame.constpixel<uint8_t>((unsigned int)(xLookupFull), (unsigned int)(yLookupFull)), sizeof(uint8_t) * channels) != 0)
								{
									allSucceeded = false;
								}
							}
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

bool TestCanvas::testPointWithFraction(const double testDuration)
{
	Log::info() << "Testing point with fraction:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 100u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 100u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		// for verification, we render the point in four different frames and interpolate the results

		Frame frameTopLeft = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);
		Frame frameTopRight = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);
		Frame frameBottomLeft = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);
		Frame frameBottomRight = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

		for (const CV::PixelCenter pixelCenter : {CV::PC_TOP_LEFT, CV::PC_CENTER})
		{
			for (const unsigned int pointSize : {1u, 3u, 5u, 7u, 9u, 11u, 13u, 15u})
			{
				std::vector<uint8_t> backgroundColor(channels);
				std::vector<uint8_t> foregroundColor(channels);

				for (uint8_t& value : backgroundColor)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				for (uint8_t& value : foregroundColor)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				const unsigned int xLeft = RandomI::random(randomGenerator, pointSize * 2u, width - pointSize * 2u - 1u);
				const unsigned int yTop = RandomI::random(randomGenerator, pointSize * 2u, height - pointSize * 2u - 1u);

				frame.setValue(backgroundColor.data(), backgroundColor.size());
				frameTopLeft.setValue(backgroundColor.data(), backgroundColor.size());
				frameTopRight.setValue(backgroundColor.data(), backgroundColor.size());
				frameBottomLeft.setValue(backgroundColor.data(), backgroundColor.size());
				frameBottomRight.setValue(backgroundColor.data(), backgroundColor.size());

				// in case the pixel center is in the actual center of the pixel (not in the top-left corner), we need to apply a half-pixel offset
				const Vector2 pixelCenterOffset = pixelCenter == CV::PC_TOP_LEFT ? Vector2(0, 0) : Vector2(Scalar(0.5), Scalar(0.5));

				if (!CV::Canvas::Comfort::point(frameTopLeft, Vector2(Scalar(xLeft), Scalar(yTop)) + pixelCenterOffset, pixelCenter, pointSize, foregroundColor.data())
						|| !CV::Canvas::Comfort::point(frameTopRight, Vector2(Scalar(xLeft + 1u), Scalar(yTop)) + pixelCenterOffset, pixelCenter, pointSize, foregroundColor.data())
						|| !CV::Canvas::Comfort::point(frameBottomLeft, Vector2(Scalar(xLeft), Scalar(yTop + 1u)) + pixelCenterOffset, pixelCenter, pointSize, foregroundColor.data())
						|| !CV::Canvas::Comfort::point(frameBottomRight, Vector2(Scalar(xLeft + 1u), Scalar(yTop + 1u)) + pixelCenterOffset, pixelCenter, pointSize, foregroundColor.data()))
				{
					allSucceeded = false;
				}

				const Scalar xFactor = Random::scalar(randomGenerator, Scalar(0), Scalar(1));
				const Scalar yFactor = Random::scalar(randomGenerator, Scalar(0), Scalar(1));

				const Scalar x = Scalar(xLeft) + xFactor;
				const Scalar y = Scalar(yTop) + yFactor;

				if (!CV::Canvas::Comfort::point(frame, Vector2(x, y) + pixelCenterOffset, pixelCenter, pointSize, foregroundColor.data()))
				{
					allSucceeded = false;
				}

				const int radius = int(pointSize / 2u + 2u);

				const Scalar factorTopLeft = (Scalar(1) - xFactor) * (Scalar(1) - yFactor);
				const Scalar factorTopRight = xFactor * (Scalar(1) - yFactor);
				const Scalar factorBottomLeft = (Scalar(1) - xFactor) * yFactor;
				const Scalar factorBottomRight = xFactor * yFactor;

				ocean_assert(Numeric::isEqual(factorTopLeft + factorTopRight + factorBottomLeft + factorBottomRight, Scalar(1)));

				for (int yOffset = -radius; yOffset <= radius; ++yOffset)
				{
					for (int xOffset = -radius; xOffset <= radius; ++xOffset)
					{
						const unsigned int xPositionLeft = (unsigned int)(int(xLeft) + xOffset);
						const unsigned int yPositionTop = (unsigned int)(int(yTop) + yOffset);
						ocean_assert(xPositionLeft < width && yPositionTop < height);

						const uint8_t* pixel = frame.constpixel<uint8_t>(xPositionLeft, yPositionTop);

						const uint8_t* pixelTopLeft = frameTopLeft.constpixel<uint8_t>(xPositionLeft, yPositionTop);
						const uint8_t* pixelTopRight = frameTopRight.constpixel<uint8_t>(xPositionLeft, yPositionTop);
						const uint8_t* pixelBottomLeft = frameBottomLeft.constpixel<uint8_t>(xPositionLeft, yPositionTop);
						const uint8_t* pixelBottomRight = frameBottomRight.constpixel<uint8_t>(xPositionLeft, yPositionTop);

						for (unsigned int n = 0u; n < channels; ++n)
						{
							const Scalar value = Scalar(pixelTopLeft[n]) * factorTopLeft + Scalar(pixelTopRight[n]) * factorTopRight + Scalar(pixelBottomLeft[n]) * factorBottomLeft + Scalar(pixelBottomRight[n]) * factorBottomRight;

							if (Numeric::isNotEqual(value, Scalar(pixel[n]), 5))
							{
								allSucceeded = false;
							}
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

unsigned int TestCanvas::ssd(const uint8_t* pixel0, const uint8_t* pixel1, const unsigned int channels)
{
	ocean_assert(pixel0 != nullptr);
	ocean_assert(pixel1 != nullptr);
	ocean_assert(channels >= 1u);

	unsigned int result = 0u;

	for (unsigned int n = 0u; n < channels; ++n)
	{
		result += (unsigned int)(NumericT<int>::sqrDistance(int(pixel0[n]), int(pixel1[n])));
	}

	return result;
}

}

}

}
