/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/ImagePng.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG

#include <png.h>

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

Frame ImagePng::decodeImage(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size > 0);

	const uint8_t* byteBuffer = (const uint8_t*)(buffer);

	if (size <= 8 || png_sig_cmp(byteBuffer, 0, 8) != 0)
	{
		// we do not have a PNG signature, so we stop here
		return Frame();
	}

	png_structp pngReadStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (pngReadStruct == nullptr)
	{
		return Frame();
	}

	png_infop pngInfoStruct = png_create_info_struct(pngReadStruct);

	if (pngInfoStruct == nullptr)
	{
		png_destroy_read_struct(&pngReadStruct, nullptr, nullptr);
		return Frame();
	}

	// we need to create the Frame instance before the setjump() function, otherwise the frame will not be released in case of an error
	Frame result;

	const ScopedFunctionVoid scopedDestroyReadStructFunction(std::bind(&png_destroy_read_struct, &pngReadStruct, &pngInfoStruct, nullptr));

	// we define the central error handling function
	if (setjmp(png_jmpbuf(pngReadStruct)))
	{
		return Frame();
	}

	// we setup our own input buffer
	DataInputPair inputDataPair = std::make_pair(&byteBuffer, size);
	png_set_read_fn(pngReadStruct, (void*)&inputDataPair, png_rw_ptr(readInputData));

	// no we determine the image's properties
	png_read_info(pngReadStruct, pngInfoStruct);

	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bitDepthPerChannel = 0;
	int colorType = -1;

	if (png_get_IHDR(pngReadStruct, pngInfoStruct, &width, &height, &bitDepthPerChannel, &colorType, nullptr, nullptr, nullptr) != 1)
	{
		return Frame();
	}

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

	if (colorType == PNG_COLOR_TYPE_PALETTE && bitDepthPerChannel >= 1 && bitDepthPerChannel <= 8)
	{
		png_set_palette_to_rgb(pngReadStruct);

		if (png_get_valid(pngReadStruct, pngInfoStruct, PNG_INFO_tRNS) != 0)
		{
			png_set_tRNS_to_alpha(pngReadStruct);
		}

		png_read_update_info(pngReadStruct, pngInfoStruct);

		const png_byte channels = png_get_channels(pngReadStruct, pngInfoStruct);

		if (channels == png_byte(3))
		{
			pixelFormat = FrameType::FORMAT_RGB24;
		}
		else if (channels == png_byte(4))
		{
			pixelFormat = FrameType::FORMAT_RGBA32;
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}
	else
	{
		pixelFormat = translatePixelFormat(colorType, bitDepthPerChannel);
	}

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return Frame();
	}

	const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	// **TODO** hot fix to ensure that images cannot be larger than 2^32 bytes

	ocean_assert(frameType.channels() <= 4u);

	if (!frameType.isValid() || uint64_t(frameType.width()) * uint64_t(frameType.height()) >= uint64_t(1073741823ull)) // width * height * 4 < 2^32
	{
		return Frame();
	}

	if (bitDepthPerChannel > 8 && Processor::isLittleEndian())
	{
		// PNG files store 16-bit pixels in network byte order (big-endian, i.e. most significant bytes first).
		// png_set_swap() shall switch the byte-order to little-endian (i.e., least significant bits first).
		png_set_swap(pngReadStruct);
	}

	const size_t pngBytesPerRow = png_get_rowbytes(pngReadStruct, pngInfoStruct);

	result = Frame(frameType);

	if (size_t(result.planeWidthBytes(0u)) != pngBytesPerRow)
	{
		ocean_assert(false && "do we handle stride?");
		return Frame();
	}

	unsigned int numberOfPasses = 1u;

	const png_byte interlaceType = png_get_interlace_type(pngReadStruct, pngInfoStruct);

	if (interlaceType == PNG_INTERLACE_ADAM7)
	{
		const int passes = png_set_interlace_handling(pngReadStruct);

		if (passes >= 1 && passes <= 256) // currently PNG should return 7
		{
			numberOfPasses = (unsigned int)(passes);
		}
		else
		{
			ocean_assert(false && "Invalid interlace type");
			return Frame();
		}
	}

	for (unsigned int nPass = 0u; nPass < numberOfPasses; ++nPass)
	{
		for (unsigned int y = 0u; y < result.height(); ++y)
		{
			png_read_row(pngReadStruct, result.row<uint8_t>(y), nullptr);
		}
	}

	return result;
}

bool ImagePng::encodeImage(const Frame& frame, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted)
{
	ocean_assert(frame.isValid());

	if (hasBeenConverted)
	{
		*hasBeenConverted = false;
	}

	Frame convertedFrame;
	const Frame* outputFrame = &frame;

	if (!isPixelFormatSupported(frame.pixelFormat()) || !isPixelOriginSupported(frame.pixelOrigin()))
	{
		if (!allowConversion)
		{
			return false;
		}

		FrameType::PixelFormat convertedPixelFormat = FrameType::FORMAT_UNDEFINED;

		if (isPixelFormatSupported(frame.pixelFormat()))
		{
			convertedPixelFormat = frame.pixelFormat();
		}
		else
		{
			// we try to get the best matching pixel format possible

			if (frame.hasAlphaChannel())
			{
				convertedPixelFormat = frame.channels() >= 3u ? FrameType::FORMAT_RGBA32 : FrameType::FORMAT_YA16;
			}
			else
			{
				convertedPixelFormat = frame.channels() >= 3u ? FrameType::FORMAT_RGB24 : FrameType::FORMAT_Y8;
			}
		}

		if (!CV::FrameConverter::Comfort::convert(frame, convertedPixelFormat, FrameType::ORIGIN_UPPER_LEFT, convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
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
	ocean_assert(outputFrame->pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	png_structp pngWriteStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (pngWriteStruct == nullptr)
	{
		return false;
	}

	png_infop pngInfoStruct = png_create_info_struct(pngWriteStruct);

	if (pngInfoStruct == nullptr)
	{
		png_destroy_write_struct(&pngWriteStruct, nullptr);
		return false;
	}

	// we define the central error handling function
	if (setjmp(png_jmpbuf(pngWriteStruct)))
	{
		png_destroy_write_struct(&pngWriteStruct, &pngInfoStruct);
		return false;
	}

	OutputDataPair outputDataPair(std::vector<uint8_t>(outputFrame->size() / 4u), 0);

	// we setup our own output buffer
	png_set_write_fn(pngWriteStruct, (void*)&outputDataPair, png_rw_ptr(writeOutputData), png_flush_ptr(flushOutputData));

	int pngColorType = -1;
	int pngBitDepthPerChannel = 1;
	if (!translatePixelFormat(outputFrame->pixelFormat(), pngColorType, pngBitDepthPerChannel))
	{
		png_destroy_write_struct(&pngWriteStruct, &pngInfoStruct);
		return false;
	}

	png_set_IHDR(pngWriteStruct, pngInfoStruct, outputFrame->width(), outputFrame->height(), pngBitDepthPerChannel, pngColorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(pngWriteStruct, pngInfoStruct);

	if (pngBitDepthPerChannel > 8 && Processor::isLittleEndian())
	{
		// PNG files store 16-bit pixels in network byte order (big-endian, i.e. most significant bytes first).
		// png_set_swap() shall switch the byte-order to little-endian (i.e., least significant bits first).
		png_set_swap(pngWriteStruct);
	}

	const size_t pngBytesPerRow = png_get_rowbytes(pngWriteStruct, pngInfoStruct);

	if (size_t(outputFrame->planeWidthBytes(0u)) != pngBytesPerRow)
	{
		ocean_assert(false && "do we handle stride?");
		png_destroy_write_struct(&pngWriteStruct, &pngInfoStruct);

		return false;
	}

	for (unsigned int y = 0u; y < outputFrame->height(); ++y)
	{
		png_write_row(pngWriteStruct, outputFrame->constrow<uint8_t>(y));
	}

	png_write_end(pngWriteStruct, pngInfoStruct);

	png_destroy_write_struct(&pngWriteStruct, &pngInfoStruct);

	ocean_assert(outputDataPair.second <= outputDataPair.first.size());
	buffer.resize(outputDataPair.second);
	memcpy(buffer.data(), outputDataPair.first.data(), outputDataPair.second);

	return true;
}

FrameType::PixelFormat ImagePng::translatePixelFormat(const int pngColorType, const int pngBitDepthPerChannel)
{
	switch (int64_t(pngColorType) | (int64_t(pngBitDepthPerChannel) << 32ll))
	{
		case (int64_t(PNG_COLOR_TYPE_GRAY) | (8ll << 32ll)):
			return FrameType::FORMAT_Y8;

		case (int64_t(PNG_COLOR_TYPE_GRAY) | (16ll << 32ll)):
			return FrameType::FORMAT_Y16;

		case (int64_t(PNG_COLOR_TYPE_GA) | (8ll << 32ll)):
			return FrameType::FORMAT_YA16;

		case (int64_t(PNG_COLOR_TYPE_RGB) | (8ll << 32ll)):
			return FrameType::FORMAT_RGB24;

		case (int64_t(PNG_COLOR_TYPE_RGBA) | (8ll << 32ll)):
			return FrameType::FORMAT_RGBA32;

		case (int64_t(PNG_COLOR_TYPE_RGBA) | (16ll << 32ll)):
			return FrameType::FORMAT_RGBA64;

		default:
			return FrameType::FORMAT_UNDEFINED;
	}
}

bool ImagePng::translatePixelFormat(const FrameType::PixelFormat pixelFormat, int& pngColorType, int& pngBitDepthPerChannel)
{
	pngColorType = -1;

	switch (pixelFormat)
	{
		case FrameType::FORMAT_Y8:
			pngColorType = PNG_COLOR_TYPE_GRAY;
			break;

		case FrameType::FORMAT_Y16:
			pngColorType = PNG_COLOR_TYPE_GRAY;
			break;

		case FrameType::FORMAT_YA16:
			pngColorType = PNG_COLOR_TYPE_GA;
			break;

		case FrameType::FrameType::FORMAT_RGB24:
			pngColorType = PNG_COLOR_TYPE_RGB;
			break;

		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_RGBA64:
			pngColorType = PNG_COLOR_TYPE_RGBA;
			break;

		default:
			return false;
	}

	const unsigned int channels = FrameType::channels(pixelFormat);
	const FrameType::DataType dataType = FrameType::dataType(pixelFormat);

	pngBitDepthPerChannel = channels != 0u ? int(FrameType::bytesPerDataType(dataType) * 8u) : 0u;
	ocean_assert(pngBitDepthPerChannel == 8u || pngBitDepthPerChannel == 16u);

	ocean_assert(translatePixelFormat(pngColorType, pngBitDepthPerChannel) == pixelFormat);

	return true;
}

void ImagePng::readInputData(void* png_ptr, unsigned char* outBytes, const size_t byteCountToRead)
{
	static_assert(sizeof(png_structp) == sizeof(png_ptr), "Invalid data type!");
	static_assert(sizeof(png_bytep) == sizeof(outBytes), "Invalid data type!");
	static_assert(sizeof(png_size_t) == sizeof(byteCountToRead), "Invalid data type!");

	DataInputPair* inputData = reinterpret_cast<DataInputPair*>(png_get_io_ptr(png_structp(png_ptr)));
	ocean_assert(inputData != nullptr);

	if (byteCountToRead <= inputData->second)
	{
		ocean_assert(outBytes != nullptr);

		memcpy(outBytes, *inputData->first, byteCountToRead);
		*inputData->first += byteCountToRead;
		inputData->second -= byteCountToRead;
	}
	else
	{
		png_error(png_structp(png_ptr), "not enough memory");
	}
}

void ImagePng::writeOutputData(void* png_ptr, unsigned char* buffer, const size_t size)
{
	static_assert(sizeof(png_structp) == sizeof(png_ptr), "Invalid data type!");
	static_assert(sizeof(png_bytep) == sizeof(buffer), "Invalid data type!");
	static_assert(sizeof(png_size_t) == sizeof(size), "Invalid data type!");

	OutputDataPair* outputData = reinterpret_cast<OutputDataPair*>(png_get_io_ptr(png_structp(png_ptr)));

	if (outputData->second + size > outputData->first.size())
	{
		outputData->first.resize(outputData->first.size() + max(size_t(1024 * 64), size));
	}

	memcpy(outputData->first.data() + outputData->second, buffer, size);
	outputData->second += size;
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG
