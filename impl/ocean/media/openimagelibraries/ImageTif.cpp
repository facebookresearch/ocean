/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/ImageTif.h"

#include "ocean/base/ScopedObject.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF

#include <tiffio.h>

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

static tmsize_t imageTifReadData_Load(thandle_t userData, void* buffer, tmsize_t size)
{
	ocean_assert(userData);
	ImageTif::InputDataTriple* inputDataTriple = reinterpret_cast<ImageTif::InputDataTriple*>(userData);

	// inputDataTriple->first() holds the start pointer to the memory buffer
	// inputDataTriple->second() holds the number of bytes the buffer has
	// inputDataTriple->third() holds the current location within the buffer

	ocean_assert(inputDataTriple->third() <= inputDataTriple->second());
	if (inputDataTriple->third() + size <= inputDataTriple->second())
	{
		memcpy(buffer, inputDataTriple->first() + inputDataTriple->third(), size_t(size));
		inputDataTriple->third() += size;

		ocean_assert(inputDataTriple->third() <= inputDataTriple->second());
		return size;
	}

	return 0;
}

static tmsize_t imageTifWriteData_Write(thandle_t userData, void* buffer, tmsize_t size)
{
	ocean_assert(userData && size >= 0);
	ImageTif::OutputDataTriple* outputDataTriple = reinterpret_cast<ImageTif::OutputDataTriple*>(userData);

	// outputDataTriple->first() holds the memory buffer including some unsued bytes
	// outputDataTriple->second() holds the number of used bytes in the output buffer
	// outputDataTriple->third() holds the current location within the output buffer

	if (outputDataTriple->third() + size > outputDataTriple->first().size())
	{
		outputDataTriple->first().resize(max(size_t(outputDataTriple->third() + size), size_t(outputDataTriple->first().size() + 1024 * 64)));
	}

	memcpy(outputDataTriple->first().data() + outputDataTriple->third(), buffer, size_t(size));
	outputDataTriple->third() += size;
	outputDataTriple->second() = max(outputDataTriple->second(), outputDataTriple->third());

	return size;
}

static tmsize_t imageTifDataInvalid(thandle_t /*userData*/, void* /*buffer*/, tmsize_t /*size*/)
{
	ocean_assert(false && "This must never happen!");
	return 0;
}

static toff_t imageTifSeekData_Load(thandle_t userData, toff_t position, int whence)
{
	ocean_assert(userData);
	ImageTif::InputDataTriple* inputDataTriple = reinterpret_cast<ImageTif::InputDataTriple*>(userData);

	ocean_assert(position != 0xFFFFFFFF);

	// inputDataTriple->first() holds the start pointer to the memory buffer
	// inputDataTriple->second() holds the number of bytes the buffer has
	// inputDataTriple->third() holds the current location within the buffer

	switch (whence)
	{
		case 0:
			// absolute location from beginning
			if (position <= inputDataTriple->second())
			{
				inputDataTriple->third() = size_t(position);
			}
			else
			{
				return toff_t(-1);
			}
			break;

		case 1:
			// relative location
			if (inputDataTriple->third() + size_t(position) <= inputDataTriple->second())
			{
				inputDataTriple->third() += size_t(position);
			}
			else
			{
				return toff_t(-1);
			}
			break;

		case 2:
			// absolute location from ending
			ocean_assert(position == toff_t(0));
			if (position <= inputDataTriple->second())
			{
				inputDataTriple->third() = size_t((long long)inputDataTriple->second() - (long long)position);
			}
			else
			{
				return toff_t(-1);
			}
			break;

		default:
			return toff_t(-1);
	}

	ocean_assert(inputDataTriple->third() <= inputDataTriple->second());
	return inputDataTriple->third();
}

static toff_t imageTifSeekData_Write(thandle_t userData, toff_t position, int whence)
{
	ocean_assert(userData);
	ImageTif::OutputDataTriple* outputDataTriple = reinterpret_cast<ImageTif::OutputDataTriple*>(userData);

	ocean_assert(position != 0xFFFFFFFF);

	// outputDataTriple->first() holds the memory buffer including some unsued bytes
	// outputDataTriple->second() holds the number of used bytes in the output buffer
	// outputDataTriple->third() holds the current location within the output buffer

	switch (whence)
	{
		case 0:
			// absolute location from beginning
			if (position <= outputDataTriple->first().size())
			{
				outputDataTriple->third() = size_t(position);
			}
			else
			{
				return toff_t(-1);
			}
			break;

		case 1:
			// relative location
			if (outputDataTriple->third() + size_t(position) <= outputDataTriple->first().size())
			{
				outputDataTriple->third() += size_t(position);
			}
			else
			{
				return toff_t(-1);
			}
			break;

		case 2:
			// absolute location from ending
			ocean_assert(position == toff_t(0));
			if (position <= outputDataTriple->second())
			{
				outputDataTriple->third() = size_t((long long)outputDataTriple->second() - (long long)position);
			}
			else
			{
				return toff_t(-1);
			}
			break;

		default:
			return toff_t(-1);
	}

	ocean_assert(outputDataTriple->second() <= outputDataTriple->first().size());
	ocean_assert(outputDataTriple->third() <= outputDataTriple->first().size());
	return outputDataTriple->third();
}

static toff_t imageTifSizeData_Load(thandle_t userData)
{
	ocean_assert(userData);
	ImageTif::InputDataTriple* inputDataTriple = reinterpret_cast<ImageTif::InputDataTriple*>(userData);

	return inputDataTriple->second();
}

static toff_t imageTifSizeData_Write(thandle_t userData)
{
	ocean_assert(userData);
	ImageTif::OutputDataTriple* outputDataTriple = reinterpret_cast<ImageTif::OutputDataTriple*>(userData);

	return outputDataTriple->third();
}

static int imageTifCloseData(thandle_t)
{
	return 0;
}

static int imageTifMapData(thandle_t, void** /*base*/, toff_t* /*size*/)
{
	return 0;
}

static void imageTifUnmapData(thandle_t, void* /*base*/, toff_t /*size*/)
{
	// nothing to do here
}

Frame ImageTif::decodeImage(const void* buffer, const size_t size)
{
	// currently we limited support for 24bit Windows bitmaps (not OS/2 bitmaps) without compression only

	ocean_assert(buffer != nullptr && size > 0);

	const uint8_t* byteBuffer = (const uint8_t*)(buffer);

	if (size <= 4)
	{
		return Frame();
	}

	// Magic number little endian: 0x49 0x49 0x2a 0x00
	// Magic number big endian:    0x4d 0x4d 0x00 0x2a (e.g., Mac encoding)

	const bool hasMagicNumberLittleEndian = byteBuffer[0] == 0x49u && byteBuffer[1] == 0x49u && byteBuffer[2] == 0x2Au && byteBuffer[3] == 0x00u;
	const bool hasMagicNumberBigEndian = byteBuffer[0] == 0x4Du && byteBuffer[1] == 0x4Du && byteBuffer[2] == 0x00u && byteBuffer[3] == 0x2Au;

	if (!hasMagicNumberLittleEndian && !hasMagicNumberBigEndian)
	{
		return Frame();
	}

	// first we need to ensure that we have a little endian system

	InputDataTriple inputDataTriple(byteBuffer, size, 0);

	using ScopedTiffObject = ScopedObjectCompileTimeVoidT<TIFF*, &TIFFClose>;

	TIFF* tiff = TIFFClientOpen("tiffmemoryclient", "r", (thandle_t)&inputDataTriple, imageTifReadData_Load, imageTifDataInvalid, imageTifSeekData_Load, imageTifCloseData, imageTifSizeData_Load, imageTifMapData, imageTifUnmapData);

	if (tiff == nullptr)
	{
		return Frame();
	}

	const ScopedTiffObject scopedTiff(tiff);

	static_assert(sizeof(unsigned int) == 4u, "Invalid data type!");

	unsigned int width = 0u;
	unsigned int height = 0u;
	unsigned int samplesPerPixel = 0u;
	if (TIFFGetField(*scopedTiff, TIFFTAG_IMAGEWIDTH, &width) != 1 || TIFFGetField(*scopedTiff, TIFFTAG_IMAGELENGTH, &height) != 1 || TIFFGetField(*scopedTiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel) != 1 || width == 0u || height == 0u || samplesPerPixel == 0u)
	{
		return Frame();
	}

	unsigned short orientation = (unsigned short)(-1);
	if (TIFFGetField(*scopedTiff, TIFFTAG_ORIENTATION, &orientation) != 1 || (orientation != ORIENTATION_TOPLEFT && orientation != ORIENTATION_BOTLEFT))
	{
		return Frame();
	}

	ocean_assert(orientation == ORIENTATION_TOPLEFT || orientation == ORIENTATION_BOTLEFT);
	const FrameType::PixelOrigin pixelOrigin = (orientation == ORIENTATION_TOPLEFT) ? FrameType::ORIGIN_UPPER_LEFT : FrameType::ORIGIN_LOWER_LEFT;

	// **TODO** hot fix to ensure that images cannot be larger than 2^32 bytes

	if (uint64_t(width) * uint64_t(height) >= uint64_t(1073741823ull)) // width * height * 4 < 2^32
	{
		return Frame();
	}

	Frame result(FrameType(width, height, FrameType::FORMAT_RGBA32, pixelOrigin));

	// currently we keep it quite simple, we request an RGBA frame independent of the acutal data stored
	// this can be improved if necessary - however, tiff images seem not to have a high impact

	ocean_assert(result.isContinuous());
	if (TIFFReadRGBAImageOriented(*scopedTiff, width, height, result.data<uint32_t>(), orientation, 1) != 1)
	{
		return Frame();
	}

	switch (samplesPerPixel)
	{
		case 1u:
			CV::FrameConverter::Comfort::change(result, FrameType::FORMAT_Y8, true, WorkerPool::get().conditionalScopedWorker(result.pixels() >= 400u * 400u)());
			break;

		case 3u:
			CV::FrameConverter::Comfort::change(result, FrameType::FORMAT_RGB24, true, WorkerPool::get().conditionalScopedWorker(result.pixels() >= 400u * 400u)());
			break;

		default:
			break;
	}

	return result;
}

bool ImageTif::encodeImage(const Frame& frame, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted)
{
	ocean_assert(frame.isValid());

	if (hasBeenConverted)
	{
		*hasBeenConverted = false;
	}

	Frame convertedFrame;
	const Frame* outputFrame = &frame;

	if (frame.pixelFormat() != FrameType::FORMAT_RGB24 && frame.pixelFormat() != FrameType::FORMAT_RGBA32)
	{
		if (!allowConversion)
		{
			return false;
		}

		const FrameType::PixelFormat convertedPixelFormat = frame.hasAlphaChannel() ? FrameType::FORMAT_RGBA32 : FrameType::FORMAT_RGB24;

		if (!CV::FrameConverter::Comfort::convert(frame, convertedPixelFormat, convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
		{
			return false;
		}

		if (hasBeenConverted)
		{
			*hasBeenConverted = true;
		}

		outputFrame = &convertedFrame;
	}

	ocean_assert(outputFrame != nullptr && outputFrame->isValid());

	OutputDataTriple outputDataTriple(std::vector<uint8_t>(outputFrame->frameTypeSize()), 0, 0);

	using ScopedTiffObject = ScopedObjectCompileTimeVoidT<TIFF*, &TIFFClose>;

	TIFF* tiff = TIFFClientOpen("tiffmemoryclient", "w", (thandle_t)&outputDataTriple, imageTifDataInvalid, imageTifWriteData_Write, imageTifSeekData_Write, imageTifCloseData, imageTifSizeData_Write, imageTifMapData, imageTifUnmapData);

	if (tiff == nullptr)
	{
		return false;
	}

	ScopedTiffObject scopedTiff(tiff);

	if (TIFFSetField(*scopedTiff, TIFFTAG_IMAGEWIDTH, outputFrame->width()) != 1 || TIFFSetField(*scopedTiff, TIFFTAG_IMAGELENGTH, outputFrame->height()) != 1)
	{
		return false;
	}

	ocean_assert(outputFrame->pixelFormat() != FrameType::FORMAT_RGB24 || outputFrame->pixelFormat() != FrameType::FORMAT_RGBA32);
	const unsigned short tiffSamplesPerPixel = outputFrame->pixelFormat() == FrameType::FORMAT_RGB24 ? 3u : 4u;
	const unsigned int tiffBitsPerSample = 8u;

	if (TIFFSetField(*scopedTiff, TIFFTAG_SAMPLESPERPIXEL, tiffSamplesPerPixel) != 1 || TIFFSetField(*scopedTiff, TIFFTAG_BITSPERSAMPLE, tiffBitsPerSample) != 1)
	{
		return false;
	}

	if (outputFrame->pixelFormat() == FrameType::FORMAT_RGBA32)
	{
		// libtiff will internally copy the contents of this array before TIFFSetField() returns
		uint16_t tt[1] = {EXTRASAMPLE_ASSOCALPHA};
		if (TIFFSetField(*scopedTiff, TIFFTAG_EXTRASAMPLES, 1, tt) != 1)
		{
			return false;
		}
	}

	const unsigned short orientation = outputFrame->pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT ? ORIENTATION_TOPLEFT : ORIENTATION_BOTLEFT;

	if (TIFFSetField(*scopedTiff, TIFFTAG_ORIENTATION, orientation) != 1|| TIFFSetField(*scopedTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG) != 1 || TIFFSetField(*scopedTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB) != 1)
	{
		return false;
	}

	if (TIFFSetField(*scopedTiff, TIFFTAG_ROWSPERSTRIP, 1u) != 1 || TIFFSetField(*scopedTiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE) != 1)
	{
		return false;
	}

	for (unsigned int y = 0u; y < outputFrame->height(); ++y)
	{
		TIFFWriteEncodedStrip(*scopedTiff, y, const_cast<void*>(outputFrame->constrow<void>(y)), outputFrame->planeWidthBytes(0u));
	}

	scopedTiff.release(); // releasing before copying the data to ensure valid data

	ocean_assert(outputDataTriple.second() <= outputDataTriple.first().size());
	buffer.resize(outputDataTriple.second());
	memcpy(buffer.data(), outputDataTriple.first().data(), outputDataTriple.second());

	return true;
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_TIF
