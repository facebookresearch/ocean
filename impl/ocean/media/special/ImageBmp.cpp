/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/special/ImageBmp.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

/**
 * MSDN conform bitmap file header, equivalent to BITMAPFILEHEADER.
 */
struct ImageBmp::FileHeader
{
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
};

/**
 * MSDN conform bitmap data header, equivalent to BITMAPINFOHEADER.
 */
struct ImageBmp::DataHeader
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};

Frame ImageBmp::decodeImage(const void* buffer, const size_t size)
{
#ifdef _WINDOWS
	static_assert(sizeof(FileHeader) >= sizeof(BITMAPFILEHEADER), "Invalid data type!");
	static_assert(sizeof(DataHeader) == sizeof(BITMAPINFOHEADER), "Invalid data type!");
#endif

	// currently we limited support for 24bit Windows bitmaps (not OS/2 bitmaps) without compression only

	ocean_assert(buffer != nullptr && size > 0);

	// first we need to ensure that we have a little endian system

	const int32_t littleEndianValue = 1;
	if (*(int8_t*)(&littleEndianValue) != int8_t(1))
	{
		ocean_assert(false && "This must never happen as we expect always a little endian system!");
		return Frame();
	}

	static_assert(sizeof(unsigned int) == 4, "Invalid data type");
	static_assert(sizeof(unsigned short) == 2, "Invalid data type");
	static_assert(sizeof(int) == 4, "Invalid data type");

	if (size <= 14)
	{
		return Frame();
	}

	// as we do not want to force sizeof(FileHeader) == 14, we have to load every parameter individually

	const uint8_t* byteBuffer = (const uint8_t*)(buffer);

	FileHeader bitmapFileHeader;
	memcpy(&bitmapFileHeader.bfType, byteBuffer + 0, sizeof(bitmapFileHeader.bfType));
	memcpy(&bitmapFileHeader.bfSize, byteBuffer + 2, sizeof(bitmapFileHeader.bfSize));
	memcpy(&bitmapFileHeader.bfReserved1, byteBuffer + 6, sizeof(bitmapFileHeader.bfReserved1));
	memcpy(&bitmapFileHeader.bfReserved2, byteBuffer + 8, sizeof(bitmapFileHeader.bfReserved2));
	memcpy(&bitmapFileHeader.bfOffBits, byteBuffer + 10, sizeof(bitmapFileHeader.bfOffBits));

	if (!NumericT<size_t>::isInsideValueRange(bitmapFileHeader.bfSize))
	{
		ocean_assert(false && "Memory too large for local platform");
		return Frame();
	}

	if (bitmapFileHeader.bfType != uint16_t(19778) || bitmapFileHeader.bfSize > size) // 19778 = 'MB'
	{
		return Frame();
	}

	static_assert(sizeof(DataHeader) == 40, "Invalid data type!");

	if (size <= 14 + 40)
	{
		return Frame();
	}

	DataHeader bitmapDataHeader;
	memcpy(&bitmapDataHeader, byteBuffer + 14, sizeof(bitmapDataHeader));

	if (bitmapDataHeader.biSize != 40u || bitmapDataHeader.biCompression != 0u || bitmapDataHeader.biWidth <= 0 || bitmapDataHeader.biHeight == 0)
	{
		return Frame();
	}

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

	if (bitmapDataHeader.biBitCount == 8u)
	{
		pixelFormat = FrameType::FORMAT_Y8;
	}
	else if (bitmapDataHeader.biBitCount == 24u)
	{
		pixelFormat = FrameType::FORMAT_BGR24;
	}

	if (pixelFormat == FrameType::FORMAT_UNDEFINED || bitmapDataHeader.biPlanes != 1u)
	{
		return Frame();
	}

	// we cannot check bitmapDataHeader.biSizeImage as it can be 0 for uncompressed frames

	const uint64_t bytesPerPixel = uint64_t(bitmapDataHeader.biBitCount) / 8ull;
	ocean_assert(bytesPerPixel == 1ull || bytesPerPixel == 3ull);

	if (!NumericT<unsigned int>::isInsideValueRange(bitmapDataHeader.biWidth) || !NumericT<unsigned int>::isInsideValueRange(abs(bitmapDataHeader.biHeight)))
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	const FrameType::PixelOrigin pixelOrigin = bitmapDataHeader.biHeight > 0 ? FrameType::ORIGIN_LOWER_LEFT : FrameType::ORIGIN_UPPER_LEFT;
	const FrameType frameType((unsigned int)(bitmapDataHeader.biWidth), (unsigned int)(abs(bitmapDataHeader.biHeight)), pixelFormat, pixelOrigin);

	// **TODO** hot fix to ensure that images cannot be larger than 2^32 bytes

	ocean_assert(frameType.channels() <= 3u);
	if (uint64_t(frameType.width()) * uint64_t(frameType.height()) >= uint64_t(1431655764ull)) // width * height * 3 < 2^32
	{
		return Frame();
	}

	const uint64_t bitmapEmptyRowEndingBytes = (4ull - ((uint64_t(frameType.width()) * bytesPerPixel) % 4ull)) % 4ull;
	const uint64_t bitmapRowStrideBytes = uint64_t(frameType.width()) * bytesPerPixel + bitmapEmptyRowEndingBytes;

	// we ensure that the given buffer is large enough
	if ((uint64_t(frameType.height()) * bitmapRowStrideBytes) + uint64_t(bitmapFileHeader.bfOffBits) > uint64_t(size))
	{
		return Frame();
	}

	if (bitmapRowStrideBytes >= uint64_t(NumericT<unsigned int>::maxValue()))
	{
		return Frame();
	}

	unsigned int paddingElements = 0u;
	if (!Frame::strideBytes2paddingElements(pixelFormat, frameType.width(), (unsigned int)(bitmapRowStrideBytes), paddingElements))
	{
		return Frame();
	}

	const uint8_t* bitmapData = byteBuffer + bitmapFileHeader.bfOffBits;

	return Frame(frameType, bitmapData, Frame::CM_COPY_REMOVE_PADDING_LAYOUT, paddingElements);
}

bool ImageBmp::encodeImage(const Frame& frame, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted)
{
	ocean_assert(frame);

	if (hasBeenConverted)
	{
		*hasBeenConverted = false;
	}

	Frame convertedFrame;
	const Frame* outputFrame = &frame;

	if (!isPixelFormatSupported(frame.pixelFormat()))
	{
		if (!allowConversion)
		{
			return false;
		}

		if (FrameType::formatHasAlphaChannel(frame.pixelFormat()))
		{
			return false;
		}

		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_BGR24, convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
		{
			return false;
		}

		if (hasBeenConverted)
		{
			*hasBeenConverted = true;
		}

		outputFrame = &convertedFrame;
	}

	ocean_assert(outputFrame != nullptr && outputFrame->isValid() && outputFrame->pixelFormat() == FrameType::FORMAT_BGR24);

	const int32_t littleEndianValue = 1;
	if (*(int8_t*)&(littleEndianValue) != int8_t(1))
	{
		ocean_assert(false && "This must never happen as we expect always a little endian system!");
		return false;
	}

	static_assert(sizeof(unsigned int) == 4, "Invalid data type");
	static_assert(sizeof(unsigned short) == 2, "Invalid data type");
	static_assert(sizeof(int) == 4, "Invalid data type");

	ocean_assert(outputFrame->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && FrameType::formatIsGeneric(outputFrame->pixelFormat()));

	const unsigned int bytesPerPixel = outputFrame->channels();
	const unsigned int bitmapEmptyRowEndingBytes = (4u - ((outputFrame->width() * bytesPerPixel) % 4u)) % 4u;
	const unsigned int bitmapRowStrideBytes = outputFrame->width() * bytesPerPixel + bitmapEmptyRowEndingBytes;

	const unsigned int bfOffBits = 14u + 40u;

	const size_t bufferSize = size_t(bfOffBits) + size_t(bitmapRowStrideBytes * outputFrame->height());

	if (!NumericT<uint32_t>::isInsideValueRange(bufferSize))
	{
		return false;
	}

	buffer.resize(bufferSize);
	uint8_t* bufferData = buffer.data();

	const FileHeader fileHeader =
	{
		uint16_t(19778), // bfType // 19778 = 'MB'
		uint32_t(bufferSize), // bfSize
		0u, // bfReserved1
		0u, // bfReserved2
		bfOffBits // bfOffBits
	};

	memcpy(bufferData + 0, &fileHeader.bfType, sizeof(fileHeader.bfType));
	memcpy(bufferData + 2, &fileHeader.bfSize, sizeof(fileHeader.bfSize));
	memcpy(bufferData + 6, &fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
	memcpy(bufferData + 8, &fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
	memcpy(bufferData + 10, &fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));

	static_assert(sizeof(DataHeader) == 40, "Invalid data type!");

	const DataHeader bitmapDataHeader =
	{
		40u, // biSize;
		int32_t(outputFrame->width()), // biWidth;
		outputFrame->pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT ? int(outputFrame->height()) : -int(outputFrame->height()), // biHeight;
		uint16_t(1u), // biPlanes;
		uint16_t(bytesPerPixel * 8u), // biBitCount;
		uint32_t(0u), // biCompression;
		bitmapRowStrideBytes * outputFrame->height(), // biSizeImage;
		0, // biXPelsPerMeter;
		0, // biYPelsPerMeter;
		0u, // biClrUsed;
		0u // biClrImportant;
	};

	memcpy(bufferData + 14, &bitmapDataHeader, sizeof(bitmapDataHeader));

	uint8_t* bitmapData = bufferData + bfOffBits;

	for (unsigned int y = 0u; y < outputFrame->height(); ++y)
	{
		memcpy(bitmapData, outputFrame->constrow<void>(y), outputFrame->width() * bytesPerPixel);

		bitmapData += bitmapRowStrideBytes;
	}

	return true;
}

}

}

}
