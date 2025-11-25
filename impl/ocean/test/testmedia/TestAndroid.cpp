/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestAndroid.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include "ocean/media/android/VideoDecoder.h"
#include "ocean/media/android/VideoEncoder.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestAndroid::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Android test");
	Log::info() << " ";

	if (selector.shouldRun("videoencoderdecoder"))
	{
		testResult = testVideoEncoderDecoder(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

bool TestAndroid::testVideoEncoderDecoder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Video encoder/decoder test:";

	Validation validation;

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	constexpr unsigned int bitrate = 2000000u; // 2 Mbps
	constexpr double frameRate = 30.0;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		// Test different codecs
		const std::vector<std::string> mimeTypes =
		{
			"video/avc",  // H.264
			"video/hevc"  // H.265
		};

		// Test different I-frame interval configurations
		const std::vector<int> iFrameIntervals =
		{
			-1,  // Only first frame is a key frame (no periodic key frames)
			0,   // All frames are key frames
			2    // Key frame every 2 seconds
		};

		for (const std::string& mimeType : mimeTypes)
		{
			for (const int iFrameInterval : iFrameIntervals)
			{
				Log::info() << " ";
				Log::info() << "Testing MIME type: " << mimeType << ", I-frame interval: " << iFrameInterval << " seconds";

				Media::Android::VideoEncoder videoEncoder;

				if (!videoEncoder.initialize(width, height, mimeType, bitrate, frameRate, iFrameInterval))
				{
					Log::info() << "Codec " << mimeType << " is not supported on this device, skipping...";
					continue;
				}

				if (!videoEncoder.start())
				{
					Log::error() << "Failed to start encoder for " << mimeType;
					OCEAN_SET_FAILED(validation);

					continue;
				}

				Media::Android::VideoDecoder videoDecoder;

				if (!videoDecoder.initialize(mimeType, width, height))
				{
					Log::error() << "Failed to initialize decoder for " << mimeType;
					OCEAN_SET_FAILED(validation);

					continue;
				}

				if (!videoDecoder.start())
				{
					Log::error() << "Failed to start decoder for " << mimeType;
					OCEAN_SET_FAILED(validation);

					continue;
				}

				constexpr unsigned int numberFrames = 10u;

				Frames testFrames;
				Media::Android::VideoEncoder::Samples encodedSamples;
				Frames decodedFrames;

				{
					// first, let's encode several frames

					for (unsigned int frameIndex = 0u; frameIndex < numberFrames; ++frameIndex)
					{
						// Create a test frame with a unique pattern
						Frame testFrame = createTestFrame(width, height, frameIndex);

						const uint64_t presentationTime = uint64_t(1.0e6 * double(frameIndex) / frameRate);

						if (!videoEncoder.pushFrame(testFrame, presentationTime))
						{
							Log::error() << "Failed to push frame " << frameIndex;
							OCEAN_SET_FAILED(validation);

							break;
						}

						testFrames.push_back(std::move(testFrame));

						Media::Android::VideoEncoder::Sample sample = videoEncoder.popSample();

						if (sample)
						{
							encodedSamples.push_back(std::move(sample));
						}
					}

					constexpr unsigned int expectedSamples = numberFrames + 1u; // one extra configuration frame

					Timestamp timeoutTimestamp(true);

					while (encodedSamples.size() < expectedSamples && !timeoutTimestamp.hasTimePassed(2.0))
					{
						Media::Android::VideoEncoder::Sample sample = videoEncoder.popSample();

						if (sample)
						{
							encodedSamples.push_back(std::move(sample));

							timeoutTimestamp.toNow();
						}
						else
						{
							Thread::sleep(1u);
						}
					}
				}

				{
					// now, we decode the frames again

					for (const Media::Android::VideoEncoder::Sample& sample : encodedSamples)
					{
						if (!videoDecoder.pushSample(sample.data().data(), sample.data().size(), uint64_t(sample.presentationTime())))
						{
							Log::error() << "Failed to push encoded sample to decoder";
							OCEAN_SET_FAILED(validation);

							break;
						}

						int64_t decodedPresentationTime = -1;
						Frame decodedFrame = videoDecoder.popFrame(&decodedPresentationTime);

						if (decodedFrame.isValid())
						{
							decodedFrames.push_back(std::move(decodedFrame));
						}
					}

					Timestamp timeoutTimestamp(true);

					while (decodedFrames.size() < numberFrames && !timeoutTimestamp.hasTimePassed(2.0))
					{
						int64_t decodedPresentationTime = -1;
						Frame decodedFrame = videoDecoder.popFrame(&decodedPresentationTime);

						if (decodedFrame.isValid())
						{
							decodedFrames.push_back(std::move(decodedFrame));
						}
						else
						{
							Thread::sleep(1u);
						}
					}
				}

				OCEAN_EXPECT_EQUAL(validation, testFrames.size(), decodedFrames.size());

				for (size_t n = 0; n < std::min(testFrames.size(), decodedFrames.size()); ++n)
				{
					const Frame& testFrame = testFrames[n];
					const Frame& decodedFrame = decodedFrames[n];

					OCEAN_EXPECT_TRUE(validation, validateTestFrame(testFrame, decodedFrame));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Frame TestAndroid::createTestFrame(const unsigned int width, const unsigned int height, const unsigned int frameIndex)
{
	ocean_assert(width >= 1u && height >= 1u);

	Frame frame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			uint8_t* const pixel = frame.pixel<uint8_t>(x, y);

			const uint8_t r = uint8_t((x + frameIndex * 10u) % 256u);
			const uint8_t g = uint8_t((y + frameIndex * 15u) % 256u);
			const uint8_t b = uint8_t(((x + y) + frameIndex * 20u) % 256u);

			pixel[0] = r;
			pixel[1] = g;
			pixel[2] = b;
		}
	}

	return frame;
}

bool TestAndroid::validateTestFrame(const Frame& testFrame, const Frame& decodedFrame)
{
	ocean_assert(testFrame.isValid());
	ocean_assert(decodedFrame.isValid());

	if (!testFrame.isValid() || !decodedFrame.isValid())
	{
		return false;
	}

	Frame rgbTestFrame;
	if (!CV::FrameConverter::Comfort::convert(testFrame, FrameType::FORMAT_RGB24, rgbTestFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		return false;
	}

	Frame rgbDecodedFrame;
	if (!CV::FrameConverter::Comfort::convert(decodedFrame, FrameType::FORMAT_RGB24, rgbDecodedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		return false;
	}

	if (rgbTestFrame.frameType() != rgbDecodedFrame.frameType())
	{
		return false;
	}

	constexpr unsigned int maxDifference = 25u; // allow up to 10% different values per channel due to compression artifacts

	unsigned int differentPixels = 0u;

	for (unsigned int y = 0u; y < rgbTestFrame.height(); ++y)
	{
		for (unsigned int x = 0u; x < rgbTestFrame.width(); ++x)
		{
			const uint8_t* testPixel = rgbTestFrame.constpixel<uint8_t>(x, y);
			const uint8_t* decodedPixel = rgbDecodedFrame.constpixel<uint8_t>(x, y);

			for (unsigned int nChannel = 0u; nChannel < 3u; ++nChannel)
			{
				const int32_t testValue = int32_t(testPixel[nChannel]);
				const int32_t decodedValue = int32_t(decodedPixel[nChannel]);

				const unsigned int diff = (unsigned int)(std::abs(testValue - decodedValue));

				if (diff > maxDifference)
				{
					++differentPixels;
				}
			}
		}
	}

	const unsigned int totalPixels = testFrame.pixels() * 3u;
	const double differenceRatio = double(differentPixels) / double(totalPixels);

	constexpr double differenceThreshold = 0.1; // allow up to 10% of pixels to be significantly different

	return differenceRatio <= differenceThreshold;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#endif // OCEAN_PLATFORM_BUILD_ANDROID
