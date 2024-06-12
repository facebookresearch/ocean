/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestOpenImageLibraries.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/media/BufferImage.h"
#include "ocean/media/BufferImageRecorder.h"
#include "ocean/media/Manager.h"

#include "ocean/media/openimagelibraries/Image.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	#include "ocean/media/openimagelibraries/ImageJpg.h"
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	#include "ocean/media/openimagelibraries/ImagePng.h"
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF
	#include "ocean/media/openimagelibraries/ImageTif.h"
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF
	#include "ocean/media/openimagelibraries/ImageGif.h"
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP
	#include "ocean/media/openimagelibraries/ImageWebp.h"
#endif

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestOpenImageLibraries::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_RUNTIME_STATIC
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
#endif

	Log::info() << "OpenImageLibraries test:";
	Log::info() << " ";

	bool allSucceeded = true;

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	allSucceeded = testJpgImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	#ifdef OCEAN_DEBUG
		Log::info() << "Skipping JPG stress test in debug builds";
	#else
		allSucceeded = testJpgDecodeStressTest() && allSucceeded;
	#endif
#else
	Log::info() << "Skipping JPG as not supported on this platforms.";
#endif

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	allSucceeded = testPngImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	#ifdef OCEAN_DEBUG
		Log::info() << "Skipping PNG stress test in debug builds";
	#else
		allSucceeded = testPngDecodeStressTest() && allSucceeded;
	#endif
#else
	Log::info() << "Skipping PNG as not supported on this platforms.";
#endif

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF
	allSucceeded = testTifImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	#ifdef OCEAN_DEBUG
		Log::info() << "Skipping TIF stress test in debug builds";
	#else
		allSucceeded = testTifDecodeStressTest() && allSucceeded;
	#endif
#else
	Log::info() << "Skipping TIF as not supported on this platforms.";
#endif

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF
	#ifdef OCEAN_DEBUG
		Log::info() << "Skipping GIF stress test in debug builds";
	#else
		allSucceeded = testGifDecodeStressTest() && allSucceeded;
	#endif
#else
	Log::info() << "Skipping GIF as not supported on this platforms.";
#endif

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP
	allSucceeded = testWebpImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	#ifdef OCEAN_DEBUG
		Log::info() << "Skipping WEBP stress test in debug builds";
	#else
		allSucceeded = testWebpDecodeStressTest() && allSucceeded;
	#endif
#else
	Log::info() << "Skipping WEBP as not supported on this platforms.";
#endif

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAnyImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

#ifdef OCEAN_DEBUG
	Log::info() << "Skipping any stress test in debug builds";
#else
	allSucceeded = testDecodeStressTest() && allSucceeded;
#endif

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire OpenImageLibraries test succeeded.";
	}
	else
	{
		Log::info() << "OpenImageLibraries test FAILED!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
#endif

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

/**
 * This class implements a simple instance for the GTest ensuring that
 * the OpenImageLibrary plugin is registered during startup and unregistered before shutdown.
 */
class TestOpenImageLibrariesGTestInstance : public ::testing::Test
{
	protected:

		/**
		 * Default constructor.
		 */
  		TestOpenImageLibrariesGTestInstance()
  		{
  			// nothing to do here
		}

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
#endif
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
#endif
		}
};

TEST_F(TestOpenImageLibrariesGTestInstance, AllFileFormats)
{
	// we ensure that all file formats are tested

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	const bool jpgEnabled = true;
#else
	const bool jpgEnabled = false;
#endif

	EXPECT_TRUE(jpgEnabled);


#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	const bool pngEnabled = true;
#else
	const bool pngEnabled = false;
#endif

	EXPECT_TRUE(pngEnabled);


#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF
	const bool gifEnabled = true;
#else
	const bool gifEnabled = false;
#endif

	EXPECT_TRUE(gifEnabled);
}


#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageY8UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageY8LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageYUV24UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YUV24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageYUV24LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YUV24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageBGRA32UpperLeft)
{
	std::vector<uint8_t> buffer;
	EXPECT_TRUE(Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_BGRA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageRGBA32AUpperLeft)
{
	std::vector<uint8_t> buffer;
	EXPECT_TRUE(Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageYA16UpperLeft)
{
	std::vector<uint8_t> buffer;
	EXPECT_TRUE(Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageYUVA32UpperLeft)
{
	std::vector<uint8_t> buffer;
	EXPECT_TRUE(Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YUVA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true));
}

TEST_F(TestOpenImageLibrariesGTestInstance, JpgImageRGB24Recorder)
{
	EXPECT_TRUE(TestOpenImageLibraries::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "jpg", 10.0));
}

#ifndef OCEAN_DEBUG
	TEST_F(TestOpenImageLibrariesGTestInstance, JpgDecodeStressTest)
	{
		EXPECT_TRUE(TestOpenImageLibraries::testJpgDecodeStressTest());
	}
#endif

#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageY8UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageY8LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageY16UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y16, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageY16LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y16, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageYA16UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageYA16LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YA16, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageRGBA32UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageRGBA32LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, PngImageRGBA32Recorder)
{
	EXPECT_TRUE(TestOpenImageLibraries::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT), "png", 0.0));
}

#ifndef OCEAN_DEBUG
	TEST_F(TestOpenImageLibrariesGTestInstance, PngDecodeStressTest)
	{
		EXPECT_TRUE(TestOpenImageLibraries::testPngDecodeStressTest());
	}
#endif

#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF

TEST_F(TestOpenImageLibrariesGTestInstance, TifImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, TifImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, TifImageRGBA32UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, TifImageRGBA32LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, TifImageRGBA32Recorder)
{
	EXPECT_TRUE(TestOpenImageLibraries::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT), "tif", 0.0));
}

#ifndef OCEAN_DEBUG
	TEST_F(TestOpenImageLibrariesGTestInstance, TifDecodeStressTest)
	{
		EXPECT_TRUE(TestOpenImageLibraries::testTifDecodeStressTest());
	}
#endif

#endif // OCEAN_MEDIA_OIL_SUPPORT_TIF


#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF

#ifndef OCEAN_DEBUG
	TEST_F(TestOpenImageLibrariesGTestInstance, GifDecodeStressTest)
	{
		EXPECT_TRUE(TestOpenImageLibraries::testGifDecodeStressTest());
	}
#endif

#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF


#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP

TEST_F(TestOpenImageLibrariesGTestInstance, WebpImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testWebpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, WebpImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testWebpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, WebpImageBGR24UpperLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testWebpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_BGR24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, WebpImageBGR24LowerLeft)
{
	EXPECT_TRUE(TestOpenImageLibraries::testWebpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_BGR24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestOpenImageLibrariesGTestInstance, WebpImageRGB24Recorder)
{
	EXPECT_TRUE(TestOpenImageLibraries::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "webp", 0.0));
}

#ifndef OCEAN_DEBUG
	TEST_F(TestOpenImageLibrariesGTestInstance, WebpDecodeStressTest)
	{
		EXPECT_TRUE(TestOpenImageLibraries::testWebpDecodeStressTest());
	}
#endif

#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP


TEST_F(TestOpenImageLibrariesGTestInstance, AnyImageEncodeDecode)
{
	EXPECT_TRUE(TestOpenImageLibraries::testAnyImageEncodeDecode(GTEST_TEST_DURATION));
}


TEST_F(TestOpenImageLibrariesGTestInstance, DecodeStressTest)
{
	EXPECT_TRUE(TestOpenImageLibraries::testDecodeStressTest());
}

#endif // OCEAN_USE_GTEST

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG

bool TestOpenImageLibraries::testJpgImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "JPEG image encode/decode test:";

	bool allSucceeded = true;

	// first we ensure that we cannot encode images with alpha channel

	std::vector<uint8_t> buffer;

	if (Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_BGRA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, false))
	{
		allSucceeded = false;
	}

	if (!Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_BGRA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true))
	{
		allSucceeded = false;
	}

	if (Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, false))
	{
		allSucceeded = false;
	}

	if (!Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true))
	{
		allSucceeded = false;
	}

	if (Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT)), buffer, false))
	{
		allSucceeded = false;
	}

	if (!Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT)), buffer, true))
	{
		allSucceeded = false;
	}

	if (Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YUVA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, false))
	{
		allSucceeded = false;
	}

	if (!Media::OpenImageLibraries::ImageJpg::encodeImage(Frame(FrameType(128u, 128u, FrameType::FORMAT_YUVA32, FrameType::ORIGIN_UPPER_LEFT)), buffer, true))
	{
		allSucceeded = false;
	}

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
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_YUV24
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
				if (!testJpgImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration))
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << " ";
	}

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "jpg", 10.0))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "JPEG image encode/decode test succeeded.";
	}
	else
	{
		Log::info() << "JPEG image encode/decode test FAILED!";
	}

	return allSucceeded;
}

#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG

bool TestOpenImageLibraries::testPngImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "PNG image encode/decode test:";

	bool allSucceeded = true;

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
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y16,
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_RGBA64
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
				if (!testPngImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration))
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << " ";
	}

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT), "png", 0.0))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "PNG image encode/decode test succeeded.";
	}
	else
	{
		Log::info() << "PNG image encode/decode test FAILED!";
	}

	return allSucceeded;
}

#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF

bool TestOpenImageLibraries::testTifImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "TIFF image encode/decode test:";

	bool allSucceeded = true;

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
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32
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
				if (!testTifImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration))
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << " ";
	}

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT), "tif", 0.0))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "TIFF image encode/decode test succeeded.";
	}
	else
	{
		Log::info() << "TIFF image encode/decode test FAILED!";
	}

	return allSucceeded;
}

#endif // OCEAN_MEDIA_OIL_SUPPORT_TIF

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP

bool TestOpenImageLibraries::testWebpImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "WEBP image encode/decode test:";

	bool allSucceeded = true;

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

#if 0
	// Disabled RGBA and BGRA for now; for some tiny images with alpha the WEBP encoder simply drops the alpha layer even when `WebPConfig::exact == 1`.
	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_BGR24,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_BGRA32,
	};
#else
	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_BGR24,
	};
#endif

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		Log::info() << " ";
		Log::info() << "Testing pixel format " << FrameType::translatePixelFormat(pixelFormat) << ":";

		ocean_assert(widths.size() == heights.size());

		for (size_t s = 0; s < widths.size(); ++s)
		{
			Log::info() << " ";

			for (const FrameType::PixelOrigin pixelOrigin : {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT})
			{
				if (!testWebpImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration))
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << " ";
	}

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "webp", 0.0))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "WEBP image encode/decode test succeeded.";
	}
	else
	{
		Log::info() << "WEBP image encode/decode test FAILED!";
	}

	return allSucceeded;
}

#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP

bool TestOpenImageLibraries::testAnyImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Any image encode/decode test:";

	bool allSucceeded = true;

	std::vector<std::string> encoderTypes(1, "bmp");

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	encoderTypes.push_back("jpg");
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	encoderTypes.push_back("png");
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF
	encoderTypes.push_back("tif");
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP
	encoderTypes.push_back("webp");
#endif

	const Timestamp startTimestamp(true);

	do
	{
		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());

		for (size_t n = 0; n < encoderTypes.size(); ++n)
		{
			const std::string& encoderType = encoderTypes[n];

			std::vector<uint8_t> buffer;
			if (Media::OpenImageLibraries::Image::encodeImage(sourceFrame, encoderType, buffer, true))
			{
				std::string decoderTypeExplicit;
				const Frame targetFrameExplicit = Media::OpenImageLibraries::Image::decodeImage(buffer.data(), buffer.size(), encoderType, &decoderTypeExplicit);

				if (!targetFrameExplicit.isValid() || encoderType != decoderTypeExplicit)
				{
					allSucceeded = false;
				}
				else
				{
					Frame convertedFrame;
					if (!CV::FrameConverter::Comfort::convert(targetFrameExplicit, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, false))
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
				const Frame targetFrameImplicit = Media::OpenImageLibraries::Image::decodeImage(buffer.data(), buffer.size(), "", &decoderTypeImplicit);

				if (!targetFrameImplicit.isValid() || encoderType != decoderTypeImplicit)
				{
					allSucceeded = false;
				}
				else
				{
					Frame convertedFrame;
					if (!CV::FrameConverter::Comfort::convert(targetFrameImplicit, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, false))
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
			else
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

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG

bool TestOpenImageLibraries::testJpgImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));

		// we have to blur the random image to create realistic test data for JPEG images
		CV::FrameFilterGaussian::filter(sourceFrame, 7u, sourceFrame.pixels() >= 50u * 50u ? WorkerPool::get().scopedWorker()() : nullptr);

		unsigned int correctRows = 0u;

		// png supports ORIGIN_UPPER_LEFT only
		const bool allowConversion = pixelOrigin != FrameType::ORIGIN_UPPER_LEFT;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::OpenImageLibraries::ImageJpg::encodeImage(sourceFrame, buffer, allowConversion, nullptr, 90);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
				const Frame targetFrame = Media::OpenImageLibraries::ImageJpg::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determined the similarity directly

					double minDifference, aveDifference, maxDifference;
					if (sourceFrame.pixels() < 8u * 8u || (determineSimilarity(sourceFrame, targetFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10)) // quite generous
					{
						correctRows = sourceFrame.height();
					}
				}
				else if (sourceFrame.frameType() == FrameType(targetFrame, FrameType::ORIGIN_LOWER_LEFT))
				{
					// we have to flip the resulting target frame

					Frame flippedFrame;
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::ORIGIN_LOWER_LEFT, flippedFrame, false))
					{
						ocean_assert(flippedFrame.frameType() == sourceFrame.frameType());

						double minDifference, aveDifference, maxDifference;
						if (sourceFrame.pixels() < 8u * 8u || (determineSimilarity(sourceFrame, flippedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10)) // quite generous
						{
							correctRows = sourceFrame.height();
						}
					}
				}
				else if (sourceFrame.pixelFormat() == FrameType::FORMAT_YUV24 && targetFrame.pixelFormat() == FrameType::FORMAT_RGB24)
				{
					// special handling for YUV24 input frames: we get an RGB24 frame as result
					// we must convert it from RGB24 to YUV24 and we may need to flip it

					Frame convertedFrame;
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::FORMAT_YUV24, sourceFrame.pixelOrigin(), convertedFrame, false))
					{
						ocean_assert(convertedFrame.frameType() == sourceFrame.frameType());

						double minDifference, aveDifference, maxDifference;
						if (sourceFrame.pixels() < 8u * 8u || (determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= 10)) // quite generous
						{
							correctRows = sourceFrame.height();
						}
					}
				}
			}
		}

		if (correctRows != sourceFrame.height())
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

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

bool TestOpenImageLibraries::testJpgDecodeStressTest()
{
	Log::info() << "JPEG decode stress test:";

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
			// memory starting with 'FF D8 FF'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 3u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 0xFF;
			data[1] = 0xD8;
			data[2] = 0xFF;
		}

		const Frame frame = Media::OpenImageLibraries::ImageJpg::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG

bool TestOpenImageLibraries::testPngImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;
	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));

		unsigned int correctRows = 0u;

		// png supports ORIGIN_UPPER_LEFT only
		const bool allowConversion = pixelOrigin != FrameType::ORIGIN_UPPER_LEFT;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::OpenImageLibraries::ImagePng::encodeImage(sourceFrame, buffer, allowConversion);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::OpenImageLibraries::ImagePng::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						if (memcmp(sourceFrame.constrow<void>(y), targetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
						{
							++correctRows;
						}
					}
				}
				else if (sourceFrame.frameType() == FrameType(targetFrame, FrameType::ORIGIN_LOWER_LEFT))
				{
					Frame flippedTargetFrame;
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::ORIGIN_LOWER_LEFT, flippedTargetFrame, false))
					{
						ocean_assert(flippedTargetFrame.frameType() == sourceFrame.frameType());

						for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
						{
							if (memcmp(sourceFrame.constrow<void>(y), flippedTargetFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
							{
								++correctRows;
							}
						}
					}
				}
			}
		}

		if (correctRows != sourceFrame.height())
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

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

bool TestOpenImageLibraries::testPngDecodeStressTest()
{
	Log::info() << "PNG decode stress test:";

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
			// memory starting with '89 50 4E 47'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 4u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 0x89;
			data[1] = 0x50;
			data[2] = 0x4E;
			data[3] = 0x47;
		}

		const Frame frame = Media::OpenImageLibraries::ImagePng::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF

bool TestOpenImageLibraries::testTifImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));

		unsigned int correctRows = 0u;

		const bool allowConversion = false;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::OpenImageLibraries::ImageTif::encodeImage(sourceFrame, buffer, allowConversion);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::OpenImageLibraries::ImageTif::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
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

		if (correctRows != sourceFrame.height())
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

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

bool TestOpenImageLibraries::testTifDecodeStressTest()
{
	Log::info() << "TIFF decode stress test:";

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
			// memory starting with '49 49 2A 00'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 4u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 0x49;
			data[1] = 0x49;
			data[2] = 0x2A;
			data[3] = 0x00;
		}

		const Frame frame = Media::OpenImageLibraries::ImageTif::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

#endif // OCEAN_MEDIA_OIL_SUPPORT_TIF

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF

bool TestOpenImageLibraries::testGifDecodeStressTest()
{
	Log::info() << "GIF decode stress test:";

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
			// memory starting with '47 49 46 38 37 61'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 6u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 0x47;
			data[1] = 0x49;
			data[2] = 0x46;
			data[3] = 0x38;
			data[4] = 0x37;
			data[5] = 0x61;
		}

		const Frames frames = Media::OpenImageLibraries::ImageGif::decodeImages(data.data(), data.size());

		if (!frames.empty())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP

bool TestOpenImageLibraries::testWebpImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	std::vector<uint8_t> buffer;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		buffer.clear();

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), &randomGenerator);

		unsigned int correctRows = 0u;

		constexpr bool allowConversion = true;

		performanceEncoding.start();
		const bool compressionSucceeded = Media::OpenImageLibraries::ImageWebp::encodeImage(sourceFrame, buffer, allowConversion, nullptr, 100.0f);
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::OpenImageLibraries::ImageWebp::decodeImage(buffer.data(), buffer.size());
			performanceDecoding.stop();

			if (targetFrame)
			{
				// WEBP decoding optionally converts the pixel format on load. Currently, it is set to load images with alpha as RGBA32 and without alpha as RGB24.
				// WEBP encoding currently only uses lossy compression, so we check for similarity of source and target, not equality
				// we must convert it from RGBA24 to pixel format of the source and we may need to flip it

				Frame convertedFrame;
				if (CV::FrameConverter::Comfort::convert(targetFrame, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, false))
				{
					ocean_assert(convertedFrame.frameType() == sourceFrame.frameType());

					double minDifference = -1.0;
					double avgDifference = -1.0;
					double maxDifference = -1.0;
					if (sourceFrame.pixels() < 8u * 8u || (determineSimilarity(sourceFrame, convertedFrame, minDifference, avgDifference, maxDifference) && avgDifference <= 10)) // quite generous
					{
						correctRows = sourceFrame.height();
					}
				}
			}
		}

		if (correctRows != sourceFrame.height())
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Encoding: Best: " << performanceEncoding.bestMseconds() << "ms, worst: " << performanceEncoding.worstMseconds() << "ms, average: " << performanceEncoding.averageMseconds() << "ms";
	Log::info() << "Decoding: Best: " << performanceDecoding.bestMseconds() << "ms, worst: " << performanceDecoding.worstMseconds() << "ms, average: " << performanceDecoding.averageMseconds() << "ms";

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

bool TestOpenImageLibraries::testWebpDecodeStressTest()
{
	Log::info() << "WEBP decode stress test:";

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
			// memory starting with '52 49 46 46'

			data = std::vector<uint8_t>(RandomI::random(randomGenerator, 4u, maximalDataSize));

			for (uint8_t& value : data)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			data[0] = 0x52;
			data[1] = 0x49;
			data[2] = 0x46;
			data[3] = 0x46;
		}

		const Frame frame = Media::OpenImageLibraries::ImageWebp::decodeImage(data.data(), data.size());

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP

bool TestOpenImageLibraries::testBufferImageRecorder(const FrameType& frameType, const std::string& imageType, const double maximalAverageDifference)
{
	ocean_assert(frameType.isValid());
	ocean_assert(maximalAverageDifference >= 0.0);

#ifdef OCEAN_DEBUG
	// we simply ensure that the one and only registered library is our OpenImageLibraries media library
	const std::vector<std::string> libraryNames = Media::Manager::get().libraries();
	ocean_assert(libraryNames.size() == 1 && libraryNames.front() == Media::OpenImageLibraries::nameOpenImageLibrariesLibrary());
#endif

	const Media::BufferImageRecorderRef imageRecorder = Media::Manager::get().newRecorder(Media::Recorder::BUFFER_IMAGE_RECORDER);
	if (imageRecorder.isNull())
	{
		return false;
	}

	Frame sourceFrame = CV::CVUtilities::randomizedFrame(frameType);

	if (maximalAverageDifference > 0.0)
	{
		CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());
	}

	std::vector<uint8_t> buffer;
	if (!imageRecorder->saveImage(sourceFrame, imageType, buffer) || buffer.empty())
	{
		return false;
	}

	const Media::BufferImageRef image = Media::Manager::get().newMedium("BufferImage", Media::Medium::BUFFER_IMAGE);
	if (image.isNull())
	{
		return false;
	}

	// setting the buffer without specifying the image type
	if (!image->setBufferImage(buffer.data(), buffer.size()))
	{
		return false;
	}

	if (!image->start())
	{
		return false;
	}

	FrameRef targetFrame = image->frame();

	if (targetFrame.isNull() || !targetFrame->isValid())
	{
		return false;
	}

	double minDifference, aveDifference, maxDifference;
	if (!determineSimilarity(sourceFrame, *targetFrame, minDifference, aveDifference, maxDifference) || aveDifference > maximalAverageDifference)
	{
		return false;
	}

	targetFrame.release();
	image->stop();

	// now we set the buffer again, but we also provide the image type
	if (!image->setBufferImage(buffer.data(), buffer.size(), imageType))
	{
		return false;
	}

	if (!image->start())
	{
		return false;
	}

	targetFrame = image->frame();

	if (targetFrame.isNull() || !targetFrame->isValid())
	{
		return false;
	}

	if (!determineSimilarity(sourceFrame, *targetFrame, minDifference, aveDifference, maxDifference) || aveDifference > maximalAverageDifference)
	{
		return false;
	}

	return true;
}

bool TestOpenImageLibraries::testDecodeStressTest()
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

		const Frame frame = Media::OpenImageLibraries::Image::decodeImage(data.data(), data.size(), imageBufferType);

		if (frame.isValid())
		{
			dummyValue += 1u;
		}
		else
		{
			dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestOpenImageLibraries::determineSimilarity(const Frame& firstFrame, const Frame& secondFrame, double& minDifference, double& aveDifference, double& maxDifference)
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
void TestOpenImageLibraries::determineSimilarity(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements, double& minDifference, double& aveDifference, double& maxDifference)
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
