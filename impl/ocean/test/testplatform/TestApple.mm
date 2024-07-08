/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testplatform/TestApple.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS
	#include <AppKit/AppKit.h>
#else
	#include <UIKit/UIKit.h>
#endif

namespace Ocean
{

namespace Test
{

namespace TestPlatform
{

bool TestApple::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Apple test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testToCGImage(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire Apple test succeeded.";
	}
	else
	{
		Log::info() << "Apple test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestApple, ToCGImage)
{
	EXPECT_TRUE(TestApple::testToCGImage(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestApple::testToCGImage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Utilities::toCGImage() test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
	};

	const Timestamp startTimestamp(true);
	do
	{
		for (const FrameType::PixelFormat pixelFormat : pixelFormats)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);
			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), paddingElements);

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int n = 0u; n < frame.planeWidthBytes(0u); ++n)
				{
					row[n] = uint8_t(RandomI::random(randomGenerator, 255u));
				}
			}

			const bool copyData = RandomI::random(randomGenerator, 1u) == 0u;

			Platform::Apple::ScopedCGImageRef cgImageRef(Platform::Apple::Utilities::toCGImage(frame, copyData));

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS
			NSImage* image = [[NSImage alloc] initWithCGImage:cgImageRef.object() size:NSMakeSize(width, height)];
#else
			UIImage* image = [[UIImage alloc] initWithCGImage:cgImageRef.object()];
#endif

			// now, we release the frame and the CG image to ensure that the NS/UIImage is still valid

			cgImageRef.release();
			frame.release();

			// now, we ensure that we can still access the image

			const CGSize size = image.size;

			if (double(size.width) != double(width) || double(size.height) != double(height))
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

	return true;
}

}

}

}
