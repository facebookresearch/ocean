/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Frame.h"
#include "ocean/base/String.h"

namespace Ocean
{

unsigned int FrameType::channels(const PixelFormat pixelFormat)
{
	const unsigned int genericChannels = formatGenericNumberChannels(pixelFormat);

	if (genericChannels != 0u)
	{
		return genericChannels;
	}

	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
			return 0u;

		case FORMAT_Y10_PACKED:
			return 1u;

		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
		case FORMAT_RGGB10_PACKED:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
			return 3u;

		case FORMAT_BGR4444: // similar to RGB32 which has 4 channels
		case FORMAT_BGRA4444:
		case FORMAT_RGB4444:
		case FORMAT_RGBA4444:
			return 4u;

		default:
			ocean_assert(false && "Unknown pixel format.");
			return 0u;
	}
}

unsigned int FrameType::bytesPerDataType(const DataType dataType)
{
	switch (dataType)
	{
		case DT_UNDEFINED:
			return 0u;

		case DT_UNSIGNED_INTEGER_8:
		case DT_SIGNED_INTEGER_8:
			return 1u;

		case DT_UNSIGNED_INTEGER_16:
		case DT_SIGNED_INTEGER_16:
		case DT_SIGNED_FLOAT_16:
			return 2u;

		case DT_UNSIGNED_INTEGER_32:
		case DT_SIGNED_INTEGER_32:
		case DT_SIGNED_FLOAT_32:
			return 4u;

		case DT_UNSIGNED_INTEGER_64:
		case DT_SIGNED_INTEGER_64:
		case DT_SIGNED_FLOAT_64:
			return 8u;

		case DT_END:
			break;
	}

	ocean_assert(false && "Invalid data type!");
	return 0u;
}

FrameType::PixelFormat FrameType::genericPixelFormat(const unsigned int bitsPerPixelChannel, const uint32_t channels, const uint32_t planes, const uint32_t widthMultiple, const uint32_t heightMultiple)
{
	ocean_assert(bitsPerPixelChannel >= 4u && bitsPerPixelChannel <= 64);
	ocean_assert(channels >= 1u && channels <= 31u);
	ocean_assert(planes >= 1u && planes <= 255u);

	if (channels == 0u || channels >= 32u || planes == 0u || planes >= 256u || widthMultiple == 0u || widthMultiple >= 256u || heightMultiple == 0u || heightMultiple >= 256u)
	{
		return FORMAT_UNDEFINED;
	}

	switch (bitsPerPixelChannel)
	{
		case 8u:
			return FrameType::genericPixelFormat(DT_UNSIGNED_INTEGER_8, channels, planes, widthMultiple, heightMultiple);

		case 16u:
			return FrameType::genericPixelFormat(DT_UNSIGNED_INTEGER_16, channels, planes, widthMultiple, heightMultiple);

		case 32u:
			return FrameType::genericPixelFormat(DT_UNSIGNED_INTEGER_32, channels, planes, widthMultiple, heightMultiple);

		case 64u:
			return FrameType::genericPixelFormat(DT_UNSIGNED_INTEGER_64, channels, planes, widthMultiple, heightMultiple);
	}

	ocean_assert(false && "Invalid bits per pixel value!");
	return FORMAT_UNDEFINED;
}

unsigned int FrameType::formatBitsPerPixelRedChannel(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
			return 0u;

		case FORMAT_ABGR32:
		case FORMAT_ARGB32:
		case FORMAT_BGR24:
		case FORMAT_BGR32:
		case FORMAT_BGRA32:
		case FORMAT_RGB24:
		case FORMAT_RGB32:
		case FORMAT_RGBA32:
		case FORMAT_RGBT32:
			return 8u;

		case FORMAT_BGR4444:
		case FORMAT_BGRA4444:
		case FORMAT_RGB4444:
		case FORMAT_RGBA4444:
			return 4u;

		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
			return 5u;

		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		case FORMAT_Y8_LIMITED_RANGE:
		case FORMAT_Y10:
		case FORMAT_Y10_PACKED:
		case FORMAT_Y16:
		case FORMAT_Y32:
		case FORMAT_Y64:
		case FORMAT_YA16:
		case FORMAT_YUV24:
		case FORMAT_YUVA32:
		case FORMAT_YUVT32:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_YVU24:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_F32:
		case FORMAT_F64:
			return 0u;

		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGGB10_PACKED:
			return 10u;

		case FORMAT_RGB48:
		case FORMAT_RGBA64:
			return 16u;

		case FORMAT_END:
			break;
	}

	ocean_assert(false && "Unknown pixel format.");
	return 0;
}

unsigned int FrameType::formatBitsPerPixelGreenChannel(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
			return 0u;

		case FORMAT_ABGR32:
		case FORMAT_ARGB32:
		case FORMAT_BGR24:
		case FORMAT_BGR32:
		case FORMAT_BGRA32:
		case FORMAT_RGB24:
		case FORMAT_RGB32:
		case FORMAT_RGBA32:
		case FORMAT_RGBT32:
			return 8u;

		case FORMAT_BGR4444:
		case FORMAT_BGRA4444:
		case FORMAT_RGB4444:
		case FORMAT_RGBA4444:
			return 4u;

		case FORMAT_BGR5551:
		case FORMAT_RGB5551:
			return 5u;

		case FORMAT_BGR565:
		case FORMAT_RGB565:
			return 6u;

		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		case FORMAT_Y8_LIMITED_RANGE:
		case FORMAT_Y10:
		case FORMAT_Y10_PACKED:
		case FORMAT_Y16:
		case FORMAT_Y32:
		case FORMAT_Y64:
		case FORMAT_YA16:
		case FORMAT_YUV24:
		case FORMAT_YUVA32:
		case FORMAT_YUVT32:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_YVU24:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_F32:
		case FORMAT_F64:
			return 0u;

		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGGB10_PACKED:
			return 10u;

		case FORMAT_RGB48:
		case FORMAT_RGBA64:
			return 16u;

		case FORMAT_END:
			break;
	}

	ocean_assert(false && "Unknown pixel format.");
	return 0;
}

unsigned int FrameType::formatBitsPerPixelBlueChannel(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
			return 0u;

		case FORMAT_ABGR32:
		case FORMAT_ARGB32:
		case FORMAT_BGR24:
		case FORMAT_BGR32:
		case FORMAT_BGRA32:
		case FORMAT_RGB24:
		case FORMAT_RGB32:
		case FORMAT_RGBA32:
		case FORMAT_RGBT32:
			return 8u;

		case FORMAT_BGR4444:
		case FORMAT_BGRA4444:
		case FORMAT_RGB4444:
		case FORMAT_RGBA4444:
			return 4u;

		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
			return 5u;

		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		case FORMAT_Y8_LIMITED_RANGE:
		case FORMAT_Y10:
		case FORMAT_Y10_PACKED:
		case FORMAT_Y16:
		case FORMAT_Y32:
		case FORMAT_Y64:
		case FORMAT_YA16:
		case FORMAT_YUV24:
		case FORMAT_YUVA32:
		case FORMAT_YUVT32:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_YVU24:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_F32:
		case FORMAT_F64:
			return 0u;

		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGGB10_PACKED:
			return 10u;

		case FORMAT_RGB48:
		case FORMAT_RGBA64:
			return 16u;

		case FORMAT_END:
			break;
	}

	ocean_assert(false && "Unknown pixel format.");
	return 0;
}

unsigned int FrameType::formatBitsPerPixelAlphaChannel(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
		case FORMAT_BGR24:
		case FORMAT_BGR32:
		case FORMAT_BGR4444:
		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGB24:
		case FORMAT_RGB32:
		case FORMAT_RGB48:
		case FORMAT_RGBT32:
		case FORMAT_RGB4444:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
		case FORMAT_RGGB10_PACKED:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_YUV24:
		case FORMAT_YUVT32:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_YVU24:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		case FORMAT_Y8_LIMITED_RANGE:
		case FORMAT_Y10:
		case FORMAT_Y10_PACKED:
		case FORMAT_Y16:
		case FORMAT_Y32:
		case FORMAT_Y64:
		case FORMAT_F32:
		case FORMAT_F64:
			return 0u;

		case FORMAT_ABGR32:
		case FORMAT_ARGB32:
		case FORMAT_BGRA32:
		case FORMAT_RGBA32:
		case FORMAT_YA16:
		case FORMAT_YUVA32:
			return 8u;

		case FORMAT_BGRA4444:
		case FORMAT_RGBA4444:
			return 4u;

		case FORMAT_RGBA64:
			return 16u;

		case FORMAT_END:
			break;
	}

	ocean_assert(false && "Unknown pixel format.");
	return 0u;
}

bool FrameType::formatHasAlphaChannel(const PixelFormat pixelFormat, bool* isLastChannel)
{
	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
		case FORMAT_BGR24:
		case FORMAT_BGR32:
		case FORMAT_BGR4444:
		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGB24:
		case FORMAT_RGB32:
		case FORMAT_RGB48:
		case FORMAT_RGB4444:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
		case FORMAT_RGBT32:
		case FORMAT_RGGB10_PACKED:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_YUV24:
		case FORMAT_YUVT32:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_YVU24:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		case FORMAT_Y8_LIMITED_RANGE:
		case FORMAT_Y10:
		case FORMAT_Y10_PACKED:
		case FORMAT_Y16:
		case FORMAT_Y32:
		case FORMAT_Y64:
		case FORMAT_F32:
		case FORMAT_F64:
			ocean_assert(formatBitsPerPixelAlphaChannel(pixelFormat) == 0u);
			return false;

		case FORMAT_ABGR32:
		case FORMAT_ARGB32:
		{
			ocean_assert(formatBitsPerPixelAlphaChannel(pixelFormat) != 0u);

			if (isLastChannel)
			{
				*isLastChannel = false;
			}

			return true;
		}

		case FORMAT_BGRA32:
		case FORMAT_BGRA4444:
		case FORMAT_RGBA32:
		case FORMAT_RGBA4444:
		case FORMAT_RGBA64:
		case FORMAT_YA16:
		case FORMAT_YUVA32:
		{
			ocean_assert(formatBitsPerPixelAlphaChannel(pixelFormat) != 0u);

			if (isLastChannel)
			{
				*isLastChannel = true;
			}

			return true;
		}

		case FORMAT_END:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	// a pure generic pixel format does not have an alpha channel
	if (FrameType::formatIsPureGeneric(pixelFormat))
	{
		return false;
	}

	ocean_assert(false && "Unknown pixel format.");
	return false;
}

bool FrameType::formatIsPacked(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGGB10_PACKED:
		case FORMAT_Y10_PACKED:
			return true;

		default:
			break;
	}

	return false;
}

FrameType::PixelFormat FrameType::genericSinglePlanePixelFormat(const PixelFormat pixelFormat)
{
	if (formatGenericNumberChannels(pixelFormat) != 0u)
	{
		ocean_assert(dataType(pixelFormat) != DT_UNDEFINED);
		return pixelFormat;
	}

	switch (pixelFormat)
	{
		case FORMAT_BGR4444:
		case FORMAT_BGR5551:
		case FORMAT_BGR565:
			return FORMAT_BGR24;

		case FORMAT_BGRA4444:
			return FORMAT_BGRA32;

		case FORMAT_RGB4444:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
			return FORMAT_RGB24;

		case FORMAT_RGBA4444:
			return FORMAT_RGBA32;

		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
			return FORMAT_YUV24;

		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
			return FORMAT_YVU24;

		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
			return FORMAT_YUV24;

		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
			return FORMAT_YVU24;

		default:
			ocean_assert(false && "Unknown pixel format.");
			break;
	}

	return FORMAT_UNDEFINED;
}

FrameType::PixelFormat FrameType::formatAddAlphaChannel(const PixelFormat pixelFormat, const bool lastChannel)
{
	switch (pixelFormat)
	{
		case FORMAT_BGR24:
		{
			if (lastChannel)
			{
				return FORMAT_BGRA32;
			}
			else
			{
				return FORMAT_ABGR32;
			}
		}

		case FORMAT_RGB24:
		{
			if (lastChannel)
			{
				return FORMAT_RGBA32;
			}
			else
			{
				return FORMAT_ARGB32;
			}
		}

		case FORMAT_RGB48:
		{
			if (lastChannel)
			{
				return FORMAT_RGBA64;
			}

			break;
		}

		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		{
			if (lastChannel)
			{
				return FORMAT_YA16;
			}

			break;
		}

		case FORMAT_YUV24:
		{
			if (lastChannel)
			{
				return FORMAT_YUVA32;
			}

			break;
		}

		case FORMAT_BGRA32:
		case FORMAT_ABGR32:
		case FORMAT_RGBA32:
		case FORMAT_RGBA64:
		case FORMAT_ARGB32:
		case FORMAT_YA16:
		case FORMAT_YUVA32:
		{
			return pixelFormat;
		}

		default:
		{
			ocean_assert(false && "Unknown pixel format.");
			return FORMAT_UNDEFINED;
		}
	}

	ocean_assert(false && "Unknown pixel format.");
	return FORMAT_UNDEFINED;
}

FrameType::PixelFormat FrameType::formatRemoveAlphaChannel(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_ABGR32:
			return FORMAT_BGR24;

		case FORMAT_ARGB32:
			return FORMAT_RGB24;

		case FORMAT_BGRA32:
			return FORMAT_BGR24;

		case FORMAT_RGBA32:
			return FORMAT_RGB24;

		case FORMAT_RGBA64:
			return FORMAT_RGB48;

		case FORMAT_YA16:
			return FORMAT_Y8; // FORMAT_Y8_FULL_RANGE

		case FORMAT_YUVA32:
			return FORMAT_YUV24;

		case FORMAT_BGR24:
		case FORMAT_BGR32:
		case FORMAT_BGR4444:
		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGB24:
		case FORMAT_RGB32:
		case FORMAT_RGB4444:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
		case FORMAT_RGBT32:
		case FORMAT_RGGB10_PACKED:
		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
		case FORMAT_Y_U_V24_FULL_RANGE:
		case FORMAT_YUV24:
		case FORMAT_YUVT32:
		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
		case FORMAT_Y_V_U12_FULL_RANGE:
		case FORMAT_YVU24:
		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
		case FORMAT_Y_VU12_FULL_RANGE:
		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
		case FORMAT_Y8_LIMITED_RANGE:
		case FORMAT_Y10:
		case FORMAT_Y10_PACKED:
		case FORMAT_Y16:
		case FORMAT_Y32:
		case FORMAT_Y64:
		case FORMAT_F32:
		case FORMAT_F64:
			return pixelFormat;

		default:
			ocean_assert(false && "Unknown pixel format.");
			return FORMAT_UNDEFINED;
	}
}

unsigned int FrameType::planeChannels(const PixelFormat& imagePixelFormat, const unsigned int planeIndex)
{
	unsigned int planeWidthDummy;
	unsigned int planeHeightDummy;
	unsigned int planeChannels;

	if (!planeLayout(imagePixelFormat, widthMultiple(imagePixelFormat), heightMultiple(imagePixelFormat), planeIndex, planeWidthDummy, planeHeightDummy, planeChannels))
	{
		return 0u;
	}

	return planeChannels;
}

bool FrameType::planeLayout(const PixelFormat imagePixelFormat, const unsigned int imageWidth, const unsigned int imageHeight, const unsigned int planeIndex, unsigned int& planeWidth, unsigned int& planeHeight, unsigned int& planeChannels, unsigned int* planeWidthElementsMultiple, unsigned int* planeHeightElementsMultiple)
{
	ocean_assert(imagePixelFormat != FORMAT_UNDEFINED);
	ocean_assert(imageWidth != 0u && imageHeight != 0u);

	if (formatIsGeneric(imagePixelFormat))
	{
		// the pixel format is generic (e.g, FORMAT_RGB24, or FORMAT_Y8) and each pixel channel is composed of one data type

		if (planeIndex >= numberPlanes(imagePixelFormat))
		{
			return false;
		}

		planeWidth = imageWidth;
		planeHeight = imageHeight;
		planeChannels = channels(imagePixelFormat);

		if (planeWidthElementsMultiple != nullptr)
		{
			*planeWidthElementsMultiple = 1u;
		}

		if (planeHeightElementsMultiple != nullptr)
		{
			*planeHeightElementsMultiple = 1u;
		}

		return true;
	}

	const unsigned int widthMultiple = FrameType::widthMultiple(imagePixelFormat);
	const unsigned int heightMultiple = FrameType::heightMultiple(imagePixelFormat);

	if (widthMultiple == 0u || heightMultiple == 0u || (imageWidth % widthMultiple != 0u) || (imageHeight % heightMultiple != 0u))
	{
		ocean_assert(false && "Invalid pixel format or image resolutions!");
		return false;
	}

	switch (imagePixelFormat)
	{
		case FORMAT_BGR4444:
		case FORMAT_BGRA4444:
		case FORMAT_RGB4444:
		case FORMAT_RGBA4444:
		case FORMAT_BGR5551:
		case FORMAT_BGR565:
		case FORMAT_RGB5551:
		case FORMAT_RGB565:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_16);

			if (planeIndex != 0u)
			{
				return false;
			}

			// one data type covers one pixel
			planeWidth = imageWidth;
			planeHeight = imageHeight;
			planeChannels = 1u;

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 1u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 1u;
			}

			return true;
		}

		case FORMAT_BGGR10_PACKED:
		case FORMAT_RGGB10_PACKED:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_8);

			if (planeIndex != 0u)
			{
				return false;
			}

			ocean_assert(imageWidth % 4u == 0u);
			ocean_assert(imageHeight % 2u == 0u);

			// four pixels cover 5 bytes
			planeWidth = (imageWidth / 4u) * 5u;
			planeHeight = imageHeight;
			planeChannels = 1u;

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 5u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 2u;
			}

			return true;
		}

		case FORMAT_Y10_PACKED:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_8);

			if (planeIndex != 0u)
			{
				return false;
			}

			ocean_assert(imageWidth % 4u == 0u);

			// four pixels cover 5 bytes
			planeWidth = (imageWidth / 4u) * 5u;
			planeHeight = imageHeight;
			planeChannels = 1u;

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 5u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 1u;
			}

			return true;
		}

		case FORMAT_Y_U_V12:
		case FORMAT_Y_V_U12:
		case FORMAT_Y_U_V12_FULL_RANGE:
		case FORMAT_Y_V_U12_FULL_RANGE:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_8);
			ocean_assert(widthMultiple == 2u);
			ocean_assert(heightMultiple == 2u);

			if (planeIndex > 2u)
			{
				return false;
			}

			planeChannels = 1u;

			if (planeIndex == 0u)
			{
				planeWidth = imageWidth;
				planeHeight = imageHeight;
			}
			else
			{
				ocean_assert(planeIndex == 1u || planeIndex == 2u);

				planeWidth = imageWidth / 2u;
				planeHeight = imageHeight / 2u;
			}

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 1u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 1u;
			}

			return true;
		}

		case FORMAT_Y_UV12:
		case FORMAT_Y_VU12:
		case FORMAT_Y_UV12_FULL_RANGE:
		case FORMAT_Y_VU12_FULL_RANGE:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_8);
			ocean_assert(widthMultiple == 2u);
			ocean_assert(heightMultiple == 2u);

			if (planeIndex > 2u)
			{
				return false;
			}

			if (planeIndex == 0u)
			{
				planeWidth = imageWidth;
				planeHeight = imageHeight;
				planeChannels = 1u;
			}
			else
			{
				ocean_assert(planeIndex == 1u);

				planeWidth = imageWidth / 2u;
				planeHeight = imageHeight / 2u;
				planeChannels = 2u;
			}

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 1u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 1u;
			}

			return true;
		}

		case FORMAT_UYVY16:
		case FORMAT_YUYV16:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_8);
			ocean_assert(widthMultiple == 2u);
			ocean_assert(heightMultiple == 1u);

			if (planeIndex != 0u)
			{
				return false;
			}

			planeWidth = imageWidth;
			planeHeight = imageHeight;
			planeChannels = 2u;

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 1u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 1u;
			}

			ocean_assert(planeWidth * planeHeight * planeChannels * 8u == imageWidth * imageHeight * 16u);

			return true;
		}

		case FORMAT_Y_U_V24:
		case FORMAT_Y_U_V24_FULL_RANGE:
		{
			ocean_assert(dataType(imagePixelFormat) == DT_UNSIGNED_INTEGER_8);

			if (planeIndex > 2u)
			{
				return false;
			}

			ocean_assert(planeIndex == 0u || planeIndex == 1u || planeIndex == 2u);

			// we treat the three planes as one
			planeWidth = imageWidth;
			planeHeight = imageHeight;
			planeChannels = 1u;

			if (planeWidthElementsMultiple != nullptr)
			{
				*planeWidthElementsMultiple = 1u;
			}

			if (planeHeightElementsMultiple != nullptr)
			{
				*planeHeightElementsMultiple = 1u;
			}

			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

FrameType::DataType FrameType::translateDataType(const std::string& dataType)
{
	const std::string upperValue = String::toUpper(dataType);

	if (upperValue == "UNDEFINED")
		return DT_UNDEFINED;

	if (upperValue == "UNSIGNED_INTEGER_8")
		return DT_UNSIGNED_INTEGER_8;

	if (upperValue == "SIGNED_INTEGER_8")
		return DT_SIGNED_INTEGER_8;

	if (upperValue == "UNSIGNED_INTEGER_16")
		return DT_UNSIGNED_INTEGER_16;

	if (upperValue == "SIGNED_INTEGER_16")
		return DT_SIGNED_INTEGER_16;

	if (upperValue == "UNSIGNED_INTEGER_32")
		return DT_UNSIGNED_INTEGER_32;

	if (upperValue == "SIGNED_INTEGER_32")
		return DT_SIGNED_INTEGER_32;

	if (upperValue == "UNSIGNED_INTEGER_64")
		return DT_UNSIGNED_INTEGER_64;

	if (upperValue == "SIGNED_INTEGER_64")
		return DT_SIGNED_INTEGER_64;

	if (upperValue == "SIGNED_FLOAT_16")
		return DT_SIGNED_FLOAT_16;

	if (upperValue == "SIGNED_FLOAT_32")
		return DT_SIGNED_FLOAT_32;

	if (upperValue == "SIGNED_FLOAT_64")
		return DT_SIGNED_FLOAT_64;

	ocean_assert(false && "Invalid input!");
	return DT_UNDEFINED;
}

FrameType::PixelFormat FrameType::translatePixelFormat(const std::string& pixelFormat)
{
	const std::string upperValue = String::toUpper(pixelFormat);

	if (upperValue == "UNDEFINED")
		return FORMAT_UNDEFINED;

	if (upperValue == "ABGR32")
		return FORMAT_ABGR32;

	if (upperValue == "ARGB32")
		return FORMAT_ARGB32;

	if (upperValue == "BGR24")
		return FORMAT_BGR24;

	if (upperValue == "BGR32")
		return FORMAT_BGR32;

	if (upperValue == "BGR4444")
		return FORMAT_BGR4444;

	if (upperValue == "BGR5551")
		return FORMAT_BGR5551;

	if (upperValue == "BGR565")
		return FORMAT_BGR565;

	if (upperValue == "BGRA32")
		return FORMAT_BGRA32;

	if (upperValue == "BGRA4444")
		return FORMAT_BGRA4444;

	if (upperValue == "BGGR10_PACKED")
		return FORMAT_BGGR10_PACKED;

	if (upperValue == "RGB24")
		return FORMAT_RGB24;

	if (upperValue == "RGB32")
		return FORMAT_RGB32;

	if (upperValue == "RGB4444")
		return FORMAT_RGB4444;

	if (upperValue == "RGB5551")
		return FORMAT_RGB5551;

	if (upperValue == "RGB565")
		return FORMAT_RGB565;

	if (upperValue == "RGBA32")
		return FORMAT_RGBA32;

	if (upperValue == "RGB48")
		return FORMAT_RGB48;

	if (upperValue == "RGBA64")
		return FORMAT_RGBA64;

	if (upperValue == "RGBT32")
		return FORMAT_RGBT32;

	if (upperValue == "RGBA4444")
		return FORMAT_RGBA4444;

	if (upperValue == "RGGB10_PACKED")
		return FORMAT_RGGB10_PACKED;

	if (upperValue == "Y_U_V12")
		return FORMAT_Y_U_V12;

	if (upperValue == "FORMAT_Y_U_V12_LIMITED_RANGE")
		return FORMAT_Y_U_V12_LIMITED_RANGE;

	if (upperValue == "FORMAT_Y_U_V12_FULL_RANGE")
		return FORMAT_Y_U_V12_FULL_RANGE;

	if (upperValue == "Y_U_V24")
		return FORMAT_Y_U_V24;

	if (upperValue == "FORMAT_Y_U_V24_LIMITED_RANGE")
		return FORMAT_Y_U_V24_LIMITED_RANGE;

	if (upperValue == "FORMAT_Y_U_V24_FULL_RANGE")
		return FORMAT_Y_U_V24_FULL_RANGE;

	if (upperValue == "YUV24")
		return FORMAT_YUV24;

	if (upperValue == "YUVA32")
		return FORMAT_YUVA32;

	if (upperValue == "YUVT32")
		return FORMAT_YUVT32;

	if (upperValue == "Y_V_U12")
		return FORMAT_Y_V_U12;

	if (upperValue == "FORMAT_Y_V_U12_LIMITED_RANGE")
		return FORMAT_Y_V_U12_LIMITED_RANGE;

	if (upperValue == "FORMAT_Y_V_U12_FULL_RANGE")
		return FORMAT_Y_V_U12_FULL_RANGE;

	if (upperValue == "YVU24")
		return FORMAT_YVU24;

	if (upperValue == "Y_UV12")
		return FORMAT_Y_UV12;

	if (upperValue == "FORMAT_Y_UV12_LIMITED_RANGE")
		return FORMAT_Y_UV12_LIMITED_RANGE;

	if (upperValue == "FORMAT_Y_UV12_FULL_RANGE")
		return FORMAT_Y_UV12_FULL_RANGE;

	if (upperValue == "Y_VU12")
		return FORMAT_Y_VU12;

	if (upperValue == "FORMAT_Y_VU12_LIMITED_RANGE")
		return FORMAT_Y_VU12_LIMITED_RANGE;

	if (upperValue == "FORMAT_Y_VU12_FULL_RANGE")
		return FORMAT_Y_VU12_FULL_RANGE;

	if (upperValue == "UYVY16")
		return FORMAT_UYVY16;

	if (upperValue == "YUYV16")
		return FORMAT_YUYV16;

	if (upperValue == "Y8")
		return FORMAT_Y8;

	if (upperValue == "FORMAT_Y8_LIMITED_RANGE")
		return FORMAT_Y8_LIMITED_RANGE;

	if (upperValue == "FORMAT_Y8_FULL_RANGE")
		return FORMAT_Y8_FULL_RANGE;

	if (upperValue == "Y10")
		return FORMAT_Y10;

	if (upperValue == "Y10_PACKED")
		return FORMAT_Y10_PACKED;

	if (upperValue == "Y16")
		return FORMAT_Y16;

	if (upperValue == "Y32")
		return FORMAT_Y32;

	if (upperValue == "Y64")
		return FORMAT_Y64;

	if (upperValue == "YA16")
		return FORMAT_YA16;

	if (upperValue == "F32")
		return FORMAT_F32;

	if (upperValue == "F64")
		return FORMAT_F64;

	ocean_assert(false && "Invalid input!");
	return FORMAT_UNDEFINED;
}

FrameType::PixelOrigin FrameType::translatePixelOrigin(const std::string& pixelOrigin)
{
	const std::string upperValue = String::toUpper(pixelOrigin);

	if (upperValue == "INVALID")
		return ORIGIN_INVALID;

	if (upperValue == "UPPER_LEFT")
		return ORIGIN_UPPER_LEFT;

	if (upperValue == "LOWER_LEFT")
		return ORIGIN_LOWER_LEFT;

	ocean_assert(false && "Invalid input!");
	return ORIGIN_INVALID;
}

std::string FrameType::translateDataType(const DataType dataType)
{
	switch (dataType)
	{
		case DT_UNDEFINED:
			return "UNDEFINED";

		case DT_UNSIGNED_INTEGER_8:
			return "UNSIGNED_INTEGER_8";

		case DT_SIGNED_INTEGER_8:
			return "SIGNED_INTEGER_8";

		case DT_UNSIGNED_INTEGER_16:
			return "UNSIGNED_INTEGER_16";

		case DT_SIGNED_INTEGER_16:
			return "SIGNED_INTEGER_16";

		case DT_UNSIGNED_INTEGER_32:
			return "UNSIGNED_INTEGER_32";

		case DT_SIGNED_INTEGER_32:
			return "SIGNED_INTEGER_32";

		case DT_UNSIGNED_INTEGER_64:
			return "UNSIGNED_INTEGER_64";

		case DT_SIGNED_INTEGER_64:
			return "SIGNED_INTEGER_64";

		case DT_SIGNED_FLOAT_16:
			return "SIGNED_FLOAT_16";

		case DT_SIGNED_FLOAT_32:
			return "SIGNED_FLOAT_32";

		case DT_SIGNED_FLOAT_64:
			return "SIGNED_FLOAT_64";

		case DT_END:
			break;
	}

	ocean_assert(false && "Invalid pixel format.");
	return "UNDEFINED";
}

std::string FrameType::translatePixelFormat(const PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FORMAT_UNDEFINED:
			return "UNDEFINED";

		case FORMAT_ABGR32:
			return "ABGR32";

		case FORMAT_ARGB32:
			return "ARGB32";

		case FORMAT_BGR24:
			return "BGR24";

		case FORMAT_BGR32:
			return "BGR32";

		case FORMAT_BGR4444:
			return "BGR4444";

		case FORMAT_BGR5551:
			return "BGR5551";

		case FORMAT_BGR565:
			return "BGR565";

		case FORMAT_BGRA32:
			return "BGRA32";

		case FORMAT_BGRA4444:
			return "BGRA4444";

		case FORMAT_BGGR10_PACKED:
			return "BGGR10_PACKED";

		case FORMAT_RGB24:
			return "RGB24";

		case FORMAT_RGB32:
			return "RGB32";

		case FORMAT_RGB4444:
			return "RGB4444";

		case FORMAT_RGB5551:
			return "RGB5551";

		case FORMAT_RGB565:
			return "RGB565";

		case FORMAT_RGBA32:
			return "RGBA32";

		case FORMAT_RGB48:
			return "RGB48";

		case FORMAT_RGBA64:
			return "RGBA64";

		case FORMAT_RGBT32:
			return "RGBT32";

		case FORMAT_RGBA4444:
			return "RGBA4444";

		case FORMAT_RGGB10_PACKED:
			return "RGGB10_PACKED";

		case FORMAT_Y_U_V12: // FORMAT_Y_U_V12_LIMITED_RANGE
			return "Y_U_V12";

		case FORMAT_Y_U_V12_FULL_RANGE:
			return "FORMAT_Y_U_V12_FULL_RANGE";

		case FORMAT_Y_U_V24: // FORMAT_Y_U_V24_LIMITED_RANGE
			return "Y_U_V24";

		case FORMAT_Y_U_V24_FULL_RANGE:
			return "FORMAT_Y_U_V24_FULL_RANGE";

		case FORMAT_YUV24:
			return "YUV24";

		case FORMAT_YUVA32:
			return "YUVA32";

		case FORMAT_YUVT32:
			return "YUVT32";

		case FORMAT_Y_V_U12: // FORMAT_Y_V_U12_LIMITED_RANGE
			return "Y_V_U12";

		case FORMAT_Y_V_U12_FULL_RANGE:
			return "FORMAT_Y_V_U12_FULL_RANGE";

		case FORMAT_YVU24:
			return "YVU24";

		case FORMAT_Y_UV12: // FORMAT_Y_UV12_LIMITED_RANGE
			return "Y_UV12";

		case FORMAT_Y_UV12_FULL_RANGE:
			return "FORMAT_Y_UV12_FULL_RANGE";

		case FORMAT_Y_VU12: // FORMAT_Y_VU12_LIMITED_RANGE
			return "Y_VU12";

		case FORMAT_Y_VU12_FULL_RANGE:
			return "FORMAT_Y_VU12_FULL_RANGE";

		case FORMAT_UYVY16:
			return "UYVY16";

		case FORMAT_YUYV16:
			return "YUYV16";

		case FORMAT_Y8: // FORMAT_Y8_FULL_RANGE
			return "Y8";

		case FORMAT_Y8_LIMITED_RANGE:
			return "FORMAT_Y8_LIMITED_RANGE";

		case FORMAT_Y10:
			return "Y10";

		case FORMAT_Y10_PACKED:
			return "Y10_PACKED";

		case FORMAT_Y16:
			return "Y16";

		case FORMAT_Y32:
			return "Y32";

		case FORMAT_Y64:
			return "Y64";

		case FORMAT_YA16:
			return "YA16";

		case FORMAT_F32:
			return "F32";

		case FORMAT_F64:
			return "F64";

		case FORMAT_END:
			break;
	}

	ocean_assert(false && "Invalid pixel format.");
	return "UNDEFINED";
}

std::string FrameType::translatePixelOrigin(const PixelOrigin pixelOrigin)
{
	switch (pixelOrigin)
	{
		case ORIGIN_UPPER_LEFT:
			return "UPPER_LEFT";

		case ORIGIN_LOWER_LEFT:
			return "LOWER_LEFT";

		case ORIGIN_INVALID:
			return "INVALID";
	}

	ocean_assert(false && "Invalid pixel origin!");
	return "INVALID";
}

FrameType::PixelFormat FrameType::findPixelFormat(const unsigned int bitsPerPixel)
{
	switch (bitsPerPixel)
	{
		case 8u:
			return FORMAT_Y8;

		case 16u:
			return FORMAT_YA16;

		case 24u:
			return FORMAT_RGB24;

		case 32u:
			return FORMAT_RGBA32;

		case 64u:
			return FORMAT_Y64;

		case 128u:
			return genericPixelFormat<DT_UNSIGNED_INTEGER_64, 2u>();
	}

	ocean_assert(false && "Invalid number of bits per pixel.");
	return FORMAT_UNDEFINED;
}

FrameType::PixelFormat FrameType::findPixelFormat(const DataType dataType, const unsigned int channels)
{
	ocean_assert(dataType != DT_UNDEFINED);
	ocean_assert(channels >= 1u);

	switch ((((unsigned int)dataType) << 8u) | channels)
	{
		case ((((unsigned int)DT_UNSIGNED_INTEGER_8) << 8u) | 1u):
			return FORMAT_Y8;

		case ((((unsigned int)DT_UNSIGNED_INTEGER_8) << 8u) | 2u):
			return FORMAT_YA16;

		case ((((unsigned int)DT_UNSIGNED_INTEGER_8) << 8u) | 3u):
			return FORMAT_RGB24;

		case ((((unsigned int)DT_UNSIGNED_INTEGER_8) << 8u) | 4u):
			return FORMAT_RGBA32;

		case ((((unsigned int)DT_UNSIGNED_INTEGER_16) << 8u) | 3u):
			return FORMAT_RGBA64;

		case ((((unsigned int)DT_UNSIGNED_INTEGER_16) << 8u) | 4u):
			return FORMAT_RGBA64;
	}

	return FORMAT_UNDEFINED;
}

bool FrameType::arePixelFormatsCompatible(const PixelFormat pixelFormatA, const PixelFormat pixelFormatB)
{
	ocean_assert(pixelFormatA != FORMAT_UNDEFINED && pixelFormatB != FORMAT_UNDEFINED);

	return pixelFormatA == pixelFormatB
				|| (formatIsGeneric(pixelFormatA) && formatIsGeneric(pixelFormatB) && (formatIsPureGeneric(pixelFormatA) || formatIsPureGeneric(pixelFormatB)) && dataType(pixelFormatA) == dataType(pixelFormatB) && channels(pixelFormatA) == channels(pixelFormatB));
}

bool FrameType::areFrameTypesCompatible(const FrameType& frameTypeA, const FrameType& frameTypeB, const bool allowDifferentPixelOrigins)
{
	return frameTypeA.width() == frameTypeB.width() && frameTypeA.height() == frameTypeB.height()
				&& (allowDifferentPixelOrigins || frameTypeA.pixelOrigin() == frameTypeB.pixelOrigin())
				&& arePixelFormatsCompatible(frameTypeA.pixelFormat(), frameTypeB.pixelFormat());
}

unsigned int FrameType::frameTypeSize() const
{
	const unsigned int bytesPerElement = bytesPerDataType();

	unsigned int sumBytes = 0u;

	unsigned int planeWidth = 0u;
	unsigned int planeHeight = 0u;
	unsigned int planeChannels = 0u;

	for (unsigned int planeIndex = 0u; planeIndex < numberPlanes(); ++planeIndex)
	{
		if (!planeLayout(*this, planeIndex, planeWidth, planeHeight, planeChannels))
		{
			ocean_assert(false && "This should never happen!");
			return 0u;
		}

		sumBytes += planeWidth * planeHeight * planeChannels * bytesPerElement;
	}

	return sumBytes;
}

bool FrameType::operator==(const FrameType& right) const
{
	return width_ == right.width_ && height_ == right.height_
		&& pixelFormat_.pixelFormat_ == right.pixelFormat_.pixelFormat_ && pixelOrigin_ == right.pixelOrigin_;
}

bool FrameType::operator<(const FrameType& right) const
{
	if (width_ < right.width_)
	{
		return true;
	}

	if (width_ > right.width_)
	{
		return false;
	}

	if (height_ < right.height_)
	{
		return true;
	}

	if (height_ > right.height_)
	{
		return false;
	}

	if (pixelFormat_.pixelFormat_ < right.pixelFormat_.pixelFormat_)
	{
		return true;
	}

	if (pixelFormat_.pixelFormat_ > right.pixelFormat_.pixelFormat_)
	{
		return false;
	}

	return pixelOrigin_ < right.pixelOrigin_;
}

const FrameType::DataTypes& FrameType::definedDataTypes()
{
	static const FrameType::DataTypes dataTypes =
	{
		DT_UNSIGNED_INTEGER_8,
		DT_SIGNED_INTEGER_8,
		DT_UNSIGNED_INTEGER_16,
		DT_SIGNED_INTEGER_16,
		DT_UNSIGNED_INTEGER_32,
		DT_SIGNED_INTEGER_32,
		DT_UNSIGNED_INTEGER_64,
		DT_SIGNED_INTEGER_64,
		DT_SIGNED_FLOAT_16,
		DT_SIGNED_FLOAT_32,
		DT_SIGNED_FLOAT_64,
	};

	if (dataTypes.size() != size_t(FrameType::DT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
	}

	return dataTypes;
}

const FrameType::PixelFormats& FrameType::definedPixelFormats()
{
	static const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_ARGB32,
		FrameType::FORMAT_BGR24,
		FrameType::FORMAT_BGR32,
		FrameType::FORMAT_BGR4444,
		FrameType::FORMAT_BGR5551,
		FrameType::FORMAT_BGR565,
		FrameType::FORMAT_BGRA32,
		FrameType::FORMAT_BGRA4444,
		FrameType::FORMAT_BGGR10_PACKED,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGB32,
		FrameType::FORMAT_RGB4444,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_RGB565,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_RGBA4444,
		FrameType::FORMAT_RGBT32,
		FrameType::FORMAT_RGGB10_PACKED,
		FrameType::FORMAT_UYVY16,
		FrameType::FORMAT_YUV24,
		FrameType::FORMAT_YUVA32,
		FrameType::FORMAT_YUVT32,
		FrameType::FORMAT_YVU24,
		FrameType::FORMAT_YUYV16,
		FrameType::FORMAT_Y16,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_Y64,
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_RGB48,
		FrameType::FORMAT_RGBA64,
		FrameType::FORMAT_Y_U_V24_LIMITED_RANGE,
		FrameType::FORMAT_Y_U_V24_FULL_RANGE,
		FrameType::FORMAT_Y8_LIMITED_RANGE,
		FrameType::FORMAT_Y8_FULL_RANGE,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y10_PACKED,
		FrameType::FORMAT_Y_UV12_LIMITED_RANGE,
		FrameType::FORMAT_Y_UV12_FULL_RANGE,
		FrameType::FORMAT_Y_VU12_LIMITED_RANGE,
		FrameType::FORMAT_Y_VU12_FULL_RANGE,
		FrameType::FORMAT_Y_U_V12_LIMITED_RANGE,
		FrameType::FORMAT_Y_U_V12_FULL_RANGE,
		FrameType::FORMAT_Y_V_U12_LIMITED_RANGE,
		FrameType::FORMAT_Y_V_U12_FULL_RANGE,
		FrameType::FORMAT_F32,
		FrameType::FORMAT_F64,
	};

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
	}

	return pixelFormats;
}

Frame::Plane::Plane(const Plane& plane, const AdvancedCopyMode advancedCopyMode) noexcept
{
	const bool result = copy(plane, advancedCopyMode, true);
	ocean_assert_and_suppress_unused(result, result);

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());
}

Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const unsigned int paddingElements) noexcept :
	allocatedData_(nullptr),
	constData_(nullptr),
	data_(nullptr),
	width_(width),
	height_(height),
	channels_(channels),
	elementTypeSize_(elementTypeSize),
	paddingElements_(paddingElements)
{
	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();

	ocean_assert(width_ != 0u);
	ocean_assert(height_ != 0u);
	ocean_assert(channels_ != 0u);
	ocean_assert(elementTypeSize_ != 0u);

	allocatedData_ = alignedMemory(size(), elementTypeSize, data_);
	constData_ = data_;

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());
}

Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* dataToUse, const unsigned int paddingElements) noexcept :
	allocatedData_(nullptr),
	constData_(dataToUse),
	data_(nullptr),
	width_(width),
	height_(height),
	channels_(channels),
	elementTypeSize_(elementTypeSize),
	paddingElements_(paddingElements)
{
	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();

	ocean_assert(allocatedData_ == nullptr);
	ocean_assert(constData_ != nullptr);
	ocean_assert(data_ == nullptr);
	ocean_assert(width_ != 0u);
	ocean_assert(height_ != 0u);
	ocean_assert(channels_ != 0u);
	ocean_assert(elementTypeSize_ != 0u);

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());
}

Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, void* dataToUse, const unsigned int paddingElements) noexcept :
	allocatedData_(nullptr),
	constData_(dataToUse), // the we interpret the plane as read-only as well
	data_(dataToUse),
	width_(width),
	height_(height),
	channels_(channels),
	elementTypeSize_(elementTypeSize),
	paddingElements_(paddingElements)
{
	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();

	ocean_assert(allocatedData_ == nullptr);
	ocean_assert(constData_ != nullptr);
	ocean_assert(data_ != nullptr);
	ocean_assert(width_ != 0u);
	ocean_assert(height_ != 0u);
	ocean_assert(channels_ != 0u);
	ocean_assert(elementTypeSize_ != 0u);

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());
}

Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* sourceDataToCopy, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements, const bool makeCopyOfPaddingData) noexcept :
	allocatedData_(nullptr),
	constData_(nullptr),
	data_(nullptr),
	width_(width),
	height_(height),
	channels_(channels),
	elementTypeSize_(elementTypeSize),
	paddingElements_(targetPaddingElements)
{
	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();

	ocean_assert(elementTypeSize >= 1u);

	allocatedData_ = alignedMemory(size(), elementTypeSize, data_);
	constData_ = data_;

	const unsigned int sourceStrideBytes = (width * channels + sourcePaddingElements) * elementTypeSize;

	copy(sourceDataToCopy, sourceStrideBytes, sourcePaddingElements, makeCopyOfPaddingData);

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());
}

Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* sourceDataToCopy, const unsigned int sourcePaddingElements, const CopyMode copyMode) noexcept :
	allocatedData_(nullptr),
	constData_(nullptr),
	data_(nullptr),
	width_(width),
	height_(height),
	channels_(channels),
	elementTypeSize_(elementTypeSize),
	paddingElements_(0u)
{
	ocean_assert(elementTypeSize >= 1u);

	if (copyMode != CM_COPY_REMOVE_PADDING_LAYOUT)
	{
		ocean_assert(copyMode == CM_USE_KEEP_LAYOUT || copyMode == CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA || copyMode == CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		// all three copy modes keep the padding layout

		paddingElements_ = sourcePaddingElements;
	}

	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();

	if (copyMode == CM_USE_KEEP_LAYOUT)
	{
		constData_ = sourceDataToCopy;
	}
	else
	{
		ocean_assert(copyMode == CM_COPY_REMOVE_PADDING_LAYOUT || copyMode == CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA || copyMode == CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		// all three copy modes copy the memory

		allocatedData_ = alignedMemory(size(), elementTypeSize, data_);
		constData_ = data_;

		const unsigned int sourceStrideBytes = (width * channels + sourcePaddingElements) * elementTypeSize;

		const bool makeCopyOfPaddingData = copyMode == CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA;

		copy(sourceDataToCopy, sourceStrideBytes, sourcePaddingElements, makeCopyOfPaddingData);
	}

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());
}

void Frame::Plane::release()
{
	if (allocatedData_ != nullptr)
	{
		free(allocatedData_);
		allocatedData_ = nullptr;
	}

	constData_ = nullptr;
	data_ = nullptr;

	width_ = 0u;
	height_ = 0u;
	channels_ = 0u;
	elementTypeSize_ = 0u;
	paddingElements_ = 0u;
	strideBytes_ = 0u;
	bytesPerPixel_ = 0u;
}

bool Frame::Plane::copy(const Plane& sourcePlane, const AdvancedCopyMode advancedCopyMode, const bool reallocateIfNecessary)
{
	if (!sourcePlane.isValid())
	{
		release();
		return true;
	}

	if (advancedCopyMode == ACM_USE_KEEP_LAYOUT || (advancedCopyMode == ACM_USE_OR_COPY && !sourcePlane.isOwner()) || (advancedCopyMode == ACM_USE_OR_COPY_KEEP_LAYOUT && !sourcePlane.isOwner()))
	{
		// ACM_USE_KEEP_LAYOUT
		// The source memory is used only, no copy is created.

		// ACM_USE_OR_COPY
		// ACM_USE_OR_COPY_KEEP_LAYOUT
		// The source memory is used if the source is not owner of the memory;

		release();

		ocean_assert(allocatedData_ == nullptr);
		constData_ = sourcePlane.constData_;
		data_ = sourcePlane.data_;

		width_ = sourcePlane.width_;
		height_ = sourcePlane.height_;
		channels_ = sourcePlane.channels_;
		elementTypeSize_ = sourcePlane.elementTypeSize_;
		paddingElements_ = sourcePlane.paddingElements_;
		strideBytes_ = sourcePlane.strideBytes_;
		bytesPerPixel_ = sourcePlane.bytesPerPixel_;

		return true;
	}

	const unsigned int newPaddingElements = (advancedCopyMode == ACM_COPY_REMOVE_PADDING_LAYOUT || advancedCopyMode == ACM_USE_OR_COPY) ? 0u : sourcePlane.paddingElements_;

	const unsigned int newStrideBytes = sourcePlane.widthBytes() + newPaddingElements * sourcePlane.elementTypeSize_;
	const unsigned int newMemorySize = newStrideBytes * sourcePlane.height_;

	const unsigned int thisOldMemorySize = size();

	if (newMemorySize != thisOldMemorySize || !isOwner() || isReadOnly())
	{
		if (reallocateIfNecessary)
		{
			release();
		}
		else
		{
			return false;
		}
	}

	width_ = sourcePlane.width_;
	height_ = sourcePlane.height_;
	channels_ = sourcePlane.channels_;
	elementTypeSize_ = sourcePlane.elementTypeSize_;
	paddingElements_ = newPaddingElements;
	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();

	if (allocatedData_ == nullptr)
	{
		data_ = nullptr;
		allocatedData_ = alignedMemory(newMemorySize, elementTypeSize_, data_);

		if (allocatedData_ == nullptr)
		{
			return false;
		}

		constData_ = data_;
	}

	ocean_assert(allocatedData_ != nullptr);
	ocean_assert(constData_ != nullptr);
	ocean_assert(data_ != nullptr);

	ocean_assert(!isReadOnly());

	ocean_assert(width() == sourcePlane.width());
	ocean_assert(height() == sourcePlane.height());
	ocean_assert(channels() == sourcePlane.channels());
	ocean_assert(paddingElements() == newPaddingElements);
	ocean_assert(elementTypeSize() == sourcePlane.elementTypeSize());
	ocean_assert(widthBytes() == sourcePlane.widthBytes());

	const bool makeCopyOfPaddingData = advancedCopyMode == ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA;

#ifdef OCEAN_DEBUG

	{
		bool debugMakeCopyOfPaddingData = false;

		switch (advancedCopyMode)
		{
			case ACM_USE_OR_COPY:
			case ACM_USE_OR_COPY_KEEP_LAYOUT:
			{
				// The source memory is used if the source is not owner of the memory;
				// The source memory is copied if the source is owner of the memory, padding layout is preserved, but padding data is not copied.

				ocean_assert(sourcePlane.isOwner());

				debugMakeCopyOfPaddingData = false;
				break;
			}

			case ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
			{
				// Makes a copy of the source memory, the padding layout is preserved, but the padding data is not copyied.

				debugMakeCopyOfPaddingData = false;
				break;
			}

			case ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
			{
				// Makes a copy of the source memory, the padding layout is preserved, the padding data is copied as well.

				debugMakeCopyOfPaddingData = true;
				break;
			}

			case ACM_COPY_REMOVE_PADDING_LAYOUT:
			{
				// Makes a copy of the source memory, but the new plane will not contain padding elements.

				debugMakeCopyOfPaddingData = false;
				break;
			}

			default:
			{
				ocean_assert(false && "Invalid copy mode!");
				debugMakeCopyOfPaddingData = false;
				break;
			}
		}

		ocean_assert(debugMakeCopyOfPaddingData == makeCopyOfPaddingData);
	}

#endif // OCEAN_DEBUG

	copy(sourcePlane.constData_, sourcePlane.strideBytes(), sourcePlane.paddingElements_, makeCopyOfPaddingData);

	ocean_assert(strideBytes_ == calculateStrideBytes());
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());

	return true;
}

Frame::Plane& Frame::Plane::operator=(const Plane& plane) noexcept
{
	if (this != &plane)
	{
		const bool result = copy(plane, ACM_USE_OR_COPY, true /*reallocateIfNecessary*/);
		ocean_assert_and_suppress_unused(result, result);
	}

	return *this;
}

Frame::Plane& Frame::Plane::operator=(Plane&& plane) noexcept
{
	if (this != &plane)
	{
		release();

		allocatedData_ = plane.allocatedData_;
		constData_ = plane.constData_;
		data_ = plane.data_;

		width_ = plane.width_;
		height_ = plane.height_;
		channels_ = plane.channels_;
		elementTypeSize_ = plane.elementTypeSize_;
		paddingElements_ = plane.paddingElements_;
		strideBytes_ = plane.strideBytes_;
		bytesPerPixel_ = plane.bytesPerPixel_;

		plane.allocatedData_ = nullptr;
		plane.constData_ = nullptr;
		plane.data_ = nullptr;

		plane.width_ = 0u;
		plane.height_ = 0u;
		plane.channels_ = 0u;
		plane.elementTypeSize_ = 0u;
		plane.paddingElements_ = 0u;
		plane.strideBytes_ = 0u;
		plane.bytesPerPixel_ = 0u;
	}

	return *this;
}

void* Frame::Plane::alignedMemory(const size_t size, const size_t alignment, void*& alignedData)
{
	ocean_assert(alignment >= size_t(1));

	if (size != size_t(0))
	{
		static_assert(sizeof(uint8_t) == 1, "Invalid data type!");

		void* allocatedData = malloc(size + alignment);
		ocean_assert(allocatedData != nullptr);

		if (allocatedData != nullptr)
		{
			const size_t alignmentOffset = (alignment - (size_t(allocatedData) % alignment)) % alignment;

			ocean_assert(alignmentOffset < alignment);
			ocean_assert((size_t(allocatedData) + alignmentOffset) % alignment == size_t(0));

			alignedData = (void*)(((uint8_t*)allocatedData) + alignmentOffset);
			ocean_assert(alignedData >= allocatedData);

			return allocatedData;
		}
	}

	alignedData = nullptr;
	return nullptr;
}

void Frame::Plane::copy(const void* sourceData, const unsigned int sourceStrideBytes, const unsigned int sourcePaddingElements, const bool makeCopyOfPaddingData)
{
	ocean_assert(isValid());

	ocean_assert(sourceData != nullptr);
	ocean_assert(sourceStrideBytes != 0u);

	ocean_assert(sourcePaddingElements * elementTypeSize_ <= sourceStrideBytes);
	ocean_assert(sourceStrideBytes - sourcePaddingElements * elementTypeSize_ == widthBytes());

	if (paddingElements_ == sourcePaddingElements && (paddingElements_ == 0u || makeCopyOfPaddingData))
	{
		ocean_assert(strideBytes() == sourceStrideBytes);

		// we can copy the entire memory block at once

		memcpy(data_, sourceData, strideBytes() * height_);
	}
	else
	{
		// we need to copy row by row, so that we skip the padding elements at the end of each row

		ocean_assert(makeCopyOfPaddingData == false);

		const unsigned int bytesToCopy = widthBytes();

		uint8_t* targetRow = (uint8_t*)(data_);
		const uint8_t* sourceRow = (const uint8_t*)(sourceData);

		for (unsigned int y = 0u; y < height_; ++y)
		{
			memcpy(targetRow, sourceRow, bytesToCopy);

			targetRow += strideBytes();
			sourceRow += sourceStrideBytes;
		}
	}
}

unsigned int Frame::Plane::calculateBytesPerPixel() const
{
	const unsigned int bytes = widthBytes();

	if (width() == 0u)
	{
		return 0u;
	}

	if (bytes % width() == 0u)
	{
		return bytes / width();
	}

	ocean_assert(false && "This should never happen!");

	return 0u;
}

Frame::Frame(const Frame& frame) :
	Frame(frame, ACM_USE_OR_COPY)
{
	// nothing to do here
}

Frame::Frame(const Frame& frame, const AdvancedCopyMode advancedCopyMode) noexcept
{
	if (frame.isValid())
	{
		dynamic_cast<FrameType&>(*this) = frame.frameType();
		timestamp_ = frame.timestamp_;
		relativeTimestamp_ = frame.relativeTimestamp_;

		ocean_assert(frame.planes_.size() >= 1);

		planes_.setCapacity(frame.planes_.size());

		for (size_t n = 0; n < frame.planes_.size(); ++n)
		{
			const Plane& sourcePlane = frame.planes_[n];

			planes_.emplaceBack(sourcePlane, advancedCopyMode);

			if (!planes_.back().isValid())
			{
				ocean_assert(false && "This should never happen!");

				// we may have ran out of memory, so we need to release the entire frame

				release();
				return;
			}
		}
	}
	else
	{
		// one invalid plane
		planes_.assign(1, Plane());
	}

	ocean_assert(planes_.size() >= 1);
}

Frame::Frame(const FrameType& frameType, const PlaneInitializer<void>* planeInitializers, size_t sizePlaneInitializers, const Timestamp& timestamp) :
	FrameType(frameType),
	timestamp_(timestamp)
{
	ocean_assert(frameType.isValid());
	ocean_assert(planeInitializers != nullptr || sizePlaneInitializers == 0);
	ocean_assert(sizePlaneInitializers == frameType.numberPlanes() || sizePlaneInitializers == 0);

	const unsigned int bytesPerElement = frameType.bytesPerDataType();

	planes_.setCapacity(frameType.numberPlanes());

	for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
	{
		unsigned int planeWidth;
		unsigned int planeHeight;
		unsigned int planeChannels;

		if (planeLayout(frameType.pixelFormat(), frameType.width(), frameType.height(), planeIndex, planeWidth, planeHeight, planeChannels))
		{
			ocean_assert(planeWidth != 0u);
			ocean_assert(planeHeight != 0u);
			ocean_assert(planeChannels != 0u);

			if (planeIndex < sizePlaneInitializers)
			{
				const PlaneInitializer<void>& planeInitializer = planeInitializers[planeIndex];

				if (planeInitializer.data_ == nullptr && planeInitializer.constdata_ == nullptr)
				{
					planes_.pushBack(Plane(planeWidth, planeHeight, planeChannels, bytesPerElement, planeInitializer.paddingElements_));
				}
				else
				{
					if (planeInitializer.copyMode_ == CM_USE_KEEP_LAYOUT)
					{
						if (planeInitializer.data_ != nullptr)
						{
							planes_.pushBack(Plane(planeWidth, planeHeight, planeChannels, bytesPerElement, planeInitializer.data_, planeInitializer.paddingElements_));
						}
						else
						{
							ocean_assert(planeInitializer.constdata_ != nullptr);
							planes_.pushBack(Plane(planeWidth, planeHeight, planeChannels, bytesPerElement, planeInitializer.constdata_, planeInitializer.paddingElements_));
						}
					}
					else
					{
						const void* const data = planeInitializer.constdata_ ? planeInitializer.constdata_ : planeInitializer.data_;
						ocean_assert(data != nullptr);

						planes_.pushBack(Plane(planeWidth, planeHeight, planeChannels, bytesPerElement, data, planeInitializer.paddingElements_, planeInitializer.copyMode_));
					}
				}
			}
			else
			{
				constexpr unsigned int paddingElements = 0u;
				planes_.pushBack(Plane(planeWidth, planeHeight, planeChannels, bytesPerElement, paddingElements));
			}
		}
		else
		{
			ocean_assert(false && "Invalid frame type!");

			release();
			return;
		}

		if (!planes_.back().isValid())
		{
			ocean_assert(false && "This should never happen!");

			// we may have ran out of memory, so we need to release the entire frame

			release();
			return;
		}
	}

	ocean_assert(planes_.size() >= 1);
}

Frame::~Frame()
{
	// nothing to do here
}

bool Frame::copy(const Frame& source, const bool copyTimestamp)
{
	ocean_assert(source.isValid());

	if (this == &source)
	{
		return true;
	}

	if (!source.isValid())
	{
		return false;
	}

	FrameType::operator=(source.frameType());

	if (copyTimestamp)
	{
		timestamp_ = source.timestamp_;
		relativeTimestamp_ = source.relativeTimestamp_;
	}

	const Planes& sourcePlanes = source.planes_;

	for (size_t n = 0; n < sourcePlanes.size(); ++n)
	{
		if (n < planes_.size())
		{
			planes_[n].copy(sourcePlanes[n], ACM_COPY_REMOVE_PADDING_LAYOUT, true /*reallocateIfNecessary*/);
		}
		else
		{
			planes_.emplaceBack(sourcePlanes[n], ACM_COPY_REMOVE_PADDING_LAYOUT);
		}
	}

	ocean_assert(sourcePlanes.size() <= planes_.size());

	if (planes_.size() > sourcePlanes.size())
	{
		planes_.resize(sourcePlanes.size());
	}

	for (const Plane& plane : planes_)
	{
		if (!plane.isValid())
		{
			ocean_assert(false && "This should never happen!");

			// we may have ran out of memory, so we need to release the entire frame

			release();
			return false;
		}
	}

	ocean_assert(planes_.size() >= 1);

	return true;
}

bool Frame::copy(const int targetLeft, const int targetTop, const Frame& source, const bool copyTimestamp)
{
	if (!isValid() || !source.isValid() || !isPixelFormatCompatible(source.pixelFormat()) || pixelOrigin() != source.pixelOrigin())
	{
		ocean_assert(false && "Invalid frame combination!");
		return false;
	}

	if ((unsigned int)(abs(targetLeft)) % widthMultiple(pixelFormat()) != 0u || (unsigned int)(abs(targetTop)) % heightMultiple(pixelFormat()) != 0u)
	{
		ocean_assert(false && "Invalid target location, must fit the pixel format's properties!");
		return false;
	}

	if (copyTimestamp)
	{
		timestamp_ = source.timestamp_;
		relativeTimestamp_ = source.relativeTimestamp_;
	}

	// first, we check the trivial case

	if (targetLeft == 0 && targetTop == 0 && isFrameTypeCompatible(source.frameType(), false))
	{
		for (unsigned int planeIndex = 0u; planeIndex < numberPlanes(); ++planeIndex)
		{
			if (paddingElements(planeIndex) == 0u && source.paddingElements(planeIndex) == 0u)
			{
				const uint64_t bytes = uint64_t(strideBytes(planeIndex)) * uint64_t(planeHeight(planeIndex));
				ocean_assert(uint64_t(size_t(bytes)) == bytes);

				memcpy(data<void>(planeIndex), source.constdata<void>(planeIndex), size_t(bytes));
			}
			else
			{
				for (unsigned int y = 0u; y < planeHeight(planeIndex); ++y)
				{
					memcpy(row<void>(y, planeIndex), source.constrow<void>(y, planeIndex), planeWidthBytes(planeIndex));
				}
			}
		}

		return true;
	}

	if (FrameType::formatIsPacked(pixelFormat()))
	{
		ocean_assert(false && "Packed pixel formats are only supported in the trivial case!");
		return false;
	}

	// now, we have a sub-frame to copy

	const unsigned int targetWidth = width();
	const unsigned int targetHeight = height();

	const unsigned int sourceWidth = source.width();
	const unsigned int sourceHeight = source.height();

	const int xTargetStart = std::max(0, targetLeft);
	const int yTargetStart = std::max(0, targetTop);

	const int xTargetEnd = std::min(targetLeft + int(sourceWidth), int(targetWidth)); // exclusive
	const int yTargetEnd = std::min(targetTop + int(sourceHeight), int(targetHeight));

	if (xTargetStart >= xTargetEnd || yTargetStart >= yTargetEnd)
	{
		// the source image is entirely outside of this image, so we can stop here
		// as the top-left target position can be arbitrary, we still return True

		return true;
	}

	const unsigned int xSize = (unsigned int)(xTargetEnd - xTargetStart);
	const unsigned int ySize = (unsigned int)(yTargetEnd - yTargetStart);

	ocean_assert(xSize >= 1u && (unsigned int)(xTargetStart) + xSize <= targetWidth);
	ocean_assert(ySize >= 1u && (unsigned int)(yTargetStart) + ySize <= targetHeight);

	const unsigned int xSourceStart = (unsigned int)(xTargetStart - targetLeft);
	const unsigned int ySourceStart = (unsigned int)(yTargetStart - targetTop);

	ocean_assert(xSourceStart < sourceWidth && xSourceStart + xSize <= sourceWidth);
	ocean_assert(ySourceStart < sourceHeight && ySourceStart + ySize <= sourceHeight);

	for (unsigned int planeIndex = 0u; planeIndex < numberPlanes(); ++planeIndex)
	{
		const unsigned int sourcePlaneWidth = source.planeWidth(planeIndex);
		const unsigned int sourcePlaneHeight = source.planeHeight(planeIndex);

		const unsigned int targetPlaneWidth = planeWidth(planeIndex);
		const unsigned int targetPlaneHeight = planeHeight(planeIndex);

		// we need to consider that some planes may contain a down-samples image of the actual resolution (e.g., Y_UV12)

		const unsigned int xSourceStartPlane = xSourceStart * sourcePlaneWidth / sourceWidth;
		const unsigned int ySourceStartPlane = ySourceStart * sourcePlaneHeight / sourceHeight;
		ocean_assert(xSourceStartPlane * sourceWidth == xSourceStart * sourcePlaneWidth);
		ocean_assert(ySourceStartPlane * sourceHeight == ySourceStart * sourcePlaneHeight);

		const unsigned int xTargetStartPlane = (unsigned int)(xTargetStart) * targetPlaneWidth / targetWidth;
		const unsigned int yTargetStartPlane = (unsigned int)(yTargetStart) * targetPlaneHeight / targetHeight;
		ocean_assert(xTargetStartPlane * targetWidth == (unsigned int)(xTargetStart) * targetPlaneWidth);
		ocean_assert(yTargetStartPlane * targetHeight == (unsigned int)(yTargetStart) * targetPlaneHeight);

		const unsigned int ySizePlane = ySize * sourcePlaneHeight / sourceHeight;
		ocean_assert(ySizePlane == ySize * targetPlaneHeight / targetHeight);

		const unsigned int bytes = xSize * planeWidthBytes(planeIndex) / width();
		ocean_assert(bytes == xSize * source.planeWidthBytes(planeIndex) / sourceWidth);

		unsigned int ySourcePlane = ySourceStartPlane;
		for (unsigned int yTargetPlane = yTargetStartPlane; yTargetPlane < yTargetStartPlane + ySizePlane; ++yTargetPlane)
		{
			memcpy(pixel<void>(xTargetStartPlane, yTargetPlane, planeIndex), source.constpixel<void>(xSourceStartPlane, ySourcePlane, planeIndex), bytes);

			++ySourcePlane;
		}
	}

	return true;
}

bool Frame::set(const FrameType& frameType, const bool forceOwner, const bool forceWritable, const Indices32& planePaddingElements, const Timestamp& timestamp, bool* reallocated)
{
	ocean_assert(planePaddingElements.empty() || planePaddingElements.size() == size_t(frameType.numberPlanes()));

	timestamp_ = timestamp;

	if (!frameType.isValid())
	{
		// the specified frame type is invalid, so we release this frame

		release();
		return true;
	}

	bool needsReallocation = Frame::frameType() != frameType;

	if (forceOwner)
	{
		if (isOwner() == false)
		{
			needsReallocation = true;
		}
	}

	if (forceWritable)
	{
		if (isReadOnly())
		{
			needsReallocation = true;
		}
	}

	if (planes_.size() != frameType.numberPlanes()) // temporary solution to handle Frames with forced single-planes
	{
		needsReallocation = true;
	}

	if (!planePaddingElements.empty())
	{
		for (size_t n = 0; !needsReallocation && n < min(planePaddingElements.size(), planes_.size()); ++n)
		{
			if (planePaddingElements[n] != planes_[n].paddingElements())
			{
				needsReallocation = true;
			}
		}
	}

	if (reallocated)
	{
		*reallocated = needsReallocation;
	}

	if (needsReallocation)
	{
		release();

		ocean_assert(planes_.size() == 1 && !planes_.front().isValid());

		const unsigned int numberPlanes = frameType.numberPlanes();

		planes_.clear();
		planes_.setCapacity(numberPlanes);

		FrameType::operator=(frameType);

		unsigned int planeWidth = 0u;
		unsigned int planeHeight = 0u;
		unsigned int planeChannels = 0u;

		for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
		{
			if (planeLayout(frameType.pixelFormat(), frameType.width(), frameType.height(), planeIndex, planeWidth, planeHeight, planeChannels))
			{
				ocean_assert(planeWidth != 0u);
				ocean_assert(planeHeight != 0u);
				ocean_assert(planeChannels != 0u);

				const unsigned int bytesPerElement = frameType.bytesPerDataType();

				const unsigned int paddingElements = planeIndex < planePaddingElements.size() ? planePaddingElements[planeIndex] : 0u;

				planes_.emplaceBack(planeWidth, planeHeight, planeChannels, bytesPerElement, paddingElements);
			}
			else
			{
				ocean_assert(false && "Invalid frame type!");

				planes_.assign(1, Plane());
				return false;
			}

			if (!planes_.back().isValid())
			{
				// we may have ran out of memory, so we need to release the entire frame

				release();
				return false;
			}
		}
	}

	ocean_assert(planes_.size() >= 1);

	return true;
}

void Frame::makeContinuous()
{
	if (isValid() && !isContinuous())
	{
		*this = Frame(*this, ACM_COPY_REMOVE_PADDING_LAYOUT);
	}
}

void Frame::makeOwner()
{
	if (isValid() && !isOwner())
	{
		*this = Frame(*this, ACM_COPY_REMOVE_PADDING_LAYOUT);

		ocean_assert(isOwner());
	}
}

Frame Frame::subFrame(const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const CopyMode copyMode) const
{
	if (isValid() == false)
	{
		return Frame();
	}

	if (subFrameWidth == 0u || subFrameHeight == 0u || subFrameLeft + subFrameWidth > width() || subFrameTop + subFrameHeight > height() || copyMode == CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA)
	{
		ocean_assert(false && "Invalid parameters!");
		return Frame();
	}

	if (subFrameLeft % widthMultiple(pixelFormat()) != 0u || subFrameTop % heightMultiple(pixelFormat()) != 0u || subFrameWidth % widthMultiple(pixelFormat()) != 0u || subFrameHeight % heightMultiple(pixelFormat()) != 0u)
	{
		ocean_assert(false && "Invalid parameters!");
		return Frame();
	}

	PlaneInitializers<void> planeInitializers;
	planeInitializers.reserve(numberPlanes());

	for (unsigned int planeIndex = 0u; planeIndex < numberPlanes(); ++planeIndex)
	{
		unsigned int planeSubFrameLeft = 0u;
		unsigned int planeSubFrameTop = 0u;

		unsigned int planeSubFrameWidth = 0u;
		unsigned int planeSubFrameHeight = 0u;

		unsigned int planeChannelsDummy = 0u;

		if ((subFrameLeft >= 1u && subFrameLeft < widthMultiple(pixelFormat())) || (subFrameTop >= 1u && subFrameTop < heightMultiple(pixelFormat())))
		{
			ocean_assert(false && "Invalid parameters!");
			return Frame();
		}

		const unsigned int subFrameLeftAdjusted = std::max(widthMultiple(pixelFormat()), subFrameLeft); // planeLayout() must be called with (subFrameLeft, subFrameTop) >= (widthMultple, heightMultiple)
		const unsigned int subFrameTopAdjusted = std::max(heightMultiple(pixelFormat()), subFrameTop);

		if (!planeLayout(pixelFormat(), subFrameLeftAdjusted, subFrameTopAdjusted, planeIndex, planeSubFrameLeft, planeSubFrameTop, planeChannelsDummy))
		{
			ocean_assert(false && "Invalid parameters!");
			return Frame();
		}

		if (subFrameLeft == 0u) // handling special case due to subFrameLeftAdjusted
		{
			planeSubFrameLeft = 0u;
		}

		if (subFrameTop == 0u) // handling special case due to subFrameTopAdjusted
		{
			planeSubFrameTop = 0u;
		}

		if (!planeLayout(pixelFormat(), subFrameWidth, subFrameHeight, planeIndex, planeSubFrameWidth, planeSubFrameHeight, planeChannelsDummy))
		{
			ocean_assert(false && "Invalid parameters!");
			return Frame();
		}

		const size_t subFrameOffsetTopLeftBytes = (const uint8_t*)(constpixel<void>(planeSubFrameLeft, planeSubFrameTop, planeIndex)) - (const uint8_t*)(constdata<void>(planeIndex));
		ocean_assert(subFrameOffsetTopLeftBytes < planes_[planeIndex].size());

		const unsigned int planeStrideElements = strideElements(planeIndex);
		const unsigned int planeSubFrameElements = planeSubFrameWidth * planeChannels(planeIndex);
		ocean_assert(planeSubFrameElements <= planeStrideElements);

		const unsigned int planeSubFramePaddingElements = planeStrideElements - planeSubFrameElements;

		if (isReadOnly())
		{
			const void* subFrameData = (const void*)((const uint8_t*)(constdata<void>(planeIndex)) + subFrameOffsetTopLeftBytes);
			ocean_assert(constpixel<void>(planeSubFrameLeft, planeSubFrameTop, planeIndex) == subFrameData);

			planeInitializers.emplace_back(subFrameData, copyMode, planeSubFramePaddingElements);
		}
		else
		{
			void* subFrameData = (void*)((uint8_t*)(constdata<void>(planeIndex)) + subFrameOffsetTopLeftBytes);
			ocean_assert(constpixel<void>(planeSubFrameLeft, planeSubFrameTop, planeIndex) == (const void*)(subFrameData));

			planeInitializers.emplace_back(subFrameData, copyMode, planeSubFramePaddingElements);
		}
	}

	const FrameType subFrameType(frameType(), subFrameWidth, subFrameHeight);

	return Frame(subFrameType, planeInitializers, timestamp_);
}

bool Frame::setValue(const uint8_t value, const unsigned int planeIndex, const bool skipPaddingData)
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	if (plane.isReadOnly())
	{
		return false;
	}

	if (plane.paddingElements_ == 0u || skipPaddingData == false)
	{
		memset(plane.data<void>(), value, plane.size());
	}
	else
	{
		const unsigned int planeStrideBytes = plane.strideBytes();
		const unsigned int planeWidthBytes = plane.widthBytes();

		for (unsigned int y = 0u; y < plane.height(); ++y)
		{
			memset(plane.data<uint8_t>() + y * planeStrideBytes, value, planeWidthBytes);
		}
	}

	return true;
}

void Frame::release()
{
	FrameType::operator=(FrameType());

	// one invalid plane
	planes_.assign(1, Plane());

	timestamp_.toInvalid();
	relativeTimestamp_.toInvalid();

	ocean_assert(!isValid());
}

bool Frame::haveIntersectingMemory(const Frame& frame) const
{
	ocean_assert(isValid() && frame.isValid());

	if (this == &frame)
	{
		// both frame objects are identical, so they share the same memory
		return true;
	}

	for (unsigned int thisPlaneIndex = 0u; thisPlaneIndex < numberPlanes(); ++thisPlaneIndex)
	{
		for (unsigned int framePlaneIndex = 0u; framePlaneIndex < frame.numberPlanes(); ++framePlaneIndex)
		{
			const size_t thisMemoryStart = size_t(constdata<void>(thisPlaneIndex));
			const size_t thisMemoryEnd = thisMemoryStart + size(thisPlaneIndex); // exclusive

			const size_t frameMemoryStart = size_t(frame.constdata<void>(framePlaneIndex));
			const size_t frameMemoryEnd = frameMemoryStart + frame.size(framePlaneIndex); // exclusive

			// thisMemory          [start                ]end
			// frameMemory                                        [start      ]end

			if (!(frameMemoryEnd <= thisMemoryStart || frameMemoryStart >= thisMemoryEnd))
			{
				return true;
			}
		}
	}

	return false;
}

Frame& Frame::operator=(const Frame& right) noexcept
{
	if (this == &right)
	{
		return *this;
	}

	if (!right.isValid())
	{
		return *this;
	}

	FrameType::operator=(right.frameType());

	const Planes& sourcePlanes = right.planes_;

	for (size_t n = 0; n < sourcePlanes.size(); ++n)
	{
		if (n < planes_.size())
		{
			planes_[n].copy(sourcePlanes[n], ACM_USE_OR_COPY, true /*reallocateIfNecessary*/);
		}
		else
		{
			planes_.emplaceBack(sourcePlanes[n], ACM_USE_OR_COPY);
		}
	}

	ocean_assert(sourcePlanes.size() <= planes_.size());

	if (planes_.size() > sourcePlanes.size())
	{
		planes_.resize(sourcePlanes.size());
	}

	for (const Plane& plane : planes_)
	{
		if (!plane.isValid())
		{
			ocean_assert(false && "This should never happen!");

			// we may have ran out of memory, so we need to release the entire frame

			release();
			return *this;
		}
	}

	ocean_assert(planes_.size() >= 1);

	timestamp_ = right.timestamp_;
	relativeTimestamp_ = right.relativeTimestamp_;

	return *this;
}

Frame& Frame::operator=(Frame&& right) noexcept
{
	if (this != &right)
	{
		release();

		FrameType::operator=(right.frameType());

		planes_ = std::move(right.planes_);

		((FrameType&)right) = FrameType();
		right.planes_.assign(1, Plane());

		timestamp_ = right.timestamp_;
		relativeTimestamp_ = right.relativeTimestamp_;
		right.timestamp_.toInvalid();
		right.relativeTimestamp_.toInvalid();
	}

	ocean_assert(planes_.size() >= 1);

	return *this;
}

bool Frame::strideBytes2paddingElements(const PixelFormat& pixelFormat, const unsigned int imageWidth, const unsigned int planeStrideBytes, unsigned int& planePaddingElements, const unsigned int planeIndex)
{
	const FrameType::DataType pixelFormatDataType = dataType(pixelFormat);

	if (pixelFormatDataType == FrameType::DT_UNDEFINED)
	{
		// we have a non-generic pixel format
		return false;
	}

	const unsigned int bytesPerElement = bytesPerDataType(pixelFormatDataType);
	ocean_assert(bytesPerElement != 0u);

	ocean_assert(planeStrideBytes % bytesPerElement == 0u);
	if (planeStrideBytes % bytesPerElement != 0u)
	{
		return false;
	}

	const unsigned int planeStrideElements = planeStrideBytes / bytesPerElement;

	unsigned int planeWidth = 0u;
	unsigned int planeHeight = 0u;
	unsigned int planeChannels = 0u;

	if (!planeLayout(pixelFormat, imageWidth, heightMultiple(pixelFormat), planeIndex, planeWidth, planeHeight, planeChannels))
	{
		return false;
	}

	const unsigned int planeRowElements = planeWidth * planeChannels;

	ocean_assert(planeStrideElements >= planeRowElements);
	if (planeStrideElements < planeRowElements)
	{
		return false;
	}

	planePaddingElements = planeStrideElements - planeRowElements;

	ocean_assert(planeStrideBytes == (planeWidth * planeChannels + planePaddingElements) * bytesPerElement);

	return true;
}

}
