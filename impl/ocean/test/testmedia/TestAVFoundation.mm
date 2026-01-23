/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestAVFoundation.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/avfoundation/PixelBufferAccessor.h"
#include "ocean/media/avfoundation/VideoDecoder.h"
#include "ocean/media/avfoundation/VideoEncoder.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestAVFoundation::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("AVFoundation test");

	Log::info() << " ";

	if (selector.shouldRun("pixelbufferaccessorgenericpixelformats"))
	{
		testResult = testPixelBufferAccessorGenericPixelFormats(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("pixelbufferaccessornongenericpixelformats"))
	{
		testResult = testPixelBufferAccessorNonGenericPixelFormats(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("videoencoderdecoder"))
	{
		testResult = testVideoEncoderDecoder(testDuration);

		Log::info() << " ";
	}

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestAVFoundation, PixelBufferAccessorGenericPixelFormats)
{
	EXPECT_TRUE(TestAVFoundation::testPixelBufferAccessorGenericPixelFormats(GTEST_TEST_DURATION));
}

TEST(TestAVFoundation, PixelBufferAccessorNonGenericPixelFormats)
{
	EXPECT_TRUE(TestAVFoundation::testPixelBufferAccessorNonGenericPixelFormats(GTEST_TEST_DURATION));
}

TEST(TestAVFoundation, VideoEncoderDecoder)
{
	EXPECT_TRUE(TestAVFoundation::testVideoEncoderDecoder(GTEST_TEST_DURATION));
}

#endif // #ifdef OCEAN_USE_GTEST

bool TestAVFoundation::testPixelBufferAccessorGenericPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel buffer accessor with generic pixel formats test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	using PixelFormatPair = std::pair<FrameType::PixelFormat, OSType>;

	const std::vector<PixelFormatPair> pixelFormatPairs =
	{
		std::make_pair(FrameType::FORMAT_ARGB32, kCVPixelFormatType_32ARGB),
		std::make_pair(FrameType::FORMAT_BGRA32, kCVPixelFormatType_32BGRA),
		std::make_pair(FrameType::FORMAT_RGB24, kCVPixelFormatType_24RGB),
		std::make_pair(FrameType::FORMAT_Y8, kCVPixelFormatType_OneComponent8)
	};

	{
		// an default accessor object should be invalid

		const Media::AVFoundation::PixelBufferAccessor invalidAccessor;
		OCEAN_EXPECT_FALSE(validation, bool(invalidAccessor));
	}

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const PixelFormatPair pixelFormatPair = RandomI::random(randomGenerator, pixelFormatPairs);

		const FrameType::PixelFormat pixelFormat = pixelFormatPair.first;
		const OSType osPixelFormat = pixelFormatPair.second;

		CVPixelBufferRef pixelBuffer = nullptr;
		const CVReturn createResult = CVPixelBufferCreate(kCFAllocatorDefault, size_t(width), size_t(height), osPixelFormat, nullptr, &pixelBuffer);

		if (createResult != kCVReturnSuccess)
		{
			ocean_assert(false && "This must never happen!");
			OCEAN_SET_FAILED(validation);
			break;
		}

		std::vector<std::vector<uint8_t>> memoryCopy;
		Indices32 memoryCopyHeights;
		Indices32 memoryCopyStrideBytes;
		if (!writeRandomDataToPixelBuffer(pixelBuffer, 1, randomGenerator, memoryCopy, memoryCopyHeights, memoryCopyStrideBytes))
		{
			ocean_assert(false && "This must never happen!");
			OCEAN_SET_FAILED(validation);
			break;
		}

		for (const bool readOnly : {true, false})
		{
			Media::AVFoundation::PixelBufferAccessor accessor(pixelBuffer, readOnly);

			if (accessor)
			{
				const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

				if (readOnly)
				{
					const Frame& frame = accessor.frame();

					OCEAN_EXPECT_EQUAL(validation, frame.frameType(), frameType);
					OCEAN_EXPECT_EQUAL(validation, frame.isReadOnly(), readOnly);
				}
				else
				{
					Frame& frame = accessor.frame();

					OCEAN_EXPECT_EQUAL(validation, frame.frameType(), frameType);
					OCEAN_EXPECT_EQUAL(validation, frame.isReadOnly(), readOnly);
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}

			if (validation.succeededSoFar())
			{
				// ensuring that the memory data is actually correct

				const Frame& frame = accessor.frame();

				const Frame testFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

				for (unsigned int y = 0u; y < testFrame.height(); ++y)
				{
					const uint8_t* frameData = frame.constrow<uint8_t>(y);
					const uint8_t* testFrameData = testFrame.constrow<uint8_t>(y);

					// we ensure that the padding area is identical as well
					OCEAN_EXPECT_EQUAL(validation, memcmp(frameData, testFrameData, frame.strideBytes()), 0);
				}
			}
		}

		CVPixelBufferRelease(pixelBuffer);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestAVFoundation::testPixelBufferAccessorNonGenericPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel buffer accessor with non-generic pixel formats test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	struct PixelFormatSpecifier
	{
		FrameType::PixelFormat pixelFormat;
		OSType osPixelFormat;
		size_t numberPlanes;
	};

	const std::vector<PixelFormatSpecifier> pixelFormatSpecifiers =
	{
		{FrameType::FORMAT_Y_UV12_LIMITED_RANGE, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, 2},
		{FrameType::FORMAT_Y_UV12_FULL_RANGE, kCVPixelFormatType_420YpCbCr8BiPlanarFullRange, 2},
		{FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, kCVPixelFormatType_420YpCbCr8Planar, 3},
		{FrameType::FORMAT_Y_U_V12_FULL_RANGE, kCVPixelFormatType_420YpCbCr8PlanarFullRange, 3}
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 2u, 1920u) & 0xFFFFFFFEu;
		const unsigned int height = RandomI::random(randomGenerator, 2u, 1080u) & 0xFFFFFFFEu;
		ocean_assert(width % 2u == 0u && height % 2 == 0u);

		const PixelFormatSpecifier pixelFormatSpecifier = RandomI::random(randomGenerator, pixelFormatSpecifiers);

		CVPixelBufferRef pixelBuffer = nullptr;
		const CVReturn createResult = CVPixelBufferCreate(kCFAllocatorDefault, size_t(width), size_t(height), pixelFormatSpecifier.osPixelFormat, nullptr, &pixelBuffer);

		if (createResult != kCVReturnSuccess)
		{
			ocean_assert(false && "This must never happen!");
			OCEAN_SET_FAILED(validation);
			break;
		}

		std::vector<std::vector<uint8_t>> memoryCopy;
		Indices32 memoryCopyHeights;
		Indices32 memoryCopyStrideBytes;
		if (!writeRandomDataToPixelBuffer(pixelBuffer, pixelFormatSpecifier.numberPlanes, randomGenerator, memoryCopy, memoryCopyHeights, memoryCopyStrideBytes))
		{
			ocean_assert(false && "This must never happen!");
			OCEAN_SET_FAILED(validation);
			break;
		}

		for (const bool readOnly : {true, false})
		{
			for (const bool accessYPlaneOnly : {true, false})
			{
				Media::AVFoundation::PixelBufferAccessor accessor(pixelBuffer, readOnly, accessYPlaneOnly);

				if (accessor)
				{
					FrameType::PixelFormat expectedPixelFormat = pixelFormatSpecifier.pixelFormat;

					if (accessYPlaneOnly)
					{
						switch (pixelFormatSpecifier.pixelFormat)
						{
							case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
							case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
								expectedPixelFormat = FrameType::FORMAT_Y8_LIMITED_RANGE;
								break;

							case FrameType::FORMAT_Y_UV12_FULL_RANGE:
							case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
								expectedPixelFormat = FrameType::FORMAT_Y8_FULL_RANGE;
								break;

							default:
								ocean_assert(false && "This should never happen!");
								expectedPixelFormat = FrameType::FORMAT_UNDEFINED;
								break;
						}
					}

					const FrameType expectedFrameType(width, height, expectedPixelFormat, FrameType::ORIGIN_UPPER_LEFT);

					Frame& frame = accessor.frame();
					ocean_assert(frame.isValid());

					OCEAN_EXPECT_EQUAL(validation, frame.isReadOnly(), readOnly);
					OCEAN_EXPECT_EQUAL(validation, frame.frameType(), expectedFrameType);

					const size_t numberPlanesToCheck = accessYPlaneOnly ? 1 : memoryCopy.size();

					for (size_t nPlane = 0; nPlane < numberPlanesToCheck; ++nPlane)
					{
						OCEAN_EXPECT_LESS(validation, nPlane, frame.planes().size());

						if (nPlane >= frame.planes().size())
						{
							break;
						}

						const unsigned int strideBytes = memoryCopyStrideBytes[nPlane];

						OCEAN_EXPECT_EQUAL(validation, frame.planes()[nPlane].strideBytes(), strideBytes);

						if (frame.planes()[nPlane].strideBytes() != strideBytes)
						{
							break;
						}

						const uint8_t* memoryCopyData = memoryCopy[nPlane].data();
						const uint8_t* frameData = frame.planes()[nPlane].constdata<uint8_t>();

						for (unsigned int y = 0u; y < memoryCopyHeights[nPlane]; ++y)
						{
							OCEAN_EXPECT_EQUAL(validation, memcmp(frameData, memoryCopyData, strideBytes), 0);

							frameData += strideBytes;
							memoryCopyData += strideBytes;
						}
					}
				}
			}
		}

		CVPixelBufferRelease(pixelBuffer);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestAVFoundation::writeRandomDataToPixelBuffer(void* voidPixelBuffer, const size_t expectedPlanes, RandomGenerator& randomGenerator, std::vector<std::vector<uint8_t>>& memoryCopy, Indices32& heights, Indices32& strideBytes)
{
	ocean_assert(voidPixelBuffer != nullptr);
	ocean_assert(expectedPlanes >= 1);
	ocean_assert(memoryCopy.empty());

	ocean_assert(heights.empty());
	ocean_assert(strideBytes.empty());

	CVPixelBufferRef pixelBuffer = CVPixelBufferRef(voidPixelBuffer);

	if (pixelBuffer == nullptr)
	{
		return false;
	}

	const size_t planes = CVPixelBufferGetPlaneCount(pixelBuffer);

	if (planes == 0 && expectedPlanes == 1)
	{
		memoryCopy.clear();
		memoryCopy.resize(1);

		heights.clear();
		heights.resize(1);

		strideBytes.clear();
		strideBytes.resize(1);

		if (CVPixelBufferLockBaseAddress(pixelBuffer, 0) != kCVReturnSuccess)
		{
			return false;
		}

		const size_t pixelBufferBytesPerPlane = CVPixelBufferGetBytesPerRow(pixelBuffer);
		const size_t height = CVPixelBufferGetHeight(pixelBuffer);

		const size_t bytesInPlane = pixelBufferBytesPerPlane * height;

		memoryCopy[0].resize(bytesInPlane);
		uint8_t* memoryCopyPlane = memoryCopy[0].data();

		uint8_t* pixelBufferPlaneData = (uint8_t*)(CVPixelBufferGetBaseAddress(pixelBuffer));

		if (pixelBufferPlaneData == nullptr)
		{
			return false;
		}

		heights[0] = (unsigned int)(height);
		strideBytes[0] = (unsigned int)(pixelBufferBytesPerPlane);

		for (size_t nByte = 0; nByte < bytesInPlane; ++nByte)
		{
			const uint8_t value = uint8_t(RandomI::random(randomGenerator, 255u));

			pixelBufferPlaneData[nByte] = value;
			memoryCopyPlane[nByte] = value;
		}

		CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
	}
	else
	{
		if (planes != expectedPlanes)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		memoryCopy.clear();
		memoryCopy.resize(planes);

		heights.clear();
		heights.resize(planes);

		strideBytes.clear();
		strideBytes.resize(planes);

		if (CVPixelBufferLockBaseAddress(pixelBuffer, 0) != kCVReturnSuccess)
		{
			return false;
		}

		for (size_t nPlane = 0; nPlane < planes; ++nPlane)
		{
			const size_t pixelBufferBytesPerPlane = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, nPlane);
			const size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, nPlane);

			const size_t bytesInPlane = pixelBufferBytesPerPlane * height;

			memoryCopy[nPlane].resize(bytesInPlane);
			uint8_t* memoryCopyPlane = memoryCopy[nPlane].data();

			uint8_t* pixelBufferPlaneData = (uint8_t*)(CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, nPlane));

			if (pixelBufferPlaneData == nullptr)
			{
				return false;
			}

			heights[nPlane] = (unsigned int)(height);
			strideBytes[nPlane] = (unsigned int)(pixelBufferBytesPerPlane);

			for (size_t nByte = 0; nByte < bytesInPlane; ++nByte)
			{
				const uint8_t value = uint8_t(RandomI::random(randomGenerator, 255u));

				pixelBufferPlaneData[nByte] = value;
				memoryCopyPlane[nByte] = value;
			}
		}

		CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
	}

	return true;
}

bool TestAVFoundation::testVideoEncoderDecoder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Video encoder/decoder test:";

	Validation validation;

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	constexpr unsigned int bitrate = 2000000u; // 2 Mbps
	constexpr double frameRate = 30.0;

	const Timestamp startTimestamp(true);

	do
	{
		// Test different codecs
		const Strings mimeTypes =
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

				Media::AVFoundation::VideoEncoder videoEncoder;

				if (!videoEncoder.initialize(width, height, mimeType, frameRate, bitrate, iFrameInterval))
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

			Media::AVFoundation::VideoDecoder videoDecoder;

				constexpr unsigned int numberFrames = 10u;

				Frames testFrames;
				Media::AVFoundation::VideoEncoder::Samples encodedSamples;
				Frames decodedFrames;

				std::vector<uint8_t> codecConfigData;

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

						Media::AVFoundation::VideoEncoder::Sample sample = videoEncoder.popSample();

						if (sample)
						{
							encodedSamples.push_back(std::move(sample));
						}
					}

					// Stop the encoder to flush all pending samples
					videoEncoder.stop();

					Timestamp timeoutTimestamp(true);

					while (!timeoutTimestamp.hasTimePassed(2.0))
					{
						Media::AVFoundation::VideoEncoder::Sample sample = videoEncoder.popSample();

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

				// Find the codec config sample and extract the data
				for (const Media::AVFoundation::VideoEncoder::Sample& sample : encodedSamples)
				{
					if (sample.isConfiguration())
					{
						codecConfigData = sample.data();
						break;
					}
				}

				// Initialize the decoder with codec config data
				if (!videoDecoder.initialize(mimeType, width, height, codecConfigData.data(), codecConfigData.size()))
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

				{
					// now, we decode the frames again (skip codec config samples)

					for (const Media::AVFoundation::VideoEncoder::Sample& sample : encodedSamples)
					{
						// Skip codec config samples - they're only used for decoder initialization
						if (sample.isConfiguration())
						{
							continue;
						}

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

Frame TestAVFoundation::createTestFrame(const unsigned int width, const unsigned int height, const unsigned int frameIndex)
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

bool TestAVFoundation::validateTestFrame(const Frame& testFrame, const Frame& decodedFrame)
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
