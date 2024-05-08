/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestAVFoundation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/avfoundation/PixelBufferAccessor.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestAVFoundation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "AVFoundation test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPixelBufferAccessorGenericPixelFormats(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPixelBufferAccessorNonGenericPixelFormats(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire AVFoundation test succeeded.";
	}
	else
	{
		Log::info() << "AVFoundation test FAILED!";
	}

	return allSucceeded;
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

#endif // #ifdef OCEAN_USE_GTESTkCVPixelFormatType_32ARGB

bool TestAVFoundation::testPixelBufferAccessorGenericPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel buffer accessor with generic pixel formats test:";

	bool allSucceeded = true;

	// an default accessor object should be invalid

	const Media::AVFoundation::PixelBufferAccessor invalidAccessor;
	if (invalidAccessor)
	{
		allSucceeded = false;
	}

	RandomGenerator randomGenerator;

	typedef std::pair<FrameType::PixelFormat, OSType> PixelFormatPair;

	const std::vector<PixelFormatPair> pixelFormatPairs =
	{
		std::make_pair(FrameType::FORMAT_ARGB32, kCVPixelFormatType_32ARGB),
		std::make_pair(FrameType::FORMAT_BGRA32, kCVPixelFormatType_32BGRA),
		std::make_pair(FrameType::FORMAT_RGB24, kCVPixelFormatType_24RGB),
		std::make_pair(FrameType::FORMAT_Y8, kCVPixelFormatType_OneComponent8)
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const PixelFormatPair pixelFormatPair = pixelFormatPairs[RandomI::random(randomGenerator, 0u, (unsigned int)pixelFormatPairs.size() - 1u)];

		const FrameType::PixelFormat pixelFormat = pixelFormatPair.first;
		const OSType osPixelFormat = pixelFormatPair.second;

		CVPixelBufferRef pixelBuffer = nullptr;
		const CVReturn createResult = CVPixelBufferCreate(kCFAllocatorDefault, size_t(width), size_t(height), osPixelFormat, nullptr, &pixelBuffer);

		if (createResult != kCVReturnSuccess)
		{
			ocean_assert(false && "This must never happen!");
			allSucceeded = false;
			break;
		}

		std::vector<std::vector<uint8_t>> memoryCopy;
		Indices32 memoryCopyHeights;
		Indices32 memoryCopyStrideBytes;
		if (!writeRandomDataToPixelBuffer(pixelBuffer, 1, randomGenerator, memoryCopy, memoryCopyHeights, memoryCopyStrideBytes))
		{
			ocean_assert(false && "This must never happen!");
			allSucceeded = false;
			break;
		}

		const bool readOnly = RandomI::random(1u) == 0u;

		Media::AVFoundation::PixelBufferAccessor accessor(pixelBuffer, readOnly);

		if (accessor)
		{
			const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

			if (readOnly)
			{
				const Frame& frame = accessor.frame();

				if (frame.frameType() != frameType || frame.isReadOnly() != readOnly)
				{
					allSucceeded = false;
				}
			}
			else
			{
				Frame& frame = accessor.frame();

				if (frame.frameType() != frameType || frame.isReadOnly() != readOnly)
				{
					allSucceeded = false;
				}
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (allSucceeded)
		{
			// ensuring that the memory data is actually correct

			const Frame& frame = accessor.frame();

			const Frame testFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

			for (unsigned int y = 0u; y < testFrame.height(); ++y)
			{
				const uint8_t* frameData = frame.constrow<uint8_t>(y);
				const uint8_t* testFrameData = testFrame.constrow<uint8_t>(y);

				// we ensure that the padding area is identical as well
				if (memcmp(frameData, testFrameData, frame.strideBytes()) != 0)
				{
					allSucceeded = false;
				}
			}
		}

		accessor.release();

		CVPixelBufferRelease(pixelBuffer);
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

bool TestAVFoundation::testPixelBufferAccessorNonGenericPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel buffer accessor with non-generic pixel formats test:";

	bool allSucceeded = true;

	// a default accessor object should be invalid

	const Media::AVFoundation::PixelBufferAccessor invalidAccessor;
	if (invalidAccessor)
	{
		allSucceeded = false;
	}

	RandomGenerator randomGenerator;

	struct PixelFormatSpecifier
	{
		FrameType::PixelFormat pixelFormat;
		OSType osPixelFormat;
		size_t numberPlanes;
	};

	const std::vector<PixelFormatSpecifier> pixelFormatSpecifiers =
	{
		{FrameType::FORMAT_Y_UV12, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, 2},
		{FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, kCVPixelFormatType_420YpCbCr8Planar, 3},
		{FrameType::FORMAT_Y_U_V12_FULL_RANGE, kCVPixelFormatType_420YpCbCr8PlanarFullRange, 3}
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 2u, 1920u) & 0xFFFFFFFEu;
		const unsigned int height = RandomI::random(randomGenerator, 2u, 1080u) & 0xFFFFFFFEu;
		ocean_assert(width % 2u == 0u && height % 2 == 0u);

		const PixelFormatSpecifier pixelFormatSpecifier = pixelFormatSpecifiers[RandomI::random(randomGenerator, 0u, (unsigned int)pixelFormatSpecifiers.size() - 1u)];

		CVPixelBufferRef pixelBuffer = nullptr;
		const CVReturn createResult = CVPixelBufferCreate(kCFAllocatorDefault, size_t(width), size_t(height), pixelFormatSpecifier.osPixelFormat, nullptr, &pixelBuffer);

		if (createResult != kCVReturnSuccess)
		{
			ocean_assert(false && "This must never happen!");
			allSucceeded = false;
			break;
		}

		std::vector<std::vector<uint8_t>> memoryCopy;
		Indices32 memoryCopyHeights;
		Indices32 memoryCopyStrideBytes;
		if (!writeRandomDataToPixelBuffer(pixelBuffer, pixelFormatSpecifier.numberPlanes, randomGenerator, memoryCopy, memoryCopyHeights, memoryCopyStrideBytes))
		{
			ocean_assert(false && "This must never happen!");
			allSucceeded = false;
			break;
		}

		const bool readOnly = RandomI::random(1u) == 0u;
		const bool accessYPlaneOnly = RandomI::random(1u) == 0u;

		Media::AVFoundation::PixelBufferAccessor accessor(pixelBuffer, readOnly, accessYPlaneOnly);

		if (accessor)
		{
			const FrameType::PixelFormat pixelFormat = accessYPlaneOnly ? FrameType::FORMAT_Y8 : pixelFormatSpecifier.pixelFormat;
			const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

			Frame& frame = accessor.frame();
			ocean_assert(frame.isValid());

			if (frame.isReadOnly() != readOnly)
			{
				allSucceeded = false;
			}
			if (pixelFormat != frameType.pixelFormat())
			{
				allSucceeded = false;
			}

			const size_t numberPlanesToCheck = accessYPlaneOnly ? 1 : memoryCopy.size();

			for (size_t nPlane = 0; nPlane < numberPlanesToCheck; ++nPlane)
			{
				if (nPlane >= frame.planes().size())
				{
					allSucceeded = false;
					break;
				}

				const unsigned int strideBytes = memoryCopyStrideBytes[nPlane];

				if (frame.planes()[nPlane].strideBytes() != strideBytes)
				{
					allSucceeded = false;
					break;
				}

				const uint8_t* memoryCopyData = memoryCopy[nPlane].data();
				const uint8_t* frameData = frame.planes()[nPlane].constdata<uint8_t>();

				for (unsigned int y = 0u; y < memoryCopyHeights[nPlane]; ++y)
				{
					if (memcmp(frameData, memoryCopyData, strideBytes) != 0)
					{
						allSucceeded = false;
					}

					frameData += strideBytes;
					memoryCopyData += strideBytes;
				}
			}
		}

		accessor.release();

		CVPixelBufferRelease(pixelBuffer);
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

}

}

}
