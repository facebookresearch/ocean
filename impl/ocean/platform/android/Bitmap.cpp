/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/Bitmap.h"

#include <android/ndk-version.h>

namespace Ocean
{

namespace Platform
{

namespace Android
{

Frame Bitmap::toFrame(JNIEnv* env, jobject bitmap, const bool copyData)
{
	ocean_assert(env != nullptr && bitmap != nullptr);
	if (env == nullptr || bitmap == nullptr)
	{
		return Frame();
	}

	AndroidBitmapInfo bitmapInfo;
	if (AndroidBitmap_getInfo(env, bitmap, &bitmapInfo) < 0)
	{
		return Frame();
	}

	ocean_assert(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888);
	if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	{
		return Frame();
	}

	const unsigned int width = bitmapInfo.width;
	const unsigned int height = bitmapInfo.height;
	const unsigned int strideBytes = bitmapInfo.stride;

	unsigned int paddingElements = 0u;
	if (!Frame::strideBytes2paddingElements(FrameType::FORMAT_RGBA32, width, strideBytes, paddingElements))
	{
		return Frame();
	}

	void* pixelData = nullptr;

	if (AndroidBitmap_lockPixels(env, bitmap, &pixelData) < 0)
	{
		ocean_assert(false && "Could not lock the pixel data!");
		return Frame();
	}

	const Frame::CopyMode copyMode = copyData ? Frame::CM_COPY_REMOVE_PADDING_LAYOUT : Frame::CM_USE_KEEP_LAYOUT;

	Frame frame(FrameType(width, height, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT), pixelData, copyMode, paddingElements);

	if (AndroidBitmap_unlockPixels(env, bitmap) < 0)
	{
		ocean_assert(false && "Could not unlock the pixel data!");
		return Frame();
	}

	return frame;
}

FrameType::PixelFormat Bitmap::translateFormat(const AndroidBitmapFormat format)
{
	switch (format)
	{
		case ANDROID_BITMAP_FORMAT_NONE:
			return FrameType::FORMAT_UNDEFINED;

		case ANDROID_BITMAP_FORMAT_RGBA_8888:
			return FrameType::FORMAT_RGBA32;

		case ANDROID_BITMAP_FORMAT_RGB_565:
			return FrameType::FORMAT_BGR565;

		case ANDROID_BITMAP_FORMAT_RGBA_4444: // deprecated in API 13
			break;

		case ANDROID_BITMAP_FORMAT_A_8:
			return FrameType::genericPixelFormat<uint8_t, 1u>();

#if defined(__NDK_MAJOR__) && __NDK_MAJOR__ >= 21
		case ANDROID_BITMAP_FORMAT_RGBA_F16:
			break;
#endif

#if defined(__NDK_MAJOR__) && __NDK_MAJOR__ >= 25
		case ANDROID_BITMAP_FORMAT_RGBA_1010102:
			break;
#endif
	}

	ocean_assert(false && "No corresponding Ocean pixel format!");
	return FrameType::FORMAT_UNDEFINED;
}

AndroidBitmapFormat Bitmap::translateFormat(const FrameType::PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_UNDEFINED:
			return ANDROID_BITMAP_FORMAT_NONE;

		case FrameType::FORMAT_RGBA32:
			return ANDROID_BITMAP_FORMAT_RGBA_8888;

		case FrameType::FORMAT_BGR565:
			return ANDROID_BITMAP_FORMAT_RGB_565;

		default:
			break;
	}

	 ocean_assert(false && "No corresponding Android bitmap format!");
	return ANDROID_BITMAP_FORMAT_NONE;
}

}

}

}
