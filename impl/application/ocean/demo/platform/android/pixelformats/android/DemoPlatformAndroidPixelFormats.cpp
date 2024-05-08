/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/android/pixelformats/android/DemoPlatformAndroidPixelFormats.h"

#include "ocean/platform/android/Utilities.h"

#include <android/bitmap.h>

using namespace Ocean;

bool Java_com_meta_ocean_app_demo_platform_android_pixelformats_android_DemoPlatformAndroidPixelFormatsActivity_setChannelToValue(JNIEnv* env, jobject javaThis, jobject bitmap, jint channel, jint value)
{
	ocean_assert(channel >= 0 && channel <= 4);
	ocean_assert(value >= 0 && value <= 255);

	Log::info() << " ";

	AndroidBitmapInfo bitmapInfo;

	if (AndroidBitmap_getInfo(env, bitmap, &bitmapInfo) < 0)
	{
			Log::error() << "Could not determine the bitmap info!";
			return false;
	}

	std::string format = "Unknown!";

	switch (bitmapInfo.format)
	{
		case ANDROID_BITMAP_FORMAT_A_8:
			format = "ANDROID_BITMAP_FORMAT_A_8";
			break;

		case ANDROID_BITMAP_FORMAT_NONE:
			format = "ANDROID_BITMAP_FORMAT_NONE";
			break;

		case ANDROID_BITMAP_FORMAT_RGBA_4444:
			format = "ANDROID_BITMAP_FORMAT_RGBA_4444";
			break;

		case ANDROID_BITMAP_FORMAT_RGBA_8888:
			format = "ANDROID_BITMAP_FORMAT_RGBA_8888";
			break;

		case ANDROID_BITMAP_FORMAT_RGB_565:
			format = "ANDROID_BITMAP_FORMAT_RGB_565";
			break;
	}

	const unsigned int width = bitmapInfo.width;
	const unsigned int height = bitmapInfo.height;
	const unsigned int strideBytes = bitmapInfo.stride;

	Log::info() << "The resolution of the bitmap object is " << width << "x" << height;
	Log::info() << "The pixel format is " + format;
	Log::info() << "The stride is " << strideBytes << " bytes";

	void* pixelData = nullptr;

	if (AndroidBitmap_lockPixels(env, bitmap, &pixelData) < 0)
	{
		Log::error() << "Could not lock the pixel data!";
		return false;
	}

	Log::info() << "Setting " << channel << " channel to value " << value;

	switch (bitmapInfo.format)
	{
		case ANDROID_BITMAP_FORMAT_A_8:
			Log::error() << "Format not supported!";
			return false;

		case ANDROID_BITMAP_FORMAT_NONE:
			Log::error() << "Format not supported!";
			return false;

		case ANDROID_BITMAP_FORMAT_RGBA_4444:
			Log::error() << "Format not supported!";
			return false;

		case ANDROID_BITMAP_FORMAT_RGBA_8888:
		{
			// ANDROID_BITMAP_FORMAT_RGBA_8888 is actually Ocean's FORMAT_RGBA32
			// as the first (8) bits in memory are red, the second (8) bits are green, the next (8) bits are blue, ...

			// here is a layout in memory:
			// pixel:    0                               1
			// bytes:    0       1       2       3       4
			// bits:     0123456789ABCDEF0123456789ABCDEF01234567
			// channels: 0       1       2       3       0
			//           RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAARRRRRRRR

			ocean_assert(channel <= 3u);

			if (channel >= 4u)
			{
				return false;
			}

			uint8_t* rgbaData = (uint8_t*)(pixelData);

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					rgbaData[x * 4u + channel] = uint8_t(value);
				}

				rgbaData += strideBytes;
			}

			break;
		}

		case ANDROID_BITMAP_FORMAT_RGB_565:
		{
			// ANDROID_BITMAP_FORMAT_RGB_565 is actually Ocean's FORMAT_BGR565
			// as the first (5) bits in memory are blue, the second (6) bits are green, the next (5) bits are red, ...

			// here is a layout in memory:
			// pixel:    0               1
			// bytes:    0       1       2       3
			// bits:     0123456789ABCDEF0123456789ABCDEF
			// channels: 0    1     2    0    1     2
			//           BBBBBGGGGGGRRRRRBBBBBGGGGGGRRRRR

			ocean_assert(channel <= 2u);

			if (channel >= 3u)
			{
				return false;
			}

			uint16_t* rgb565Data = (uint16_t*)(pixelData);

			ocean_assert(strideBytes % 2u == 0u);
			const unsigned int strideElements = (strideBytes / 2u);

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					uint8_t rgb888[3] =
					{
						uint8_t(rgb565Data[x] & 0x1Fu), // red with 5 bits
					 	uint8_t((rgb565Data[x] >> 5u) & 0x3Fu), // green with 6 bits
						uint8_t(rgb565Data[x] >> 11u), // blue with 5 bits
					};

					rgb888[channel] = value;

					const uint16_t r5 = (uint16_t(rgb888[0] & 0x1Fu)) << 0u;
					const uint16_t g6 = (uint16_t(rgb888[1] & 0x3Fu)) << 5u;
					const uint16_t b5 = (uint16_t(rgb888[2] & 0x1Fu)) << 11u;

					rgb565Data[x] = r5 | g6 | b5;
				}

				rgb565Data += strideElements;
			}

			break;
		}
	}

	if (AndroidBitmap_unlockPixels(env, bitmap) < 0)
	{
		Log::error() << "Could not unlock the pixel data!";
		return false;
	}

	return true;
}
