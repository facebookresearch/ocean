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
		case COLOR_FORMAT_YUV420Planar:
		case COLOR_FORMAT_YUV420Flexible:
		{
			if (androidColorRange == COLOR_RANGE_FULL)
			{
				return FrameType::FORMAT_Y_U_V12_FULL_RANGE;
			}

			ocean_assert(androidColorRange == COLOR_RANGE_LIMITED);
			return FrameType::FORMAT_Y_U_V12_LIMITED_RANGE;
		}

		case COLOR_FORMAT_YUV420SemiPlanar:
		{
			if (androidColorRange == COLOR_RANGE_FULL)
			{
				return FrameType::FORMAT_Y_UV12_FULL_RANGE;
			}

			ocean_assert(androidColorRange == COLOR_RANGE_LIMITED);
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
			androidColorRange = COLOR_RANGE_LIMITED;
			return COLOR_FORMAT_YUV420Flexible;

		case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
			androidColorRange = COLOR_RANGE_FULL;
			return COLOR_FORMAT_YUV420Flexible;

		case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
			androidColorRange = COLOR_RANGE_LIMITED;
			return COLOR_FORMAT_YUV420SemiPlanar;

		case FrameType::FORMAT_Y_UV12_FULL_RANGE:
			androidColorRange = COLOR_RANGE_FULL;
			return COLOR_FORMAT_YUV420SemiPlanar;

		default:
			break;
	}

	androidColorRange = COLOR_RANGE_UNKNOWN;
	return COLOR_FORMAT_UNKNOWN;
}

}

}

}
