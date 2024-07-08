/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_BITMAP_H
#define META_OCEAN_PLATFORM_ANDROID_BITMAP_H

#include "ocean/platform/android/Android.h"

#include "ocean/base/Frame.h"

#include <android/bitmap.h>

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements bitmap functions.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT Bitmap
{
	public:

		/**
		 * Translates an Android bitmap format to an Ocean pixel format.
		 * @param format The Android bitmap format to translate
		 * @return The corresponding Ocean pixel format, FORMAT_UNDEFINED if unknown
		 */
		static FrameType::PixelFormat translateFormat(const AndroidBitmapFormat format);

		/**
		 * Translates an Ocean pixel format to an Android bitmap format.
		 * @param pixelFormat The Ocean pixel format to translate
		 * @return The corresponding Android bitmap format, ANDROID_BITMAP_FORMAT_NONE if unknown
		 */
		static AndroidBitmapFormat translateFormat(const FrameType::PixelFormat pixelFormat);
};

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_BITMAP_H
