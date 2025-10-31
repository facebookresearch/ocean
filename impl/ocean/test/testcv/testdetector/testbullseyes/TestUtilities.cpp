/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestUtilities.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

using namespace CV::Detector::Bullseyes;

bool TestUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for Utilities:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = testCreateBullseyeImage(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDrawBullseye(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDrawBullseyes(testDuration, randomGenerator) && allSucceeded;

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

} // namespace TestBullseyes

TEST(TestUtilities, CreateBullseyeImage)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestUtilities::testCreateBullseyeImage(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestUtilities, DrawBullseye)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestUtilities::testDrawBullseye(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestUtilities, DrawBullseyes)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestUtilities::testDrawBullseyes(GTEST_TEST_DURATION, randomGenerator));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestUtilities::testCreateBullseyeImage(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Utilities::createBullseyeImage() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		// Generate random diameter (must be multiple of 5, and >= 15)
		// Canvas::ellipse requires sizes to be odd and >= 3
		// Since centerDiscDiameter = diameter / 5, we need diameter / 5 to be odd and >= 3
		// This means diameter = 5 * (2k + 1) where k >= 1, giving us 15, 25, 35, ...
		const unsigned int k = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int diameter = 5u * (2u * k + 1u);

		// Generate random empty border
		const unsigned int emptyBorder = RandomI::random(randomGenerator, 0u, 100u);

		// Randomly decide whether to use custom colors or nullptr
		const bool useCustomForegroundColor = RandomI::random(randomGenerator, 1u) == 0u;
		const bool useCustomBackgroundColor = RandomI::random(randomGenerator, 1u) == 0u;

		uint8_t foregroundColorBuffer[3];
		uint8_t backgroundColorBuffer[3];

		const uint8_t* foregroundColor = nullptr;
		const uint8_t* backgroundColor = nullptr;

		if (useCustomForegroundColor)
		{
			foregroundColorBuffer[0] = uint8_t(RandomI::random(randomGenerator, 255u));
			foregroundColorBuffer[1] = uint8_t(RandomI::random(randomGenerator, 255u));
			foregroundColorBuffer[2] = uint8_t(RandomI::random(randomGenerator, 255u));
			foregroundColor = foregroundColorBuffer;
		}

		if (useCustomBackgroundColor)
		{
			backgroundColorBuffer[0] = uint8_t(RandomI::random(randomGenerator, 255u));
			backgroundColorBuffer[1] = uint8_t(RandomI::random(randomGenerator, 255u));
			backgroundColorBuffer[2] = uint8_t(RandomI::random(randomGenerator, 255u));
			backgroundColor = backgroundColorBuffer;
		}

		// Create the bullseye image
		Frame rgbFrame;
		if (!Utilities::createBullseyeImage(diameter, emptyBorder, rgbFrame, foregroundColor, backgroundColor))
		{
			allSucceeded = false;
			break;
		}

		// Verify the pixel format is RGB24
		if (rgbFrame.pixelFormat() != FrameType::FORMAT_RGB24)
		{
			allSucceeded = false;
			break;
		}

		// Verify the frame size is correct
		const unsigned int expectedSize = diameter + 2u * emptyBorder;
		if (rgbFrame.width() != expectedSize || rgbFrame.height() != expectedSize)
		{
			allSucceeded = false;
			break;
		}

		// Define the expected colors (use defaults if nullptr was passed)
		const uint8_t expectedForeground[3] = {foregroundColor ? foregroundColor[0] : uint8_t(0), foregroundColor ? foregroundColor[1] : uint8_t(0), foregroundColor ? foregroundColor[2] : uint8_t(0)};
		const uint8_t expectedBackground[3] = {backgroundColor ? backgroundColor[0] : uint8_t(255), backgroundColor ? backgroundColor[1] : uint8_t(255), backgroundColor ? backgroundColor[2] : uint8_t(255)};

		// Verify that the frame only contains the specified colors
		bool foundUnexpectedColor = false;
		for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
		{
			const uint8_t* row = rgbFrame.constrow<uint8_t>(y);

			for (unsigned int x = 0u; x < rgbFrame.width(); ++x)
			{
				const uint8_t r = row[x * 3u + 0u];
				const uint8_t g = row[x * 3u + 1u];
				const uint8_t b = row[x * 3u + 2u];

				const bool isForeground = (r == expectedForeground[0] && g == expectedForeground[1] && b == expectedForeground[2]);
				const bool isBackground = (r == expectedBackground[0] && g == expectedBackground[1] && b == expectedBackground[2]);

				if (!isForeground && !isBackground)
				{
					foundUnexpectedColor = true;
					break;
				}
			}

			if (foundUnexpectedColor)
			{
				break;
			}
		}

		if (foundUnexpectedColor)
		{
			allSucceeded = false;
			break;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestUtilities::testDrawBullseye(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Utilities::drawBullseye() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		// Create a fixed size image for simpler testing
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		// Create a white RGB24 frame (so we can see colored lines)
		Frame frame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame.setValue(0xFF);

		// Create a bullseye in the center with a moderate radius
		const Scalar centerX = Scalar(width) / Scalar(2);
		const Scalar centerY = Scalar(height) / Scalar(2);
		const Scalar radius = Scalar(50);

		const Vector2 position(centerX, centerY);
		const unsigned int grayThreshold = RandomI::random(randomGenerator, 1u, 255u);
		const Bullseye bullseye(position, radius, grayThreshold);

		// Use red color
		uint8_t colorBuffer[3];
		colorBuffer[0] = uint8_t(255); // Red
		colorBuffer[1] = uint8_t(0);
		colorBuffer[2] = uint8_t(0);
		const uint8_t* color = colorBuffer;

		// Draw the bullseye
		Utilities::drawBullseye(frame, bullseye, color);

		// Verify that at least some pixels changed from white (255,255,255)
		bool foundChangedPixel = false;
		for (unsigned int y = 0u; y < frame.height() && !foundChangedPixel; ++y)
		{
			const uint8_t* row = frame.constrow<uint8_t>(y);

			for (unsigned int x = 0u; x < frame.width(); ++x)
			{
				const uint8_t r = row[x * 3u + 0u];
				const uint8_t g = row[x * 3u + 1u];
				const uint8_t b = row[x * 3u + 2u];

				// Check if this pixel is NOT white
				if (r != 255u || g != 255u || b != 255u)
				{
					foundChangedPixel = true;
					break;
				}
			}
		}

		if (!foundChangedPixel)
		{
			allSucceeded = false;
			break;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestUtilities::testDrawBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Utilities::drawBullseyes() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		// Create a fixed size image for simpler testing
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		// Create two identical white RGB24 frames (so we can see colored lines)
		Frame frame1(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame frame2(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame1.setValue(0xFF);
		frame2.setValue(0xFF);

		// Create a few bullseyes at fixed positions
		std::vector<Bullseye> bullseyes;
		bullseyes.reserve(3u);

		const unsigned int grayThreshold = 128u;

		// Bullseye 1: center
		bullseyes.emplace_back(Vector2(Scalar(320), Scalar(240)), Scalar(50), grayThreshold);

		// Bullseye 2: upper left quadrant
		bullseyes.emplace_back(Vector2(Scalar(160), Scalar(120)), Scalar(30), grayThreshold);

		// Bullseye 3: lower right quadrant
		bullseyes.emplace_back(Vector2(Scalar(480), Scalar(360)), Scalar(40), grayThreshold);

		// Use red color
		uint8_t colorBuffer[3];
		colorBuffer[0] = uint8_t(255); // Red
		colorBuffer[1] = uint8_t(0);
		colorBuffer[2] = uint8_t(0);
		const uint8_t* color = colorBuffer;

		// Draw using drawBullseyes on frame1
		Utilities::drawBullseyes(frame1, bullseyes.data(), bullseyes.size(), color);

		// Draw using individual drawBullseye calls on frame2
		for (const Bullseye& bullseye : bullseyes)
		{
			Utilities::drawBullseye(frame2, bullseye, color);
		}

		// Verify that both frames are identical
		bool framesIdentical = true;
		for (unsigned int y = 0u; y < frame1.height(); ++y)
		{
			const uint8_t* row1 = frame1.constrow<uint8_t>(y);
			const uint8_t* row2 = frame2.constrow<uint8_t>(y);

			for (unsigned int x = 0u; x < frame1.width() * 3u; ++x)
			{
				if (row1[x] != row2[x])
				{
					framesIdentical = false;
					break;
				}
			}

			if (!framesIdentical)
			{
				break;
			}
		}

		if (!framesIdentical)
		{
			allSucceeded = false;
			break;
		}

		// Verify that at least some pixels changed from white
		bool foundChangedPixel = false;

		for (unsigned int y = 0u; y < frame1.height() && !foundChangedPixel; ++y)
		{
			const uint8_t* row = frame1.constrow<uint8_t>(y);

			for (unsigned int x = 0u; x < frame1.width(); ++x)
			{
				const uint8_t r = row[x * 3u + 0u];
				const uint8_t g = row[x * 3u + 1u];
				const uint8_t b = row[x * 3u + 2u];

				// Check if this pixel is NOT white
				if (r != 255u || g != 255u || b != 255u)
				{
					foundChangedPixel = true;
					break;
				}
			}
		}

		if (!foundChangedPixel)
		{
			allSucceeded = false;
			break;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

Bullseye TestUtilities::createRandomValidBullseye(RandomGenerator& randomGenerator)
{
	const Vector2 position(Random::scalar(randomGenerator, Scalar(0), Scalar(4096)), Random::scalar(randomGenerator, Scalar(0), Scalar(4096)));
	const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), Scalar(1024));
	const unsigned int grayThreshold = RandomI::random(randomGenerator, 1u, 255u);

	return Bullseye(position, radius, grayThreshold);
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
