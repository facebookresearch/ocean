/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/PixelFormats.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

FrameType::PixelFormat PixelFormats::androidMediaCodecColorFormatToPixelFormat(const AndroidMediaCodecColorFormat androidFormat, const AndroidMediaFormatColorRange androidColorRange)
{
	switch (androidFormat)
	{
		case AndroidMediaCodecColorFormat::COLOR_FormatYUV420Planar:
		case AndroidMediaCodecColorFormat::COLOR_FormatYUV420Flexible:
		{
			if (androidColorRange == AndroidMediaFormatColorRange::COLOR_RANGE_FULL)
			{
				return FrameType::FORMAT_Y_U_V12_FULL_RANGE;
			}

			ocean_assert(androidColorRange == AndroidMediaFormatColorRange::COLOR_RANGE_LIMITED);
			return FrameType::FORMAT_Y_U_V12_LIMITED_RANGE;
		}

		case AndroidMediaCodecColorFormat::COLOR_FormatYUV420SemiPlanar:
		{
			if (androidColorRange == AndroidMediaFormatColorRange::COLOR_RANGE_FULL)
			{
				return FrameType::FORMAT_Y_UV12_FULL_RANGE;
			}

			ocean_assert(androidColorRange == AndroidMediaFormatColorRange::COLOR_RANGE_LIMITED);
			return FrameType::FORMAT_Y_UV12_LIMITED_RANGE;
		}

		default:
			break;
	}

	return FrameType::FORMAT_UNDEFINED;
}

PixelFormats::AndroidMediaCodecColorFormat PixelFormats::pixelFormatToAndroidMediaCodecColorFormat(const FrameType::PixelFormat pixelFormat, AndroidMediaFormatColorRange& androidColorRange)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
			androidColorRange = AndroidMediaFormatColorRange::COLOR_RANGE_LIMITED;
			return AndroidMediaCodecColorFormat::COLOR_FormatYUV420Flexible;

		case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
			androidColorRange = AndroidMediaFormatColorRange::COLOR_RANGE_FULL;
			return AndroidMediaCodecColorFormat::COLOR_FormatYUV420Flexible;

		case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
			androidColorRange = AndroidMediaFormatColorRange::COLOR_RANGE_LIMITED;
			return AndroidMediaCodecColorFormat::COLOR_FormatYUV420SemiPlanar;

		case FrameType::FORMAT_Y_UV12_FULL_RANGE:
			androidColorRange = AndroidMediaFormatColorRange::COLOR_RANGE_FULL;
			return AndroidMediaCodecColorFormat::COLOR_FormatYUV420SemiPlanar;

		default:
			break;
	}

	androidColorRange = AndroidMediaFormatColorRange::UNKNOWN;
	return AndroidMediaCodecColorFormat::UNKNOWN;
}

}

}

}
