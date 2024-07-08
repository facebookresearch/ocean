/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestImageIO.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE

#include "ocean/base/Build.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/media/BufferImage.h"
#include "ocean/media/BufferImageRecorder.h"
#include "ocean/media/Manager.h"

#include "ocean/media/imageio/IIOLibrary.h"
#include "ocean/media/imageio/Image.h"

#include "ocean/media/openimagelibraries/Image.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool TestImageIO::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_RUNTIME_STATIC
	Media::ImageIO::registerImageIOLibrary();
#endif

	Log::info() << "ImageIO test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testBmpImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testJpgImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHeicImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPngImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTifImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAnyImageEncodeDecode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterchangeability(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire ImageIO test succeeded.";
	}
	else
	{
		Log::info() << "ImageIO test FAILED!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::ImageIO::unregisterImageIOLibrary();
#endif

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

/**
 * This class implements a simple instance for the GTest ensuring that
 * the ImageIO plugin is registered during startup and unregistered before shutdown.
 */
class TestImageIOGTestInstance : public ::testing::Test
{
	protected:

		/**
		 * Default constructor.
		 */
  		TestImageIOGTestInstance()
  		{
  			// nothing to do here
		}

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::ImageIO::registerImageIOLibrary();
#endif
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::ImageIO::unregisterImageIOLibrary();
#endif
		}
};

TEST_F(TestImageIOGTestInstance, BmpImageBGR24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_BGR24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, BmpImageBGR24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_BGR24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, BmpImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, BmpImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testBmpImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, BmpImageRGB24Recorder)
{
	EXPECT_TRUE(TestImageIO::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "bmp", 0.0));
}


TEST_F(TestImageIOGTestInstance, JpgImageY8UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgImageY8LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgImageYUV24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YUV24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgImageYUV24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testJpgImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YUV24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgImageRGB24Recorder)
{
	EXPECT_TRUE(TestImageIO::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "jpg", 10.0));
}

TEST_F(TestImageIOGTestInstance, JpgQualityProperty)
{
	EXPECT_TRUE(TestImageIO::testQualityProperty("jpg", GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, JpgColorProfileNameProperty)
{
	EXPECT_TRUE(TestImageIO::testColorProfileNameProperty("jpg", GTEST_TEST_DURATION));
}


TEST_F(TestImageIOGTestInstance, HeicImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testHeicImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, HeicImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testHeicImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, HeicImageRGBA32UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testHeicImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, HeicImageRGBA32LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testHeicImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, HeicImageYUV24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testHeicImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YUV24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, HeicImageYUV24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testHeicImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YUV24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, HeicImageRGB24Recorder)
{
	EXPECT_TRUE(TestImageIO::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "heic", 10.0));
}

TEST_F(TestImageIOGTestInstance, HeicColorProfileNameProperty)
{
	EXPECT_TRUE(TestImageIO::testColorProfileNameProperty("heic", GTEST_TEST_DURATION));
}


TEST_F(TestImageIOGTestInstance, PngImageY8UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageY8LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y8, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageY16UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y16, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageY16LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_Y16, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageYA16UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YA16, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageYA16LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_YA16, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageRGBA32UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageRGBA32LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testPngImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, PngImageRGBA32Recorder)
{
	EXPECT_TRUE(TestImageIO::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "png", 0.0)); // using a pixel format without alpha channel to avoid verification of pre-multiplied pixels
}


TEST_F(TestImageIOGTestInstance, TifImageRGB24UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, TifImageRGB24LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, TifImageRGBA32UpperLeft)
{
	EXPECT_TRUE(TestImageIO::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, TifImageRGBA32LowerLeft)
{
	EXPECT_TRUE(TestImageIO::testTifImageEncodeDecode(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT, GTEST_TEST_DURATION));
}

TEST_F(TestImageIOGTestInstance, TifImageRGB24Recorder)
{
	EXPECT_TRUE(TestImageIO::testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "tif", 0.0));
}


TEST_F(TestImageIOGTestInstance, AnyImageEncodeDecode)
{
	EXPECT_TRUE(TestImageIO::testAnyImageEncodeDecode(GTEST_TEST_DURATION));
}


TEST_F(TestImageIOGTestInstance, Interchangeability)
{
	EXPECT_TRUE(TestImageIO::testInterchangeability(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestImageIO::testBmpImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BMP image encode/decode test:";

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

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_BGR24, FrameType::FORMAT_RGB24};

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
				if (!testBmpImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration))
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << " ";
	}

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "bmp", 0.0))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "BMP image encode/decode test succeeded.";
	}
	else
	{
		Log::info() << "BMP image encode/decode test FAILED!";
	}

	return allSucceeded;
}

bool TestImageIO::testJpgImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "JPEG image encode/decode test:";

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

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24, FrameType::FORMAT_YUV24};

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

	Log::info() << " ";

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "jpg", 10.0))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (!testQualityProperty("jpg", testDuration))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (!testColorProfileNameProperty("jpg", testDuration))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

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

bool TestImageIO::testHeicImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "HEIC image encode/decode test:";

	bool allSucceeded = true;

	// we use tiny images to ensure code correctness,
	// normal images for performance,
	// and extreme large images to identify memory leaks

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	const Indices32 widths = {2u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u}; // Apple's HEIC does not support images with resolution 1x1
	const Indices32 heights = {2u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u};
#else
	const Indices32 widths = {2u, 3u, 640u, 641u, 640u, 641u, 1280u, 1920u, 3840u, 7680u};
	const Indices32 heights = {2u, 2u, 480u, 480u, 481u, 481u, 720u, 1080u, 2160u, 4320u};
#endif

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32, FrameType::FORMAT_YUV24};

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
				if (!testHeicImageEncodeDecode(widths[s], heights[s], pixelFormat, pixelOrigin, testDuration))
				{
					allSucceeded = false;
				}
			}
		}

		Log::info() << " ";
	}

	Log::info() << " ";

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "heic", 10.0))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (!testColorProfileNameProperty("heic", testDuration))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "HEIC image encode/decode test succeeded.";
	}
	else
	{
		Log::info() << "HEIC image encode/decode test FAILED!";
	}

	return allSucceeded;
}

bool TestImageIO::testPngImageEncodeDecode(const double testDuration)
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

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::FORMAT_Y16, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32};

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

	if (!testBufferImageRecorder(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), "png", 0.0)) // using a pixel format without alpha channel to avoid verification of pre-multiplied pixels
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

bool TestImageIO::testTifImageEncodeDecode(const double testDuration)
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

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32};

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

bool TestImageIO::testAnyImageEncodeDecode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Any image encode/decode test:";

	bool allSucceeded = true;

	const std::vector<std::string> encoderTypes = {"bmp", "jpg", "heic", "png", "tif"};

	const Timestamp startTimestamp(true);

	do
	{
		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(640u, 480u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		ocean_assert(sourceFrame);

		CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());

		for (size_t n = 0; n < encoderTypes.size(); ++n)
		{
			const std::string& encoderType = encoderTypes[n];

			double threshold = 0.1;

			if (encoderType == "jpg" || encoderType == "heic")
			{
				threshold = 10.0; // quite generous
			}

			std::vector<uint8_t> buffer;
			if (!Media::ImageIO::Image::encodeImage(sourceFrame, encoderType, buffer, true))
			{
				allSucceeded = false;
			}

			std::string decoderTypeExplicit;
			const Frame targetFrameExplicit = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), encoderType, &decoderTypeExplicit);

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
					if (!determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= threshold)
					{
						allSucceeded = false;
					}
				}
			}

			std::string decoderTypeImplicit;
			const Frame targetFrameImplicit = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), "", &decoderTypeImplicit);

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
					if (!determineSimilarity(sourceFrame, convertedFrame, minDifference, aveDifference, maxDifference) && aveDifference <= threshold)
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
		Log::info() << "Valication FAILED!";
	}

	return allSucceeded;
}

bool TestImageIO::testInterchangeability(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Interchangeability test:";

	const std::vector<FrameType::PixelFormat> pixelFormats = {FrameType::FORMAT_Y8, FrameType::FORMAT_BGR24, FrameType::FORMAT_RGB24, FrameType::FORMAT_YUV24};
	const std::vector<FrameType::PixelOrigin> pixelOrigins = {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT};

	const std::set<std::string> supportedImageTypesIIO(Media::ImageIO::IIOLibrary::supportedImageTypes());
	const std::set<std::string> supportedImageTypesOIL(Media::OpenImageLibraries::OILLibrary::supportedImageTypes());

	std::vector<std::string> imageTypes;
	std::set_intersection(supportedImageTypesIIO.begin(), supportedImageTypesIIO.end(), supportedImageTypesOIL.begin(), supportedImageTypesOIL.end(), std::back_inserter(imageTypes));
	ocean_assert(!imageTypes.empty());

	std::string stringImageTypes;
	for (unsigned int n = 0u; n < imageTypes.size(); ++n)
	{
		if (n != 0u)
		{
			stringImageTypes += ", ";
		}

		stringImageTypes += imageTypes[n];
	}

	Log::info() << "Testing for the following image types: " << stringImageTypes;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(1u, 1024u);
		const unsigned int height = RandomI::random(1u, 1024u);

		for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
		{
			for (const FrameType::PixelOrigin& pixelOrigin : pixelOrigins)
			{
				Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));
				ocean_assert(sourceFrame);

				for (const std::string& imageType : imageTypes)
				{
					double threshold = 0.1;

					if (imageType == "jpg" || imageType == "heic")
					{
						threshold = 10.0; // quite generous
					}

					if ((imageType == "jpg" || imageType == "heic") && width >= 7u && height >= 7u)
					{
						// jpg/heic applies by default a lossy compression, so that we blur the randomized image
						CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());
					}

					{
						// first we encode with ImageIO and decode with OpenImageLibraries

						std::vector<uint8_t> buffer;
						if (!Media::ImageIO::Image::encodeImage(sourceFrame, imageType, buffer, true))
						{
							allSucceeded = false;
						}

						std::string decodedImageType;
						Frame targetFrame = Media::OpenImageLibraries::Image::decodeImage(buffer.data(), buffer.size(), std::string(), &decodedImageType);

						if (!targetFrame.isValid() || imageType != decodedImageType)
						{
							allSucceeded = false;
						}
						else
						{
							if (!CV::FrameConverter::Comfort::change(targetFrame, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin()))
							{
								allSucceeded = false;
							}
							else
							{
								double minDifference, aveDifference, maxDifference;
								if (!determineSimilarity(sourceFrame, targetFrame, minDifference, aveDifference, maxDifference) && aveDifference <= threshold)
								{
									allSucceeded = false;
								}
							}
						}
					}

					{
						// now we encode with OpenImageLibraries and decode with ImageIO

						std::vector<uint8_t> buffer;
						if (!Media::OpenImageLibraries::Image::encodeImage(sourceFrame, imageType, buffer, true))
						{
							allSucceeded = false;
						}

						std::string decodedImageType;
						Frame targetFrame = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), std::string(), &decodedImageType);

						if (!targetFrame.isValid() || imageType != decodedImageType)
						{
							allSucceeded = false;
						}
						else
						{
							if (!CV::FrameConverter::Comfort::change(targetFrame, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin()))
							{
								allSucceeded = false;
							}
							else
							{
								double minDifference, aveDifference, maxDifference;
								if (!determineSimilarity(sourceFrame, targetFrame, minDifference, aveDifference, maxDifference) && aveDifference <= threshold)
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
		Log::info() << "Valication FAILED!";
	}

	return allSucceeded;
}

bool TestImageIO::testBmpImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	if (Build::platformType() == "ios")
	{
		if (width % 2u != 0u)
		{
			// iOS seems to have issues with BMP images with odd image width (the last element of the last pixel in each row is wrong), skipping those tests

			Log::info() << "Skipping test due to bug in iOS";
			return true;
		}
	}

	bool allSucceeded = true;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> buffer;

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));
		ocean_assert(sourceFrame);

		unsigned int correctRows = 0u;

		// Apple's bmp supports RGB24 only
		const bool needsConversion = pixelFormat != FrameType::FORMAT_RGB24;

		const bool allowConversion = needsConversion || RandomI::random(1u) == 0u;

		bool compressionSucceeded = false;

		performanceEncoding.start();
			if (allowConversion)
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "bmp", buffer, allowConversion, nullptr);
			}
			else
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "bmp", buffer);
			}
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), "bmp");
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determine the similarity directly

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
					// we convert the pixel format from RGB24 to match the pixel format of the source

					Frame convertedFrame;
					if (CV::FrameConverter::Comfort::convert(targetFrame, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
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

bool TestImageIO::testJpgImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> buffer;

		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));
		ocean_assert(sourceFrame);

		unsigned int correctRows = 0u;

		if (sourceFrame.width() >= 7u && sourceFrame.height() >= 7u)
		{
			// we have to blur the random image to create realistic test data for JPEG images
			CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());
		}

		// ImageIO does not support YUV pixel formats
		const bool needsConversion = pixelFormat == FrameType::FORMAT_YUV24;

		const bool allowConversion = needsConversion || RandomI::random(1u) == 0u;;

		bool compressionSucceeded = false;

		performanceEncoding.start();
			if (allowConversion)
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "jpg", buffer, allowConversion, nullptr);
			}
			else
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "jpg", buffer);
			}
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), "jpg");
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determine the similarity directly

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
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::ORIGIN_LOWER_LEFT, flippedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
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
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::FORMAT_YUV24, sourceFrame.pixelOrigin(), convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
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

bool TestImageIO::testHeicImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> buffer;

		Frame sourceFrame =	CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));
		ocean_assert(sourceFrame);

		unsigned int correctRows = 0u;

		if (sourceFrame.width() >= 7u && sourceFrame.height() >= 7u)
		{
			// we have to blur the random image to create realistic test data for HEIC images
			CV::FrameFilterGaussian::filter(sourceFrame, 7u, WorkerPool::get().conditionalScopedWorker(sourceFrame.pixels() >= 50u * 50u)());
		}

		// ImageIO does not support YUV pixel formats
		const bool needsConversion = pixelFormat == FrameType::FORMAT_YUV24;

		const bool allowConversion = needsConversion || RandomI::random(1u) == 0u;;

		bool compressionSucceeded = false;

		performanceEncoding.start();
			if (allowConversion)
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "heic", buffer, allowConversion, nullptr);
			}
			else
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "heic", buffer);
			}
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), "heic");
			performanceDecoding.stop();

			if (targetFrame)
			{
				if (sourceFrame.frameType() == targetFrame.frameType())
				{
					// we can determine the similarity directly

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
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::ORIGIN_LOWER_LEFT, flippedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
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
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::FORMAT_YUV24, sourceFrame.pixelOrigin(), convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
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

bool TestImageIO::testPngImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> buffer;

		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));
		ocean_assert(sourceFrame);

		unsigned int correctRows = 0u;

		// ImageIO (and PNG) supports ORIGIN_UPPER_LEFT only
		const bool needsConversion = pixelOrigin != FrameType::ORIGIN_UPPER_LEFT;

		const bool allowConversion = needsConversion || RandomI::random(1u) == 0u;;

		bool compressionSucceeded = false;

		performanceEncoding.start();
			if (allowConversion)
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "png", buffer, allowConversion, nullptr);
			}
			else
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "png", buffer);
			}
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
				const Frame targetFrame = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), "png");
			performanceDecoding.stop();

			if (targetFrame)
			{
				unsigned int alphaChannelIndex = (unsigned int)(-1);

				if (sourceFrame.pixelFormat() == FrameType::FORMAT_YA16)
				{
					alphaChannelIndex = 1u;
				}
				else if (sourceFrame.pixelFormat() == FrameType::FORMAT_RGBA32)
				{
					alphaChannelIndex = 3u;
				}

				ocean_assert((alphaChannelIndex != (unsigned int)(-1)) == FrameType::formatHasAlphaChannel(sourceFrame.pixelFormat()));

				Frame flippedTargetFrame(targetFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				if (sourceFrame.frameType() != flippedTargetFrame.frameType())
				{
					CV::FrameConverter::Comfort::change(flippedTargetFrame, FrameType::ORIGIN_LOWER_LEFT);
				}

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					bool invalidRow = false;

					for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
					{
						if (sourceFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
						{
							const uint8_t* const sourcePixel = sourceFrame.constpixel<uint8_t>(x, y);
							const uint8_t* const targetPixel = flippedTargetFrame.constpixel<uint8_t>(x, y);

							if (alphaChannelIndex == (unsigned int)(-1))
							{
								// no alpha channel, no premultiplied vs. straight alpha conversion, we expect a perfect result

								if (memcmp(sourcePixel, targetPixel, sourceFrame.channels() * sizeof(uint8_t)) != 0)
								{
									invalidRow = true;
								}
							}
							else
							{
								// we have an alpha channel, so we need to be more generous due to premultiplied vs. straight alpha conversion

								if (sourcePixel[alphaChannelIndex] != targetPixel[alphaChannelIndex])
								{
									invalidRow = true;
								}

								unsigned int eps = 255u;

								if (sourcePixel[alphaChannelIndex] >= 40u)
								{
									eps = 4u;
								}
								else if (sourcePixel[alphaChannelIndex] >= 20u)
								{
									eps = 8u;
								}
								else if (sourcePixel[alphaChannelIndex] >= 10u)
								{
									eps = 20u;
								}
								else if (sourcePixel[alphaChannelIndex] >= 4u)
								{
									eps = 40u;
								}

								for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
								{
									if (c != alphaChannelIndex)
									{
										if (abs(int(sourcePixel[c]) - int(targetPixel[c])) > int(eps))
										{
											invalidRow = true;
										}
									}
								}
							}
						}
						else if (sourceFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_16)
						{
							const uint16_t* const sourcePixel = sourceFrame.constpixel<uint16_t>(x, y);
							const uint16_t* const targetPixel = flippedTargetFrame.constpixel<uint16_t>(x, y);

							ocean_assert(alphaChannelIndex == (unsigned int)(-1));

							// no alpha channel, no premultiplied vs. straight alpha conversion, we expect a perfect result

							if (memcmp(sourcePixel, targetPixel, sourceFrame.channels() * sizeof(uint16_t)) != 0)
							{
								invalidRow = true;
							}
						}
					}

					if (!invalidRow)
					{
						++correctRows;
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

bool TestImageIO::testTifImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height << " image, with origin " << FrameType::translatePixelOrigin(pixelOrigin) << ":";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceEncoding, performanceDecoding;

	const Timestamp startTimestamp(true);
	do
	{
		std::vector<uint8_t> buffer;

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin));
		ocean_assert(sourceFrame);

		unsigned int correctRows = 0u;

		constexpr bool needsConversion = false;

		const bool allowConversion = needsConversion || RandomI::random(1u) == 0u;;

		bool compressionSucceeded = false;

		performanceEncoding.start();
			if (allowConversion)
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "tif", buffer, allowConversion, nullptr);
			}
			else
			{
				compressionSucceeded = Media::ImageIO::Image::encodeImage(sourceFrame, "tif", buffer);
			}
		performanceEncoding.stop();

		if (compressionSucceeded)
		{
			performanceDecoding.start();
			const Frame targetFrame = Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), "tif");
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
					Frame flippedFrame;
					if (CV::FrameConverter::Comfort::convert(targetFrame, FrameType::ORIGIN_LOWER_LEFT, flippedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
					{
						ocean_assert(flippedFrame.frameType() == sourceFrame.frameType());

						for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
						{
							if (memcmp(sourceFrame.constrow<void>(y), flippedFrame.constrow<void>(y), sourceFrame.planeWidthBytes(0u)) == 0)
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

bool TestImageIO::testBufferImageRecorder(const FrameType& frameType, const std::string& imageType, const double maximalAverageDifference)
{
	ocean_assert(frameType.isValid());
	ocean_assert(maximalAverageDifference >= 0.0);

#ifdef OCEAN_DEBUG
	// we simply ensure that the one and only registered library is our ImageIO media library
	const std::vector<std::string> libraryNames = Media::Manager::get().libraries();
	ocean_assert(libraryNames.size() == 1 && libraryNames.front() == Media::ImageIO::nameImageIOLibrary());
#endif

	const Media::BufferImageRecorderRef imageRecorder = Media::Manager::get().newRecorder(Media::Recorder::BUFFER_IMAGE_RECORDER);
	if (imageRecorder.isNull())
	{
		return false;
	}

	Frame sourceFrame = CV::CVUtilities::randomizedFrame(frameType);

	if (maximalAverageDifference > 0.0 && sourceFrame.width() >= 7u && sourceFrame.height() >= 7u)
	{
		CV::FrameFilterGaussian::filter(sourceFrame, 7u, sourceFrame.pixels() >= 50u * 50u ? WorkerPool::get().scopedWorker()() : nullptr);
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

bool TestImageIO::testQualityProperty(const std::string& imageType, const double testDuration)
{
	ocean_assert(!imageType.empty());
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test quality property for '" << imageType << "':";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(1280u, 720u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		CV::FrameFilterGaussian::filter(frame, 7u, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 50u * 50u)());

		constexpr float highQuality = 1.0f;

		std::vector<uint8_t> bufferHighQuality;
		if (!Media::ImageIO::Image::encodeImage(frame, imageType, bufferHighQuality, Media::Image::Properties(highQuality)))
		{
			allSucceeded = false;
		}

		constexpr float lowQuality = 0.5f;

		std::vector<uint8_t> bufferLowQuality;
		if (!Media::ImageIO::Image::encodeImage(frame, imageType, bufferLowQuality, Media::Image::Properties(lowQuality)))
		{
			allSucceeded = false;
		}

		if (bufferHighQuality.empty() || bufferLowQuality.empty())
		{
			allSucceeded = false;
		}

		if (bufferHighQuality.size() <= bufferLowQuality.size()) // we ensure that the low quality image has a smaller footprint
		{
			allSucceeded = false;
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

bool TestImageIO::testColorProfileNameProperty(const std::string& imageType, const double testDuration)
{
	ocean_assert(!imageType.empty());
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test color profile name property for '" << imageType << "':";

	typedef std::pair<FrameType::PixelFormat, std::string> ColorProfilePair;

	std::vector<ColorProfilePair> colorProfileNames =
	{
		{FrameType::FORMAT_RGB24, ""}, // default
		{FrameType::FORMAT_RGB24, "Display P3"},
		{FrameType::FORMAT_RGB24, "sRGB IEC61966-2.1"},
		{FrameType::FORMAT_RGB24, "sRGB IEC61966-2.1 Linear"},
		{FrameType::FORMAT_RGB24, "Adobe RGB (1998)"},
		{FrameType::FORMAT_RGB24, "Generic HDR Profile"},
		{FrameType::FORMAT_RGB24, "ROMM RGB: ISO 22028-2:2013"},
	};

	if (imageType != "heic")
	{
		colorProfileNames.emplace_back(FrameType::FORMAT_Y8, ""); // default
		colorProfileNames.emplace_back(FrameType::FORMAT_Y8, "Generic Gray Gamma 2.2 Profile");
		colorProfileNames.emplace_back(FrameType::FORMAT_Y8, "Linear Gray");
	};

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (const ColorProfilePair& colorProfilePair : colorProfileNames)
		{
			const FrameType::PixelFormat pixelFormat = colorProfilePair.first;
			const std::string& colorProfileName = colorProfilePair.second;

			const unsigned int minSize = imageType == "heic" ? 2u : 1u; // Apple's HEIC does not support 1x1

			const unsigned int width = RandomI::random(minSize, 1280u);
			const unsigned int height = RandomI::random(minSize, 720u);

			Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));

			Media::Image::Properties encodedProperties;
			encodedProperties.colorProfileName_ = colorProfileName;

			std::vector<uint8_t> buffer;
			if (!Media::ImageIO::Image::encodeImage(frame, imageType, buffer, encodedProperties))
			{
				allSucceeded = false;
			}

			const std::string inputImageType = RandomI::random(1u) == 0u ? imageType : "";
			std::string decodedImageType;

			Media::Image::Properties decodedProperties;
			if (allSucceeded && !Media::ImageIO::Image::decodeImage(buffer.data(), buffer.size(), inputImageType, &decodedImageType, &decodedProperties))
			{
				allSucceeded = false;
			}

			std::string expectedColorProfileName = colorProfileName;

			if (expectedColorProfileName.empty())
			{
				// handling default case

				if (pixelFormat == FrameType::FORMAT_RGB24)
				{
					expectedColorProfileName = "sRGB IEC61966-2.1";
				}
				else
				{
					expectedColorProfileName = "Generic Gray Gamma 2.2 Profile";
				}
			}

			if (decodedProperties.colorProfileName_ != expectedColorProfileName)
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

bool TestImageIO::determineSimilarity(const Frame& firstFrame, const Frame& secondFrame, double& minDifference, double& aveDifference, double& maxDifference)
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
void TestImageIO::determineSimilarity(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements, double& minDifference, double& aveDifference, double& maxDifference)
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

#endif // OCEAN_PLATFORM_BUILD_APPLE
