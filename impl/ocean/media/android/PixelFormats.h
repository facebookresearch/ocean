/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_PIXEL_FORMATS_H
#define META_OCEAN_MEDIA_ANDROID_PIXEL_FORMATS_H

#include "ocean/media/android/Android.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class defines conversions to/from Android color format types for video data; see the associated definitions in android.media.MediaCodecInfo.CodecCapabilities.
 * @ingroup mediaandroid
 */
class PixelFormats
{
	public:

		/**
		 * Set of Ocean-supported Android pixel formats for Android MediaCodec objects.
		 * The naming conventions match those defined in the Java class android.media.MediaCodecInfo.CodecCapabilities:
		 * https://developer.android.com/reference/android/media/MediaCodecInfo.CodecCapabilities
		 */
		enum class AndroidMediaCodecColorFormat : int32_t
		{
			/// Unknown color format.
			UNKNOWN = 0,
			/// Android identifier for YUV420 color formats, deprecated use COLOR_FormatYUV420Flexible if possible.
			COLOR_FormatYUV420Planar = 19,
			// Android identifier for YUV420 color formats, identical to FORMAT_Y_UV12_LIMITED_RANGE, deprecated use COLOR_FormatYUV420Flexible if possible.
			COLOR_FormatYUV420SemiPlanar = 21,
			/// Android identifier for YUV420 color formats, identical to FORMAT_Y_U_V12_LIMITED_RANGE
			COLOR_FormatYUV420Flexible = 0x7f420888
		};

		/**
		 * Definition of individual constants for Android MediaFormat color ranges.
		 * https://developer.android.com/reference/android/media/MediaFormat#COLOR_RANGE_FULL
		 */
		enum AndroidMediaFormatColorRange : int32_t
		{
			/// Unknown color range.
			UNKNOWN = 0,
			/// Full range. Y, Cr and Cb component values range from 0 to 255 for 8-bit content.
			COLOR_RANGE_FULL = 1,
			/// Limited range. Y component values range from 16 to 235 for 8-bit content. Cr, Cy values range from 16 to 240 for 8-bit content. This is the default for video content.
			COLOR_RANGE_LIMITED = 2
		};

	public:

		/**
		 * Translates an Android pixel format to an Ocean pixel format for video data.
		 * @param androidFormat The Android pixel format to translate
		 * @param androidColorRange The Android Media Format color range associated with the Android pixel format
		 * @return The corresponding Ocean pixel format, FORMAT_UNDEFINED if no corresponding Ocean pixel format exists
		 */
		static FrameType::PixelFormat androidMediaCodecColorFormatToPixelFormat(const AndroidMediaCodecColorFormat androidFormat, const AndroidMediaFormatColorRange androidColorRange);

		/**
		 * Translates an Ocean pixel format to an Android pixel format for video data.
		 * @param pixelFormat The Ocean pixel format to translate
		 * @param androidColorRange The resulting Android Media Format color range associated with the Android pixel format
		 * @return The corresponding Android pixel format, UNKNOWN if no corresponding Android pixel format exists
		 */
		static AndroidMediaCodecColorFormat pixelFormatToAndroidMediaCodecColorFormat(const FrameType::PixelFormat pixelFormat, AndroidMediaFormatColorRange& androidColorRange);
};

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_PIXEL_FORMATS_H
