/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestWIC.h"

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/media/BufferImage.h"
#include "ocean/media/BufferImageRecorder.h"
#include "ocean/media/Manager.h"

#include "ocean/media/wic/Image.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestWIC::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#endif

	Log::info() << "WIC test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAnyImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire WIC test succeeded.";
	}
	else
	{
		Log::info() << "WIC test FAILED!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#endif

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

/**
 * This class implements a simple instance for the GTest ensuring that
 * the WIC plugin is registered during startup and unregistered before shutdown.
 */
class TestWICGTestInstance : public ::testing::Test
{
	protected:

		/**
		 * Default constructor.
		 */
  		TestWICGTestInstance()
  		{
  			// nothing to do here
		}

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::WIC::registerWICLibrary();
#endif
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::WIC::unregisterWICLibrary();
#endif
		}
};

TEST_F(TestWICGTestInstance, AnyImageEncodeDecode)
{
	EXPECT_TRUE(TestWIC::testAnyImageEncodeDecode(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestWIC::testAnyImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Any image encode/decode test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const std::vector<FrameType::PixelFormat> pixelFormats = {FrameType::FORMAT_BGR24, FrameType::FORMAT_BGRA32, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32, FrameType::FORMAT_Y8};

	const std::vector<std::string> encoderTypes = {"bmp", "jpg", "png", "tif"};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 8u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 8u, 1080u);
		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		Indices32 paddingElements;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			paddingElements.push_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
		}

		Frame sourceFrame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);
		ocean_assert(sourceFrame.isValid());

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());

		for (size_t n = 0; n < encoderTypes.size(); ++n)
		{
			const std::string& encoderType = encoderTypes[n];

			if (sourceFrame.hasAlphaChannel() && (encoderType == "jpg" || encoderType == "bmp"))
			{
				// this combination is not supported
				continue;
			}

			std::vector<uint8_t> buffer;
			if (!Media::WIC::Image::encodeImage(sourceFrame, encoderType, buffer, true))
			{
				allSucceeded = false;
				break;
			}

			std::string decoderTypeExplicit;
			const Frame targetFrameExplicit = Media::WIC::Image::decodeImage(buffer.data(), buffer.size(), encoderType, &decoderTypeExplicit);

			if (!targetFrameExplicit.isValid() || encoderType != decoderTypeExplicit)
			{
				allSucceeded = false;
			}
			else
			{
				Frame convertedFrame;
				if (!CV::FrameConverter::Comfort::convert(targetFrameExplicit, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
				{
					allSucceeded = false;
				}
				else
				{
					double minDifference, aveDifference, maxDifference;
					if (!determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10) // quite generous
					{
						allSucceeded = false;
					}
				}
			}

			std::string decoderTypeImplicit;
			const Frame targetFrameImplicit = Media::WIC::Image::decodeImage(buffer.data(), buffer.size(), "", &decoderTypeImplicit);

			if (!targetFrameImplicit.isValid() || encoderType != decoderTypeImplicit)
			{
				allSucceeded = false;
			}
			else
			{
				Frame convertedFrame;
				if (!CV::FrameConverter::Comfort::convert(targetFrameImplicit, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
				{
					allSucceeded = false;
				}
				else
				{
					double minDifference, aveDifference, maxDifference;
					if (!determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10) // quite generous
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
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestWIC::determineSimilarity(const Frame& firstFrame, const Frame& secondFrame, double& minDifference, double& aveDifference, double& maxDifference)
{
	ocean_assert(firstFrame.frameType() == secondFrame.frameType());
	ocean_assert(firstFrame.numberPlanes() == 1u);

	if (firstFrame.frameType() != secondFrame.frameType() || firstFrame.numberPlanes() != 1u)
	{
		return false;
	}

	switch (firstFrame.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
			determineSimilarity(firstFrame.constdata<uint8_t>(), secondFrame.constdata<uint8_t>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_16:
			determineSimilarity(firstFrame.constdata<uint16_t>(), secondFrame.constdata<uint16_t>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_32:
			determineSimilarity(firstFrame.constdata<uint32_t>(), secondFrame.constdata<uint32_t>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		default:
			return false;
	}
}

template <typename T>
void TestWIC::determineSimilarity(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements, double& minDifference, double& aveDifference, double& maxDifference)
{
	ocean_assert(firstFrame != nullptr && secondFrame != nullptr);

	const unsigned int widthElements = width * channels;

	const unsigned int firstFrameStrideElements = widthElements + firstFramePaddingElements;
	const unsigned int secondFrameStrideElements = widthElements + secondFramePaddingElements;

	double sumDifference = 0.0;
	minDifference = NumericD::maxValue();
	maxDifference = 0.0;

	for (unsigned int y = 0u; y < height; ++y)
	{
		const T* const firstFrameRow = firstFrame + y * firstFrameStrideElements;
		const T* const secondFrameRow = secondFrame + y * secondFrameStrideElements;

		for (unsigned int n = 0u; n < widthElements; ++n)
		{
			const double difference = fabs(double(firstFrameRow[n]) - double(secondFrameRow[n]));

			minDifference = min(minDifference, difference);
			maxDifference = max(maxDifference, difference);
			sumDifference += difference;
		}
	}

	const unsigned int size = widthElements * height;

	if (size != 0)
	{
		aveDifference = sumDifference / double(size);
	}
	else
	{
		aveDifference = NumericD::maxValue();
	}
}

}

}

}

#endif // OCEAN_PLATFORM_BUILD_WINDOWS
