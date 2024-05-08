/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestImageQuality.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/ImageQuality.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestImageQuality::test(const unsigned int /*width*/, const unsigned int /*height*/, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Image Quality test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testStructuralSimilarityStressTest(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultiScaleStructuralSimilarityStressTest(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Image Quality test succeeded.";
	}
	else
	{
		Log::info() << "Image Quality test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestImageQuality, StructuralSimilarityStressTest)
{
	Worker worker;
	EXPECT_TRUE(TestImageQuality::testStructuralSimilarityStressTest(GTEST_TEST_DURATION, worker));
}

TEST(TestImageQuality, MultiScaleStructuralSimilarityStressTest)
{
	Worker worker;
	EXPECT_TRUE(TestImageQuality::testMultiScaleStructuralSimilarityStressTest(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestImageQuality::testStructuralSimilarityStressTest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Structural similarity stress test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 11u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 11u, 1920u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const unsigned int frameXPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int frameYPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

		Frame frameX(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), frameXPaddingElements);
		Frame frameY(frameX.frameType(), frameYPaddingElements);

		// we toggle between a similar image, and a completely random image
		// similar images should have a ssim >= 0.97 while random images should have a ssim <= 0.03

		const bool similarImage = RandomI::random(randomGenerator, 1u) == 1u;

		CV::CVUtilities::randomizeFrame(frameX, false, &randomGenerator);

		if (similarImage)
		{
			CV::FrameConverter::subFrame<uint8_t>(frameX.constdata<uint8_t>(), frameY.data<uint8_t>(), frameX.width(), frameX.height(), frameY.width(), frameY.height(), frameX.channels(), 0u, 0u, 0u, 0u, frameX.width(), frameX.height(), frameX.paddingElements(), frameY.paddingElements());

			// we change 2% of all pixel values

			for (unsigned int nPixel = 0u; nPixel < (width * height * 2u) / 100u; ++nPixel)
			{
				const unsigned int x = RandomI::random(randomGenerator, width - 1u);
				const unsigned int y = RandomI::random(randomGenerator, height - 1u);

				for (unsigned int nChannel = 0u; nChannel < channels; ++nChannel)
				{
					frameY.pixel<uint8_t>(x, y)[nChannel] = uint8_t(minmax<int>(0, frameY.pixel<uint8_t>(x, y)[nChannel] + RandomI::random(randomGenerator, -30, 30), 255));
				}
			}
		}
		else
		{
			CV::CVUtilities::randomizeFrame(frameY, false, &randomGenerator);
		}

		Worker* useWorker = (worker && RandomI::random(randomGenerator, 1u) == 1u) ? &worker : nullptr;

		double meanSSIM = -1.0;
		double meanContrast = -1.0;
		if (CV::ImageQuality::structuralSimilarity8BitPerChannel(frameX.constdata<uint8_t>(), frameY.constdata<uint8_t>(), width, height, channels, frameX.paddingElements(), frameY.paddingElements(), meanSSIM, meanContrast, useWorker))
		{
			if (meanSSIM < 0.0 || meanSSIM > 1.0)
			{
				allSucceeded = false;
			}

			if (meanContrast < 0.0 || meanContrast > 1.0)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (width >= 200u && height >= 200u)
		{
			// we need a minimum image resolution to ensure a correct validation

			if (meanSSIM == 1.0)
			{
				// this would be too perfect
				allSucceeded = false;
			}

			if (similarImage)
			{
				if (meanSSIM < 0.97)
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (meanSSIM > 0.03)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestImageQuality::testMultiScaleStructuralSimilarityStressTest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Multi-scale structural similarity stress test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 11u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 11u, 1920u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const unsigned int frameXPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int frameYPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);


		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

		Frame frameX(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), frameXPaddingElements);
		Frame frameY(frameX.frameType(), frameYPaddingElements);

		// we toggle between a similar image, and a completely random image
		// similar images should have a ssim >= 0.85 while random images should have a ssim <= 0.15

		const bool similarImage = RandomI::random(randomGenerator, 1u) == 1u;

		CV::CVUtilities::randomizeFrame(frameX, false, &randomGenerator);

		if (similarImage)
		{
			frameY.copy(frameX);

			// we change 2% of all pixel values

			for (unsigned int nPixel = 0u; nPixel < (width * height * 2u) / 100u; ++nPixel)
			{
				const unsigned int x = RandomI::random(randomGenerator, width - 1u);
				const unsigned int y = RandomI::random(randomGenerator, height - 1u);

				for (unsigned int nChannel = 0u; nChannel < channels; ++nChannel)
				{
					frameY.pixel<uint8_t>(x, y)[nChannel] = uint8_t(minmax<int>(0, frameY.pixel<uint8_t>(x, y)[nChannel] + RandomI::random(randomGenerator, -30, 30), 255));
				}
			}
		}
		else
		{
			CV::CVUtilities::randomizeFrame(frameY, false, &randomGenerator);
		}

		Worker* useWorker = (worker && RandomI::random(randomGenerator, 1u) == 1u) ? &worker : nullptr;

		double msssim = -1.0;
		if (CV::ImageQuality::multiScaleStructuralSimilarity8BitPerChannel(frameX.constdata<uint8_t>(), frameY.constdata<uint8_t>(), width, height, channels, frameX.paddingElements(), frameY.paddingElements(), msssim, useWorker))
		{
			if (msssim < 0.0 || msssim > 1.0)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (width >= 200u && height >= 200u)
		{
			// we need a minimum image resolution to ensure a correct validation

			if (msssim == 1.0)
			{
				// this would be too perfect
				allSucceeded = false;
			}

			if (similarImage)
			{
				if (msssim < 0.85)
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (msssim > 0.15)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
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
