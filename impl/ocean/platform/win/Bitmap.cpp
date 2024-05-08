/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Bitmap.h"

#include "ocean/cv/FrameConverterRGB24.h"
#include "ocean/cv/FrameConverterYUYV16.h"
#include "ocean/cv/FrameConverterY_U_V12.h"
#include "ocean/cv/FrameConverterY_V_U12.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

Bitmap::Bitmap(const Bitmap& bitmap) :
	Bitmap()
{
	*this = bitmap;
}

Bitmap::Bitmap(Bitmap&& bitmap) :
	Bitmap()
{
	*this = std::move(bitmap);
}

Bitmap::Bitmap(const Frame& frame) :
	Bitmap()
{
	set(frame);
}

Bitmap::Bitmap(const FrameType& frameType) :
	Bitmap(frameType.width(), frameType.height(), frameType.pixelFormat(), frameType.pixelOrigin())
{
	// nothing to do here
}

Bitmap::Bitmap(const unsigned int width, const unsigned int height, const FrameType::PixelFormat preferredPixelFormat, const FrameType::PixelOrigin pixelOrigin) :
	Bitmap()
{
	ocean_assert(width != 0u && height != 0u);

	const FrameType::PixelFormat supportedPixelFormat = internalPixelFormat(preferredPixelFormat);

	if (supportedPixelFormat != FrameType::FORMAT_UNDEFINED)
	{
		createBitmap(width, height, supportedPixelFormat, pixelOrigin);
	}
	else
	{
		ocean_assert(false && "This pixel format of the frame is not supported!");
	}
}

Bitmap::Bitmap(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const uint8_t* frame, unsigned int framePaddingElements) :
	Bitmap(width, height, pixelFormat, pixelOrigin)
{
	if (isValid())
	{
		ocean_assert(bitmapPixelOrigin_ == pixelOrigin);

		setData(frame, pixelFormat, framePaddingElements);
	}
}

Bitmap::~Bitmap()
{
	release();
}

bool Bitmap::setData(const uint8_t* frame, const FrameType::PixelFormat pixelFormat, const unsigned int framePaddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);

	if (frame == nullptr)
	{
		return false;
	}

	return set(Frame(FrameType(bitmapWidth_, bitmapHeight_, pixelFormat, bitmapPixelOrigin_), frame, Frame::CM_USE_KEEP_LAYOUT, framePaddingElements));
}

bool Bitmap::set(const unsigned int width, const unsigned int height, const FrameType::PixelFormat preferredPixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(preferredPixelFormat != FrameType::FORMAT_UNDEFINED);
	ocean_assert(pixelOrigin != FrameType::ORIGIN_INVALID);

	const FrameType::PixelFormat supportedPixelFormat = internalPixelFormat(preferredPixelFormat);

	if (width == bitmapWidth_ && height == bitmapHeight_ && supportedPixelFormat == bitmapPixelFormat_ && pixelOrigin == bitmapPixelOrigin_)
	{
		return true;
	}

	release();

	return createBitmap(width, height, supportedPixelFormat, pixelOrigin);
}

bool Bitmap::set(const Frame& frame)
{
	if (!frame.isValid())
	{
		release();
		return true;
	}

	if (!set(frame.width(), frame.height(), frame.pixelFormat(), frame.pixelOrigin()))
	{
		return false;
	}

	ocean_assert(bitmapData_ != nullptr);

	Frame bitmapFrame(FrameType(bitmapWidth_, bitmapHeight_, bitmapPixelFormat_, bitmapPixelOrigin_), bitmapData_, Frame::CM_USE_KEEP_LAYOUT, bitmapPaddingElements_);
	ocean_assert(bitmapFrame);

	return CV::FrameConverter::Comfort::convertAndCopy(frame, bitmapFrame);
}

void Bitmap::release()
{
	bitmapWidth_ = 0u;
	bitmapHeight_ = 0u;

	bitmapPixelFormat_ = FrameType::FORMAT_UNDEFINED;
	bitmapPixelOrigin_ = FrameType::ORIGIN_INVALID;

	bitmapData_ = nullptr;

	bitmapSize_ = 0u;

	bitmapPaddingElements_ = 0u;
	bitmapStrideElements_ = 0u;

	if (bitmapDC_)
	{
		const BOOL result = DeleteDC(bitmapDC_);
		ocean_assert_and_suppress_unused(result == TRUE, result);

		bitmapDC_ = nullptr;
	}

	if (bitmapHandle_)
	{
		const BOOL result = DeleteObject(bitmapHandle_);
		ocean_assert_and_suppress_unused(result == TRUE, result);

		bitmapHandle_ = nullptr;
	}
}

bool Bitmap::isValid() const
{
	return bitmapWidth_ != 0u && bitmapHeight_ != 0u && bitmapPixelFormat_ != FrameType::FORMAT_UNDEFINED && bitmapPixelOrigin_ != FrameType::ORIGIN_INVALID;
}

Bitmap& Bitmap::operator=(Bitmap&& bitmap)
{
	if (this != &bitmap)
	{
		release();

		bitmapWidth_ = bitmap.bitmapWidth_;
		bitmapHeight_ = bitmap.bitmapHeight_;
		bitmapPixelFormat_ = bitmap.bitmapPixelFormat_;
		bitmapPixelOrigin_ = bitmap.bitmapPixelOrigin_;
		bitmapData_ = bitmap.bitmapData_;
		bitmapSize_ = bitmap.bitmapSize_;
		bitmapPaddingElements_ = bitmap.bitmapPaddingElements_;
		bitmapStrideElements_ = bitmap.bitmapStrideElements_;
		bitmapDC_ = bitmap.bitmapDC_;
		bitmapHandle_ = bitmap.bitmapHandle_;

		bitmap.bitmapWidth_ = 0u;
		bitmap.bitmapHeight_ = 0u;
		bitmap.bitmapPixelFormat_ = FrameType::FORMAT_UNDEFINED;
		bitmap.bitmapPixelOrigin_ = FrameType::ORIGIN_INVALID;
		bitmap.bitmapData_ = nullptr;
		bitmap.bitmapSize_ = 0u;
		bitmap.bitmapPaddingElements_ = 0u;
		bitmap.bitmapStrideElements_ = 0u;
		bitmap.bitmapDC_ = nullptr;
		bitmap.bitmapHandle_ = nullptr;
	}

	return *this;
}

Bitmap& Bitmap::operator=(const Bitmap &bitmap)
{
	if (bitmap.isValid())
	{
		if (bitmapWidth_ != bitmap.bitmapWidth_ || bitmapHeight_ != bitmap.bitmapHeight_ || bitmapPixelFormat_ != bitmap.bitmapPixelFormat_ || bitmapPixelOrigin_ != bitmap.bitmapPixelOrigin_)
		{
			if (!set(bitmap.bitmapWidth_, bitmap.bitmapHeight_, bitmap.bitmapPixelFormat_, bitmap.bitmapPixelOrigin_))
			{
				ocean_assert(false && "This must never happen!");
			}
		}

		ocean_assert(bitmap.bitmapData_ != nullptr);
		ocean_assert(bitmapData_ != nullptr);

		memcpy(bitmapData_, bitmap.bitmapData_, size());
	}
	else
	{
		release();
	}

	return *this;
}

FrameType::PixelFormat Bitmap::internalPixelFormat(const FrameType::PixelFormat pixelFormat)
{
	ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);

	switch ((std::underlying_type<FrameType::PixelFormat>::type)(pixelFormat))
	{
		case FrameType::FORMAT_Y8:
		case FrameType::genericPixelFormat<uint8_t, 1u>():
			return FrameType::FORMAT_Y8;

		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_Y_U_V12:
		case FrameType::FORMAT_Y_UV12:
		case FrameType::FORMAT_Y_V_U12:
		case FrameType::FORMAT_Y_VU12:
		case FrameType::FORMAT_YUYV16:
		case FrameType::FORMAT_BGGR10_PACKED:
		case FrameType::FORMAT_RGGB10_PACKED:
		case FrameType::genericPixelFormat<uint8_t, 3u>():
			return FrameType::FORMAT_BGR24;

		case FrameType::FORMAT_BGR24:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_BGR4444:
		case FrameType::FORMAT_BGR5551:
		case FrameType::FORMAT_BGR565:
		case FrameType::FORMAT_BGRA4444:
		case FrameType::FORMAT_RGB4444:
		case FrameType::FORMAT_RGB5551:
		case FrameType::FORMAT_RGB565:
		case FrameType::FORMAT_RGBA4444:
		case FrameType::genericPixelFormat<uint8_t, 4u>():
			return pixelFormat;

		default:
			break;
	}

	return FrameType::FORMAT_UNDEFINED;
}

bool Bitmap::createBitmap(const unsigned int width, const unsigned int height, const FrameType::PixelFormat supportedPixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	ocean_assert(bitmapWidth_ == 0u && bitmapHeight_ == 0u);
	ocean_assert(bitmapPixelFormat_ == FrameType::FORMAT_UNDEFINED && bitmapPixelOrigin_ == FrameType::ORIGIN_INVALID);

	ocean_assert(bitmapData_ == nullptr);

	ocean_assert(bitmapSize_ == 0u);
	ocean_assert(bitmapPaddingElements_ == 0u);
	ocean_assert(bitmapStrideElements_ == 0u);

	ocean_assert(bitmapDC_ == nullptr);
	ocean_assert(bitmapHandle_ == nullptr);

	ocean_assert(supportedPixelFormat != FrameType::FORMAT_UNDEFINED);

	Memory bitmapInfoMemory;

	if (!createBitmapInfo(width, height, supportedPixelFormat, pixelOrigin, bitmapInfoMemory))
	{
		return false;
	}

	// create a device context for the bitmap
	bitmapDC_ = CreateCompatibleDC(nullptr);

	if (bitmapDC_ == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	bitmapHandle_ = CreateDIBSection(bitmapDC_, bitmapInfoMemory.data<BITMAPINFO>(false /*checkAlignment*/), DIB_RGB_COLORS, (void**)&bitmapData_, nullptr, 0);

	if (bitmapHandle_ == nullptr)
	{
		ocean_assert(false && "This should never happen!");

		release();
		return false;
	}

	SelectObject(bitmapDC_, bitmapHandle_);

	bitmapWidth_ = width;
	bitmapHeight_ = height;
	bitmapPixelFormat_ = supportedPixelFormat;
	bitmapPixelOrigin_ = pixelOrigin;

	ocean_assert(FrameType::dataType(supportedPixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(FrameType::numberPlanes(supportedPixelFormat) == 1u);

	unsigned int planeWidth;
	unsigned int planeHeight;
	unsigned int planeChannels;
	if (!FrameType::planeLayout(supportedPixelFormat, width, height, 0u, planeWidth, planeHeight, planeChannels))
	{
		ocean_assert(false && "This should never happen!");

		release();
		return false;
	}

	const unsigned int bitmapWidthElements = planeWidth * planeChannels; // each element is also a byte

	bitmapPaddingElements_ = (4u - (bitmapWidthElements % 4u)) % 4u; // Windows bitmaps are 4 byte aligned

	bitmapStrideElements_ = bitmapWidthElements + bitmapPaddingElements_;
	ocean_assert(bitmapStrideElements_ % 4u == 0u);

	bitmapSize_ = bitmapHeight_ * bitmapStrideElements_;

	return true;
}

bool Bitmap::createBitmapInfo(const unsigned int width, const unsigned int height, const FrameType::PixelFormat bitmapPixelFormat, const FrameType::PixelOrigin pixelOrigin, Memory& bitmapInfoMemory)
{
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(bitmapPixelFormat != FrameType::FORMAT_UNDEFINED);
	ocean_assert(internalPixelFormat(bitmapPixelFormat) == bitmapPixelFormat);

	// first we need to identify the size of bitmapInfo

	switch (bitmapPixelFormat)
	{
		case FrameType::FORMAT_Y8:
			bitmapInfoMemory = Memory(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);
			break;

		case FrameType::FORMAT_BGR24:
			bitmapInfoMemory = Memory(sizeof(BITMAPINFO)); // = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 1
			break;

		case FrameType::FORMAT_BGR4444:
		case FrameType::FORMAT_BGR5551:
		case FrameType::FORMAT_BGR565:
		case FrameType::FORMAT_BGRA4444:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_RGB4444:
		case FrameType::FORMAT_RGB5551:
		case FrameType::FORMAT_RGB565:
		case FrameType::FORMAT_RGBA4444:
		case FrameType::FORMAT_RGBA32:
			bitmapInfoMemory = Memory(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 3);
			break;

		default:
			ocean_assert(false && "This must never happen!");
			return false;
	}

	ocean_assert(FrameType::numberPlanes(bitmapPixelFormat) == 1u);

	unsigned int planeWidth;
	unsigned int planeHeight;
	unsigned int planeChannels;
	if (!FrameType::planeLayout(bitmapPixelFormat, width, height, 0u, planeWidth, planeHeight, planeChannels))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const unsigned int bitsPerPixel = planeChannels * FrameType::bytesPerDataType(FrameType::dataType(bitmapPixelFormat)) * 8u;

	ocean_assert(bitmapInfoMemory);

	// we fill the bitmap header

	BITMAPINFOHEADER& bitmapHeader = *bitmapInfoMemory.data<BITMAPINFOHEADER>(false /*check alignment*/);

	bitmapHeader.biSize = sizeof(bitmapHeader);
	bitmapHeader.biWidth = LONG(width);
	bitmapHeader.biHeight = (pixelOrigin == FrameType::ORIGIN_LOWER_LEFT) ? LONG(height) : -LONG(height);
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biBitCount = WORD(bitsPerPixel);
	bitmapHeader.biSizeImage = 0;
	bitmapHeader.biXPelsPerMeter = 0;
	bitmapHeader.biYPelsPerMeter = 0;

	RGBQUAD* colors = (RGBQUAD*)(bitmapInfoMemory.data<uint8_t>() + sizeof(BITMAPINFOHEADER));
	uint32_t* colorMasks = (uint32_t*)colors;

	switch (bitmapPixelFormat)
	{
		case FrameType::FORMAT_Y8:
			bitmapHeader.biClrUsed = 256;
			bitmapHeader.biClrImportant = 256;
			bitmapHeader.biCompression = BI_RGB;

			for (unsigned int n = 0; n < 256; n++)
			{
				colors[n].rgbRed = BYTE(n);
				colors[n].rgbGreen = BYTE(n);
				colors[n].rgbBlue = BYTE(n);
				colors[n].rgbReserved = 0;
			}
			break;

		case FrameType::FORMAT_BGR4444:
		case FrameType::FORMAT_BGRA4444:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x00000F00;
			// green mask
			colorMasks[1] = 0x000000F0;
			// blue mask
			colorMasks[2] = 0x0000000F;
			break;

		case FrameType::FORMAT_BGR5551:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x00007C00;
			// green mask
			colorMasks[1] = 0x000003E0;
			// blue mask
			colorMasks[2] = 0x0000001F;
			break;

		case FrameType::FORMAT_BGR565:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x0000F800;
			// green mask
			colorMasks[1] = 0x000007E0;
			// blue mask
			colorMasks[2] = 0x0000001F;
			break;

		case FrameType::FORMAT_BGRA32:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x00FF0000;
			// green mask
			colorMasks[1] = 0x0000FF00;
			// blue mask
			colorMasks[2] = 0x000000FF;
			break;

		case FrameType::FORMAT_RGB4444:
		case FrameType::FORMAT_RGBA4444:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x0000000F;
			// green mask
			colorMasks[1] = 0x000000F0;
			// blue mask
			colorMasks[2] = 0x00000F00;
			break;

		case FrameType::FORMAT_RGB5551:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x0000001F;
			// green mask
			colorMasks[1] = 0x000003E0;
			// blue mask
			colorMasks[2] = 0x00007C00;
			break;

		case FrameType::FORMAT_RGB565:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x0000001F;
			// green mask
			colorMasks[1] = 0x000007E0;
			// blue mask
			colorMasks[2] = 0x0000F800;
			break;

		case FrameType::FORMAT_RGBA32:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_BITFIELDS;
			// red mask
			colorMasks[0] = 0x000000FF;
			// green mask
			colorMasks[1] = 0x0000FF00;
			// blue mask
			colorMasks[2] = 0x00FF0000;
			break;

		case FrameType::FORMAT_BGR24:
			bitmapHeader.biClrUsed = 0;
			bitmapHeader.biClrImportant = 0;
			bitmapHeader.biCompression = BI_RGB;
			colors = nullptr;
			break;

		default:
			ocean_assert(false && "This must never happen!");
			return false;
	}

	return true;
}

}

}

}
