/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestSpecial.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/media/special/Image.h"
#include "ocean/media/special/ImageBmp.h"
#include "ocean/media/special/ImageNpy.h"
#include "ocean/media/special/ImageOcn.h"
#include "ocean/media/special/ImagePfm.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestSpecial::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Special test");

	Log::info() << " ";

	if (selector.shouldRun("bmpimagencodedecode"))
	{
		testResult = testBmpImageEncodeDecode(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("bmpdecodestresstest"))
	{
#ifdef OCEAN_DEBUG
		Log::info() << "Skipping BMP stress test in debug builds";
#else
		testResult = testBmpDecodeStressTest();
#endif

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("pfmimagencodedecode"))
	{
		testResult = testPfmImageEncodeDecode(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("pfmdecodestresstest"))
	{
#ifdef OCEAN_DEBUG
		Log::info() << "Skipping PFM stress test in debug builds";
#else
		testResult = testPfmDecodeStressTest();
#endif

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("npyimagencodedecode"))
	{
		testResult = testNpyImageEncodeDecode(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("npydecodesyntheticheaders"))
	{
		testResult = testNpyDecodeSyntheticHeaders();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("npydecodestresstest"))
	{
#ifdef OCEAN_DEBUG
		Log::info() << "Skipping NPY stress test in debug builds";
#else
		testResult = testNpyDecodeStressTest();
#endif

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ocnimagencodedecode"))
	{
		testResult = testOcnImageEncodeDecode(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ocndecodestresstest"))
	{
#ifdef OCEAN_DEBUG
		Log::info() << "Skipping OCN stress test in debug builds";
#else
		testResult = testOcnDecodeStressTest();
#endif

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("anyimagencodedecode"))
	{
		testResult = testAnyImageEncodeDecode(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("decodestresstest"))
	{
#ifdef OCEAN_DEBUG
		Log::info() << "Skipping any stress test in debug builds";
#else
		testResult = testDecodeStressTest();
#endif

		Log::info() << " ";
	}

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestSpecial, BmpImageBGRA32NotSupported)
{
	std::vector<uint8_t> buffer;
	EXPECT_FALSE(Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_BGRA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST(TestSpecial, BmpImageRGBA32NotSupported)
{
	std::vector<uint8_t> buffer;
	EXPECT_FALSE(Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST(TestSpecial, BmpImageYA16NotSupported)
{
	std::vector<uint8_t> buffer;
	EXPECT_FALSE(Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST(TestSpecial, BmpImageYUVA32NotSupported)
{
	std::vector<uint8_t> buffer;
	EXPECT_FALSE(Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YUVA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST(TestSpecial, BmpImageBGR24UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_BGR24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, BmpImageBGR24LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_BGR24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, BmpImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, BmpImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, BmpImageY8UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, BmpImageY8LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

#ifndef OCEAN_DEBUG
	TEST(TestSpecial, BmpDecodeStressTest)
	{
		EXPECT_TRUE(TestSpecial::testBmpDecodeStressTest());
	}
#endif


TEST(TestSpecial, PfmImageFloat1UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testPfmImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_F32, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, PfmImageFloat1LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testPfmImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_F32, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, PfmImageFloat3UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testPfmImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<float, 3u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, PfmImageFloat3LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testPfmImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<float, 3u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

#ifndef OCEAN_DEBUG
	TEST(TestSpecial, PfmDecodeStressTest)
	{
		EXPECT_TRUE(TestSpecial::testPfmDecodeStressTest());
	}
#endif


TEST(TestSpecial, NpyImageFloatUpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageFloatLowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageDoubleUpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<double, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageDoubleLowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<double, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt8UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt8LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int8_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt16UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt16LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt32UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt32LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt64UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageInt64LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<int64_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint8UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint8LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint16UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint16LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint16_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint32UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint32LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint64UpperLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyImageUint64LowerLeft)
{
	EXPECT_TRUE(TestSpecial::testNpyImageEncodeDecode(1920u, 1080u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST(TestSpecial, NpyDecodeSyntheticHeaders)
{
	EXPECT_TRUE(TestSpecial::testNpyDecodeSyntheticHeaders());
}

#ifndef OCEAN_DEBUG
	TEST(TestSpecial, NpyDecodeStressTest)
	{
		EXPECT_TRUE(TestSpecial::testNpyDecodeStressTest());
	}
#endif


TEST(TestSpecial, OcnImageEncodeDecode)
{
	EXPECT_TRUE(TestSpecial::testOcnImageEncodeDecode(GTEST_TEST_DURATION * 20.0));
}

#ifndef OCEAN_DEBUG
	TEST(TestSpecial, OcnDecodeStressTest)
	{
		EXPECT_TRUE(TestSpecial::testOcnDecodeStressTest());
	}
#endif


TEST(TestSpecial, AnyImageEncodeDecode)
{
	EXPECT_TRUE(TestSpecial::testAnyImageEncodeDecode(GTEST_TEST_DURATION));
}

#ifndef OCEAN_DEBUG
	TEST(TestSpecial, DecodeStressTest)
	{
		EXPECT_TRUE(TestSpecial::testDecodeStressTest());
	}
#endif

#endif // OCEAN_USE_GTEST

bool TestSpecial::testBmpImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BMP image encode/decode test:";

	Validation validation;

	// first we ensure that we cannot encode images with alpha channel

	std::vector<uint8_t> buffer;

	OCEAN_EXPECT_FALSE(validation, Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_BGRA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));

	OCEAN_EXPECT_FALSE(validation, Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));

	OCEAN_EXPECT_FALSE(validation, Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));

	OCEAN_EXPECT_FALSE(validation, Media::Special::ImageBmp::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YUVA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));

	// we use tiny images to ensure code correctness,
	// normal images for performance,
	// and extreme large images to identify memory leaks

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	const Indices32 widths = {1u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u};
	const Indices32 heights = {1u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u};
#else
	const Indices32 widths = {1u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u, 7680u};
	const Indices32 heights = {1u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u, 4320u};
#endif

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_BGR24,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_Y8
	};

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		Log::info() << " ";

		Log::info() << "Testing pixel format " << FrameType::translatePixelFormat(pixelFormat) << ":";

		for (size_t s = 0; s < widths.size(); ++s)
		{
			ocean_assert(widths.size() == heights.size());

			Log::info() << " ";

			for (const FrameType::PixelOrigin pixelOrigin : {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT})
			{
				OCEAN_EXPECT_TRUE(validation, testBmpImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration));
			}
		}

		Log::info() << " ";
	}

	Log::info() << "BMP image encode/decode test: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testPfmImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "PFM image encode/decode test:";

	Validation validation;

	// we use tiny images to ensure code correctness,
	// normal images for performance,
	// and extreme large images to identify memory leaks

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	const Indices32 widths = {1u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u};
	const Indices32 heights = {1u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u};
#else
	const Indices32 widths = {1u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u, 7680u};
	const Indices32 heights = {1u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u, 4320u};
#endif

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_F32,
		FrameType::genericPixelFormat<float, 3u>()
	};

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		Log::info() << " ";

		Log::info() << "Testing pixel format with " << FrameType::channels(pixelFormat) << " channels:";

		for (size_t s = 0; s < widths.size(); ++s)
		{
			ocean_assert(widths.size() == heights.size());

			Log::info() << " ";

			for (const FrameType::PixelOrigin pixelOrigin : {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT})
			{
				OCEAN_EXPECT_TRUE(validation, testPfmImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration));
			}
		}

		Log::info() << " ";
	}

	Log::info() << "PFM image encode/decode test: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testNpyImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "NPY image encode/decode test:";

	Validation validation;

	// we use tiny images to ensure code correctness,
	// normal images for performance,
	// and extreme large images to identify memory leaks

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	const Indices32 widths = {1u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u};
	const Indices32 heights = {1u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u};
#else
	const Indices32 widths = {1u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u, 7680u};
	const Indices32 heights = {1u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u, 4320u};
#endif

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint16_t, 1u>(),
		FrameType::genericPixelFormat<uint32_t, 1u>(),
		FrameType::genericPixelFormat<float, 1u>(),
		FrameType::genericPixelFormat<double, 1u>()
	};

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		Log::info() << " ";

		Log::info() << "Testing pixel format with " << FrameType::channels(pixelFormat) << " channels:";

		for (size_t s = 0; s < widths.size(); ++s)
		{
			ocean_assert(widths.size() == heights.size());

			Log::info() << " ";

			for (const FrameType::PixelOrigin pixelOrigin : {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT})
			{
				OCEAN_EXPECT_TRUE(validation, testNpyImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration));
			}
		}

		Log::info() << " ";
	}

	Log::info() << "NPY image encode/decode test: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testOcnImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "OCN image encode/decode test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const std::vector<FrameType::DataType> dataTypes =
	{
		FrameType::DT_UNSIGNED_INTEGER_8,
		FrameType::DT_SIGNED_INTEGER_8,
		FrameType::DT_UNSIGNED_INTEGER_16,
		FrameType::DT_SIGNED_INTEGER_16,
		FrameType::DT_UNSIGNED_INTEGER_32,
		FrameType::DT_SIGNED_INTEGER_32,
		FrameType::DT_UNSIGNED_INTEGER_64,
		FrameType::DT_SIGNED_INTEGER_64,
		FrameType::DT_SIGNED_FLOAT_16,
		FrameType::DT_SIGNED_FLOAT_32,
		FrameType::DT_SIGNED_FLOAT_64
	};

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, FrameType::definedPixelFormats());

		if (RandomI::boolean(randomGenerator))
		{
			const FrameType::DataType dataType = RandomI::random(randomGenerator, dataTypes);
			ocean_assert(dataType != FrameType::DT_UNDEFINED && dataType < FrameType::DT_END);

			const unsigned int channels = RandomI::random(randomGenerator, 1u, 6u);

			pixelFormat = FrameType::genericPixelFormat(dataType, channels);
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 3840u / widthMultiple) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 3840u / heightMultiple) * heightMultiple;

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);

		if (!frameType.isValid())
		{
			ocean_assert(false && "This should never happen!");
			OCEAN_SET_FAILED(validation);
		}

		Indices32 planePaddingElements;

		if (RandomI::boolean(randomGenerator))
		{
			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				const unsigned int paddingMultiplier = RandomI::random(randomGenerator, 1u);
				planePaddingElements.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * paddingMultiplier);
			}
		}

		Frame sourceFrame(frameType, planePaddingElements);
		ocean_assert(sourceFrame);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		std::vector<uint8_t> buffer;
		const bool compressionSucceeded = Media::Special::ImageOcn::encodeImage(sourceFrame, buffer);

		unsigned int correctRows = 0u;

		if (compressionSucceeded)
		{
			const Frame targetFrame = Media::Special::ImageOcn::decodeImage(buffer.data(), buffer.size());

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determined the similarity directly

					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						if (memcmp(sourceFrame.constrow<void>(y), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
						{
							++correctRows;
						}
					}
				}
			}
		}

		OCEAN_EXPECT_EQUAL(validation, correctRows, sourceFrame.height());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testAnyImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Any image encode/decode test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Strings encoderTypes =
	{
		"bmp",
		"ocn"
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int paddingMultiplier = RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * paddingMultiplier;

		Frame sourceFrame(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		ocean_assert(sourceFrame);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
		CV::FrameFilterGaussian::filter(sourceFrame, 7u, sourceFrame.pixels() >= 50u * 50u ? WorkerPool::get().scopedWorker()() : nullptr);

		for (const std::string& encoderType : encoderTypes)
		{
			std::vector<uint8_t> buffer;
			OCEAN_EXPECT_TRUE(validation, Media::Special::Image::encodeImage(sourceFrame, encoderType, buffer, true));

			std::string decoderTypeExplicit;
			const Frame targetFrameExplicit = Media::Special::Image::decodeImage(buffer.data(), buffer.size(), encoderType, &decoderTypeExplicit);

			if (!targetFrameExplicit.isValid() || encoderType != decoderTypeExplicit)
			{
				OCEAN_SET_FAILED(validation);
			}
			else
			{
				Frame convertedFrame;
				if (!CV::FrameConverter::Comfort::convert(targetFrameExplicit, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, false))
				{
					OCEAN_SET_FAILED(validation);
				}
				else
				{
					double minDifference, aveDifference, maxDifference;
					if (!determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10) // quite generous
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}

			std::string decoderTypeImplicit;
			const Frame targetFrameImplicit = Media::Special::Image::decodeImage(buffer.data(), buffer.size(), "", &decoderTypeImplicit);

			if (!targetFrameImplicit.isValid() || encoderType != decoderTypeImplicit)
			{
				OCEAN_SET_FAILED(validation);
			}
			else
			{
				Frame convertedFrame;
				if (!CV::FrameConverter::Comfort::convert(targetFrameImplicit, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, false))
				{
					OCEAN_SET_FAILED(validation);
				}
				else
				{
					double minDifference, aveDifference, maxDifference;
					if (!determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10) // quite generous
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testBmpImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		const unsigned int paddingMultiplier = RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * paddingMultiplier;

		Frame sourceFrame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);
		ocean_assert(sourceFrame);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		unsigned int correctRows = 0u;

		// bmp supports BGR24 only
		const bool allowConversion = pixelFormat != FrameType::FORMAT_BGR24;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::Special::ImageBmp::encodeImage(sourceFrame, buffer, allowConversion, nullptr);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::Special::ImageBmp::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determined the similarity directly

					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						if (memcmp(sourceFrame.constrow<void>(y), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
						{
							++correctRows;
						}
					}
				}
				else if (sourceFrame.pixelFormat() != FrameType::FORMAT_BGR24)
				{
					// we convert the pixel format from BGR24 to match the pixel format of the source

					Frame convertedFrame;
					if (CV::FrameConverter::Comfort::convert(targetFrame, sourceFrame.pixelFormat(), convertedFrame, false))
					{
						ocean_assert(convertedFrame.frameType() == sourceFrame.frameType());

						for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
						{
							if (memcmp(sourceFrame.constrow<void>(y), convertedFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
							{
								++correctRows;
							}
						}
					}
				}
			}
		}

		OCEAN_EXPECT_EQUAL(validation, correctRows, sourceFrame.height());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testBmpDecodeStressTest()
{
	Log::info() << "BMP decode stress test:";

	constexpr double testDuration = 60.0;

	constexpr unsigned int maximalDataSize = 5000u;

	unsigned int dummyValue = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> data;

		if (RandomI::random(randomGenerator, 10u) == 0u)
		{
			// any memory

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 0u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}
		}
		else
		{
			// memory starting with 'MB'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 2u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 'M';
			data[1] = 'B';
		}

		const Frame frame = Media::Special::ImageBmp::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (dummyValue % 2u == 0u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	return true;
}

bool TestSpecial::testPfmImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		const unsigned int paddingMultiplier = RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * paddingMultiplier;

		Frame sourceFrame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);
		ocean_assert(sourceFrame);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		unsigned int correctRows = 0u;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::Special::ImagePfm::encodeImage(sourceFrame, buffer);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::Special::ImagePfm::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determined the similarity directly

					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						if (memcmp(sourceFrame.constrow<void>(y), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
						{
							++correctRows;
						}
					}
				}
				else if (FrameType(sourceFrame.frameType(), FrameType::ORIGIN_LOWER_LEFT) == targetFrame.frameType())
				{
					// we can determined the similarity based on the flipped image

					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						if (memcmp(sourceFrame.constrow<void>(sourceFrame.height() - y - 1u), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
						{
							++correctRows;
						}
					}
				}
			}
		}

		OCEAN_EXPECT_EQUAL(validation, correctRows, sourceFrame.height());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testPfmDecodeStressTest()
{
	Log::info() << "PFM decode stress test:";

	constexpr double testDuration = 60.0;

	constexpr unsigned int maximalDataSize = 5000u;

	unsigned int dummyValue = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> data;

		if (RandomI::random(randomGenerator, 10u) == 0u)
		{
			// any memory

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 0u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}
		}
		else
		{
			// memory starting with 'PF\n' or 'Pf\n'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 3u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			if (RandomI::boolean(randomGenerator))
			{
				data[0] = 'P';
				data[1] = 'F';
				data[2] = '\n';
			}
			else
			{
				data[0] = 'P';
				data[1] = 'f';
				data[2] = '\n';
			}
		}

		const Frame frame = Media::Special::ImagePfm::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (dummyValue % 2u == 0u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	return true;
}

bool TestSpecial::testNpyImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		const unsigned int paddingMultiplier = RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * paddingMultiplier;

		Frame sourceFrame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);
		ocean_assert(sourceFrame);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		unsigned int correctRows = 0u;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::Special::ImageNpy::encodeImage(sourceFrame, buffer);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (FrameType::areFrameTypesCompatible(sourceFrame.frameType(), targetFrame.frameType(), true))
				{
					if (sourceFrame.pixelOrigin() == targetFrame.pixelOrigin())
					{
						// we can determined the similarity directly

						for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
						{
							if (memcmp(sourceFrame.constrow<void>(y), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
							{
								++correctRows;
							}
						}
					}
					else
					{
						// we can determined the similarity based on the flipped image

						for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
						{
							if (memcmp(sourceFrame.constrow<void>(sourceFrame.height() - y - 1u), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
							{
								++correctRows;
							}
						}
					}
				}
			}
		}

		OCEAN_EXPECT_EQUAL(validation, correctRows, sourceFrame.height());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

namespace
{

/**
 * Creates an NPY buffer from a header dictionary and a payload.
 * @param dictionary The header dictionary, without the trailing newline
 * @param payload The payload to append, can be empty
 * @param payloadSize The size of the payload, in bytes
 * @return The resulting buffer
 */
std::vector<uint8_t> createNpyBuffer(const std::string& dictionary, const void* payload, const size_t payloadSize)
{
	const std::string header = dictionary + "\n";

	std::vector<uint8_t> buffer = {0x93u, 'N', 'U', 'M', 'P', 'Y', 0x01u, 0x00u};

	const uint16_t headerLength = uint16_t(header.size());
	buffer.resize(buffer.size() + sizeof(headerLength));
	memcpy(buffer.data() + buffer.size() - sizeof(headerLength), &headerLength, sizeof(headerLength));

	buffer.insert(buffer.cend(), header.cbegin(), header.cend());

	if (payloadSize != 0)
	{
		const uint8_t* payloadData = (const uint8_t*)(payload);
		buffer.insert(buffer.cend(), payloadData, payloadData + payloadSize);
	}

	return buffer;
}

}

bool TestSpecial::testNpyDecodeSyntheticHeaders()
{
	Log::info() << "NPY decode with synthetic headers test:";

	Validation validation;

	// the encoder writes 'fortran_order': False only, so the column-major branch needs a hand-written header
	// shape is (rows, columns), a column-major payload stores element (r, c) at offset c * rows + r

	{
		// row-major reference, 1 2 3 / 4 5 6

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u1', 'fortran_order': False, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.width(), 3u);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), 2u);

			const uint8_t expected[2][3] = {{1u, 2u, 3u}, {4u, 5u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major, non-square, 1 3 5 / 2 4 6

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u1', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.width(), 3u);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), 2u);

			const uint8_t expected[2][3] = {{1u, 3u, 5u}, {2u, 4u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major, the transposed shape, 1 4 / 2 5 / 3 6

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u1', 'fortran_order': True, 'shape': (3, 2), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.width(), 2u);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), 3u);

			const uint8_t expected[3][2] = {{1u, 4u}, {2u, 5u}, {3u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major and square, identical before and after the transpose fix

		const uint8_t payload[4] = {1u, 2u, 3u, 4u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u1', 'fortran_order': True, 'shape': (2, 2), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			const uint8_t expected[2][2] = {{1u, 3u}, {2u, 4u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major with a 16 bit type, the decoder has one switch case per data type size

		const uint16_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u2', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			const uint16_t expected[2][3] = {{1u, 3u, 5u}, {2u, 4u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint16_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major with a 32 bit type

		const uint32_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u4', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			const uint32_t expected[2][3] = {{1u, 3u, 5u}, {2u, 4u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint32_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major with a 64 bit type

		const uint64_t payload[6] = {1ull, 2ull, 3ull, 4ull, 5ull, 6ull};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u8', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			const uint64_t expected[2][3] = {{1ull, 3ull, 5ull}, {2ull, 4ull, 6ull}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint64_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// column-major crossing the transposer's 8x8 block boundary, element (r, c) is at c * rows + r

		constexpr unsigned int rows = 10u;
		constexpr unsigned int columns = 17u;

		std::vector<uint8_t> payload(rows * columns);

		for (size_t n = 0; n < payload.size(); ++n)
		{
			payload[n] = uint8_t(n);
		}

		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u1', 'fortran_order': True, 'shape': (10, 17), }", payload.data(), payload.size());

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.width(), columns);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), rows);

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(y)[x], uint8_t(x * rows + y));
				}
			}
		}
	}

	{
		// the dtype must be located relative to 'descr', not at a fixed offset

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'fortran_order': False, 'descr': '<u1', 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.pixelFormat(), FrameType::FORMAT_Y8);
			OCEAN_EXPECT_EQUAL(validation, frame.width(), 3u);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), 2u);
		}
	}

	{
		// an earlier key must not be mistaken for the dtype

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'abcde': '<f4', 'descr': '<u1', 'fortran_order': False, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.pixelFormat(), FrameType::FORMAT_Y8);
		}
	}

	{
		// an unknown dtype must be rejected

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'fortran_order': False, 'descr': '<zz', 'shape': (2, 3), }", payload, sizeof(payload));

		OCEAN_EXPECT_FALSE(validation, Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size()).isValid());
	}

	{
		// numpy writes '|' as the byte order of any single byte type, so this is what np.save() of a uint8 array produces

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '|u1', 'fortran_order': False, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.pixelFormat(), FrameType::FORMAT_Y8);
			OCEAN_EXPECT_EQUAL(validation, frame.width(), 3u);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), 2u);

			const uint8_t expected[2][3] = {{1u, 2u, 3u}, {4u, 5u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// the same for a signed single byte type, and combined with the column-major layout

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '|i1', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		OCEAN_EXPECT_TRUE(validation, Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size()).isValid());
	}

	{
		// big endian float data must be byte swapped while decoding

		const float values[6] = {1.5f, -2.25f, 0.0f, 1024.5f, -0.125f, 3.0f};

		uint8_t payload[sizeof(values)];

		for (size_t n = 0; n < 6; ++n)
		{
			uint8_t littleEndian[4];
			memcpy(littleEndian, values + n, 4);

			for (size_t nByte = 0; nByte < 4; ++nByte)
			{
				payload[n * 4 + nByte] = littleEndian[3 - nByte];
			}
		}

		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '>f4', 'fortran_order': False, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.pixelFormat(), FrameType::FORMAT_F32);

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<float>(y)[x], values[y * frame.width() + x]);
				}
			}
		}
	}

	{
		// a big endian 16 bit image, also in the column-major layout

		const uint16_t values[6] = {1u, 2u, 3u, 4u, 5u, 6u};

		uint8_t payload[sizeof(values)];

		for (size_t n = 0; n < 6; ++n)
		{
			payload[n * 2 + 0] = uint8_t(values[n] >> 8u);
			payload[n * 2 + 1] = uint8_t(values[n] & 0xFFu);
		}

		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '>u2', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			// column-major, so element (r, c) is at c * rows + r

			const uint16_t expected[2][3] = {{1u, 3u, 5u}, {2u, 4u, 6u}};

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint16_t>(y)[x], expected[y][x]);
				}
			}
		}
	}

	{
		// a big endian single byte type needs no swap and must decode unchanged

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '>u1', 'fortran_order': False, 'shape': (2, 3), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(0u)[0], uint8_t(1u));
			OCEAN_EXPECT_EQUAL(validation, frame.constrow<uint8_t>(1u)[2], uint8_t(6u));
		}
	}

	{
		// a three dimensional shape, e.g. an RGB numpy array, must be rejected rather than silently decoded as one channel

		const uint8_t payload[18] = {};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '|u1', 'fortran_order': False, 'shape': (2, 3, 3), }", payload, sizeof(payload));

		OCEAN_EXPECT_FALSE(validation, Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size()).isValid());
	}

	{
		// a shape with a space before the closing bracket must still be accepted

		const uint8_t payload[6] = {1u, 2u, 3u, 4u, 5u, 6u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '|u1', 'fortran_order': False, 'shape': (2, 3 ), }", payload, sizeof(payload));

		const Frame frame = Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size());

		OCEAN_EXPECT_TRUE(validation, frame.isValid());

		if (frame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, frame.width(), 3u);
			OCEAN_EXPECT_EQUAL(validation, frame.height(), 2u);
		}
	}

	{
		// a payload smaller than the shape must be rejected

		const uint8_t payload[2] = {1u, 2u};
		const std::vector<uint8_t> buffer = createNpyBuffer("{'descr': '<u1', 'fortran_order': True, 'shape': (2, 3), }", payload, sizeof(payload));

		OCEAN_EXPECT_FALSE(validation, Media::Special::ImageNpy::decodeImage(buffer.data(), buffer.size()).isValid());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpecial::testNpyDecodeStressTest()
{
	Log::info() << "NPY decode stress test:";

	constexpr double testDuration = 60.0;

	constexpr unsigned int maximalDataSize = 5000u;

	unsigned int dummyValue = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> data;

		if (RandomI::random(randomGenerator, 10u) == 0u)
		{
			// any memory

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 0u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}
		}
		else
		{
			// memory starting with '0x93NUMPY''

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 6u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 0x93;
			data[1] = 'N';
			data[2] = 'U';
			data[3] = 'M';
			data[4] = 'P';
			data[5] = 'Y';
		}

		const Frame frame = Media::Special::ImageNpy::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (dummyValue % 2u == 0u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	return true;
}

bool TestSpecial::testOcnDecodeStressTest()
{
	Log::info() << "OCN decode stress test:";

	constexpr double testDuration = 60.0;

	constexpr unsigned int maximalDataSize = 5000u;

	unsigned int dummyValue = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> data;

		if (RandomI::random(randomGenerator, 10u) == 0u)
		{
			// any memory

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 0u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}
		}
		else
		{
			// memory starting with 'ocn\0'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 4u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 'o';
			data[1] = 'c';
			data[2] = 'n';
			data[3] = '\0';
		}

		const Frame frame = Media::Special::ImageOcn::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (dummyValue % 2u == 0u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	return true;
}

bool TestSpecial::testDecodeStressTest()
{
	Log::info() << "Any decode stress test:";

	constexpr double testDuration = 60.0;

	constexpr unsigned int maximalDataSize = 5000u;

	unsigned int dummyValue = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> data(RandomI::random(randomGenerator, 0u, maximalDataSize));

		for (uint8_t& value : data)
		{
			value = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		std::string imageBufferType;

		if (RandomI::random(randomGenerator, 10u) == 0u)
		{
			imageBufferType = std::string(RandomI::random(randomGenerator, 1u, 4u), ' ');

			for (char& value : imageBufferType)
			{
				value = char(RandomI::random(randomGenerator, 255u));
			}
		}

		const Frame frame = Media::Special::Image::decodeImage(data.data(), data.size(), imageBufferType);

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (dummyValue % 2u == 0u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	return true;
}

bool TestSpecial::determineSimilarity(const Frame& firstFrame, const Frame& secondFrame, double& minDifference, double& aveDifference, double& maxDifference)
{
	if (!FrameType::areFrameTypesCompatible(firstFrame, secondFrame, true) || firstFrame.numberPlanes() != 1u)
	{
		ocean_assert(false && "Invalid frames!");
		return false;
	}

	Frame convertedSecondFrame;
	if (!CV::FrameConverter::Comfort::convert(secondFrame, firstFrame.pixelFormat(), firstFrame.pixelOrigin(), convertedSecondFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		return false;
	}

	switch (firstFrame.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
			determineSimilarity(firstFrame.constdata<uint8_t>(), convertedSecondFrame.constdata<uint8_t>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_16:
			determineSimilarity(firstFrame.constdata<uint16_t>(), convertedSecondFrame.constdata<uint16_t>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_32:
			determineSimilarity(firstFrame.constdata<uint32_t>(), convertedSecondFrame.constdata<uint32_t>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		case FrameType::DT_SIGNED_FLOAT_32:
			determineSimilarity(firstFrame.constdata<float>(), convertedSecondFrame.constdata<float>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		case FrameType::DT_SIGNED_FLOAT_64:
			determineSimilarity(firstFrame.constdata<double>(), convertedSecondFrame.constdata<double>(), firstFrame.width(), firstFrame.height(), firstFrame.channels(), firstFrame.paddingElements(), secondFrame.paddingElements(), minDifference, aveDifference, maxDifference);
			return true;

		default:
			ocean_assert(false && "Not support pixel format!");
			return false;
	}
}

template <typename T>
void TestSpecial::determineSimilarity(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements, double& minDifference, double& aveDifference, double& maxDifference)
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
