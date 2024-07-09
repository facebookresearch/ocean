/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/ImageJpg.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG

#include <jpeglib.h>

#include <csetjmp>

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * Error-handling callback function
 * @param cinfo Context object containing state required for longjmp in case of error
 */
void imageJpgErrorExit(j_common_ptr cinfo);

struct ImageJpgErrorManagerStruct
{
	/// Public field.
	struct jpeg_error_mgr pub;

	/// Return caller.
	jmp_buf setjmp_buffer;
};

void imageJpgErrorExit(j_common_ptr cinfo)
{
	// we get our own error manager
	ImageJpgErrorManagerStruct* myerr = (ImageJpgErrorManagerStruct*)(cinfo->err);

	// we jump into our return function defined by setjmp
	longjmp(myerr->setjmp_buffer, 1);
}

Frame ImageJpg::decodeImage(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size > 0);

	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	struct ImageJpgErrorManagerStruct errorManager;
	struct jpeg_decompress_struct decompressStruct = {};

	// we need to create the Frame instance before the setjump() function, otherwise the frame will not be released in case of an error
	Frame result;

	const ScopedFunctionVoid scopedDestroyDecompressStructFunction(std::bind(&jpeg_destroy_decompress, &decompressStruct));

	// first, we set our own error manager
	decompressStruct.err = jpeg_std_error(&errorManager.pub);
	errorManager.pub.error_exit = imageJpgErrorExit;

	if (setjmp(errorManager.setjmp_buffer))
	{
		return Frame();
	}

	// then, we create a de-compressor object
	jpeg_create_decompress(&decompressStruct);

	decompressStruct.do_fancy_upsampling = boolean(0);

	unsigned char* nonConstBuffer = static_cast<unsigned char*>(const_cast<void*>(buffer));

	// now we set the binary input buffer
	jpeg_mem_src(&decompressStruct, nonConstBuffer, (unsigned long)size);

	// we read the header of the jpeg image
	if (JPEG_HEADER_OK != jpeg_read_header(&decompressStruct, boolean(1)))
	{
		// the header seems to have a wrong format so that we stop here
		return Frame();
	}

	// we start the decompression
	jpeg_start_decompress(&decompressStruct);

	// no we should know the frame type of the resulting image

	const FrameType::PixelFormat pixelFormat = translatePixelFormat(decompressStruct.out_color_space, decompressStruct.data_precision, decompressStruct.out_color_components);

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		// we do not support the pixel format so we stop here
		return Frame();
	}

	const unsigned int width = decompressStruct.output_width;
	const unsigned int height = decompressStruct.output_height;

	if (width == 0u || height == 0u)
	{
		return Frame();
	}

	// we always have the origin in the upper left corner
	const FrameType::PixelOrigin pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;

	const FrameType frameType(width, height, pixelFormat, pixelOrigin);
	ocean_assert(frameType.isValid());

	// **TODO** hot fix to ensure that images cannot be larger than 2^32 bytes

	ocean_assert(frameType.channels() <= 3u);
	if (uint64_t(width) * uint64_t(height) >= uint64_t(1431655764ull)) // width * height * 3 < 2^32
	{
		return Frame();
	}

	ocean_assert(!result.isValid());
	result.set(frameType, true /*forceOwner*/, true /*forceWritable*/);

	unsigned int yRow = 0u;

	// now we simply read each line by another
	while (decompressStruct.output_scanline < decompressStruct.output_height)
	{
		uint8_t* resultRow = result.row<uint8_t>(yRow);

		const unsigned int readLines = jpeg_read_scanlines(&decompressStruct, &resultRow, 1u);

		ocean_assert(readLines == 1u);
		if (readLines != 1u)
		{
			// something must have gone wrong so we stop here
			return Frame();
		}

		++yRow;
	}

	// cleaning up any jpeg related data
	jpeg_finish_decompress(&decompressStruct);

	return result;
}

bool ImageJpg::encodeImage(const Frame& frame, std::vector<unsigned char>& buffer, const bool allowConversion, bool* hasBeenConverted, const int quality)
{
	ocean_assert(frame);

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

			if (frame.pixelFormat() == FrameType::FORMAT_YUVA32)
			{
				convertedPixelFormat = FrameType::FORMAT_YUV24;
			}
			else if (frame.channels() >= 3u)
			{
				convertedPixelFormat = FrameType::FORMAT_RGB24;
			}
			else
			{
				convertedPixelFormat = FrameType::FORMAT_Y8;
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

	int jpegColorSpace = -1;
	int jpegPrecision = -1;
	int jpegNumberComponents = -1;
	if (!translatePixelFormat(outputFrame->pixelFormat(), jpegColorSpace, jpegPrecision, jpegNumberComponents))
	{
		return false;
	}

	struct ImageJpgErrorManagerStruct errorManager;
	struct jpeg_compress_struct compressStruct = {};

	const ScopedFunctionVoid scopedDestroyCompressStructFunction(std::bind(&jpeg_destroy_compress, &compressStruct));

	// first, we set our own error manager
	compressStruct.err = jpeg_std_error(&errorManager.pub);
	errorManager.pub.error_exit = imageJpgErrorExit;

	if (setjmp(errorManager.setjmp_buffer))
	{
		return false;
	}

	// then, we create a compressor object
	jpeg_create_compress(&compressStruct);

	unsigned char* outputBuffer = nullptr;
	unsigned long outputSize = 0ul;
	jpeg_mem_dest(&compressStruct, &outputBuffer, &outputSize);

	compressStruct.image_width = outputFrame->width();
	compressStruct.image_height = outputFrame->height();
	compressStruct.input_components = jpegNumberComponents;
	compressStruct.in_color_space = J_COLOR_SPACE(jpegColorSpace);

	jpeg_set_defaults(&compressStruct);

	jpeg_set_quality(&compressStruct, quality, boolean(1));

	jpeg_start_compress(&compressStruct, boolean(1));

	unsigned int yRow = 0u;

	while (compressStruct.next_scanline < compressStruct.image_height)
	{
		static_assert(std::is_same<JSAMPLE, uint8_t>::value, "Invalid data type!");

		const uint8_t* outputRow = outputFrame->constrow<uint8_t>(yRow);

		jpeg_write_scanlines(&compressStruct, const_cast<uint8_t**>(&outputRow), 1);

		++yRow;
	}

	jpeg_finish_compress(&compressStruct);

	buffer.resize(outputSize);
	memcpy(buffer.data(), outputBuffer, outputSize);

	free(outputBuffer);

	return true;
}

FrameType::PixelFormat ImageJpg::translatePixelFormat(const int jpegColorSpace, const int jpegPrecision, const int jpegNumberComponents)
{
	switch (int64_t(jpegColorSpace) | (int64_t(jpegPrecision) << 32ll))
	{
		case (int64_t(JCS_GRAYSCALE) | (8ll << 32ll)):
		{
			ocean_assert(jpegNumberComponents == 1);
			if (jpegNumberComponents != 1)
			{
				return FrameType::FORMAT_UNDEFINED;
			}

			return FrameType::FORMAT_Y8;
		}

		case (int64_t(JCS_GRAYSCALE) | (16ll << 32ll)):
		{
			ocean_assert(jpegNumberComponents == 1);
			if (jpegNumberComponents != 1)
			{
				return FrameType::FORMAT_UNDEFINED;
			}

			return FrameType::FORMAT_Y16;
		}

		case (int64_t(JCS_RGB) | (8ll << 32ll)):
		{
			ocean_assert(jpegNumberComponents == 3);
			if (jpegNumberComponents != 3)
			{
				return FrameType::FORMAT_UNDEFINED;
			}

			return FrameType::FORMAT_RGB24;
		}

		case (int64_t(JCS_YCbCr) | (8ll << 32ll)):
		{
			ocean_assert(jpegNumberComponents == 3);
			if (jpegNumberComponents != 3)
			{
				return FrameType::FORMAT_UNDEFINED;
			}

			return FrameType::FORMAT_YUV24;
		}

		default:
			return FrameType::FORMAT_UNDEFINED;

		//JCS_CMYK,   /* C/M/Y/K */
		//JCS_YCCK,   /* Y/Cb/Cr/K */
		//JCS_BG_RGB, /* big gamut red/green/blue, bg-sRGB */
		//JCS_BG_YCC  /* big gamut Y/Cb/Cr, bg-sYCC */
	}
}

bool ImageJpg::translatePixelFormat(const FrameType::PixelFormat pixelFormat, int& jpegColorSpace, int& jpegPrecision, int& jpegNumberComponents)
{
	switch (pixelFormat)
	{
		case  FrameType::FORMAT_Y8:
			jpegColorSpace = JCS_GRAYSCALE;
			jpegPrecision = 8;
			jpegNumberComponents = 1;
			break;

		case FrameType::FORMAT_Y16:
			jpegColorSpace = JCS_GRAYSCALE;
			jpegPrecision = 16;
			jpegNumberComponents = 1;
			break;

		case FrameType::FORMAT_RGB24:
			jpegColorSpace = JCS_RGB;
			jpegPrecision = 8;
			jpegNumberComponents = 3;
			break;

		case FrameType::FORMAT_YUV24:
			jpegColorSpace = JCS_YCbCr;
			jpegPrecision = 8;
			jpegNumberComponents = 3;
			break;

		default:
			return false;
	}

	ocean_assert(translatePixelFormat(jpegColorSpace, jpegPrecision, jpegNumberComponents) == pixelFormat);

	return true;
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG
