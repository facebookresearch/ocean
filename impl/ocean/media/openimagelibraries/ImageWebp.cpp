/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/ImageWebp.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP

#include <webp/encode.h>
#include <webp/decode.h>

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

Frame ImageWebp::decodeImage(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size > 0);

	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	WebPBitstreamFeatures webPBitstreamFeatures;
	const VP8StatusCode webPGetFeaturesStatus = WebPGetFeatures((uint8_t*)buffer, size, &webPBitstreamFeatures);

	if (webPGetFeaturesStatus != VP8_STATUS_OK)
	{
		Log::debug() << "Failed to decode WEBP with status code: " << translateVP8StatusCode(webPGetFeaturesStatus);
		return Frame();
	}

	if (webPBitstreamFeatures.has_animation)
	{
		Log::debug() << "Failed to decode WEBP because it contains animations, which are currently not supported";
		return Frame();
	}

	if (webPBitstreamFeatures.width <= 0 || webPBitstreamFeatures.height <= 0)
	{
		Log::debug() << "Failed to decode WEBP because of invalid image dimensions in header";
		return Frame();
	}

	const unsigned int width = int(webPBitstreamFeatures.width);
	const unsigned int height = int(webPBitstreamFeatures.height);

	// A convenience typedef to select the right decoding function.
	typedef uint8_t* (*WebPDecodeFunctionPtr)(const uint8_t* data, size_t data_size, uint8_t* output_buffer, size_t output_buffer_size, int output_stride);

	WebPDecodeFunctionPtr webPDecodeFunctionPtr = nullptr;
	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

	if (webPBitstreamFeatures.has_alpha)
	{
		pixelFormat = FrameType::FORMAT_RGBA32;
		webPDecodeFunctionPtr = WebPDecodeRGBAInto;
	}
	else
	{
		pixelFormat = FrameType::FORMAT_RGB24;
		webPDecodeFunctionPtr = WebPDecodeRGBInto;
	}

	ocean_assert(webPDecodeFunctionPtr != nullptr && pixelFormat != FrameType::FORMAT_UNDEFINED);

	Frame result(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));

	if (webPDecodeFunctionPtr((uint8_t*)buffer, size, result.data<uint8_t>(), result.size(), result.strideBytes()) == nullptr)
	{
		Log::debug() << "Failed to decode WEBP";
		return Frame();
	}

	return result;
}

bool ImageWebp::encodeImage(const Frame& frame, std::vector<unsigned char>& buffer, const bool allowConversion, bool* hasBeenConverted, const float quality)
{
	ocean_assert(frame);

	if (frame.width() > (unsigned int)WEBP_MAX_DIMENSION || frame.height() > (unsigned int)WEBP_MAX_DIMENSION)
	{
		return false;
	}

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
			// For everything else, try to convert the pixel format to RGB.
			convertedPixelFormat = FrameType::FORMAT_RGB24;
		}

		if (!CV::FrameConverter::Comfort::convert(frame, convertedPixelFormat, FrameType::ORIGIN_UPPER_LEFT, convertedFrame, false, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
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
	ocean_assert(isFrameTypeSupported(outputFrame->pixelFormat(), outputFrame->pixelOrigin()));

	WebPConfig webpConfig;
	WebPPicture webpPicture;
	if (!WebPConfigPreset(&webpConfig, WEBP_PRESET_DEFAULT, quality) || !WebPPictureInit(&webpPicture))
	{
		return false;
	}

	// Ensure that allocated memory is freed automatically when this function is exited.
	ScopedFunctionVoid scopedWebPPictureFree(std::bind(&WebPPictureFree, &webpPicture));

	// Enable lossless compression
	webpConfig.lossless = 1;

	if (outputFrame->hasAlphaChannel())
	{
		// Preserve the exact RGB/BGR values under transparent pixels. Otherwise, this invisible RGB/BGR information is discarded for better compression.
		webpConfig.exact = 1;
	}

	// Required to avoid conversion to a YUV format before saving; not setting this will cause the encode-decode test to fail because the average errors will exceed a max-error threshold.
	webpPicture.use_argb = 1;

	webpPicture.width = int(outputFrame->width());
	webpPicture.height = int(outputFrame->height());

	int webpPictureImportStatus = 0;

	switch (outputFrame->pixelFormat())
	{
		case FrameType::FORMAT_RGB24:
			webpPictureImportStatus = WebPPictureImportRGB(&webpPicture, (const uint8_t*)outputFrame->constdata<uint8_t>(), outputFrame->strideBytes());
			break;

		case FrameType::FORMAT_RGBA32:
			webpPictureImportStatus = WebPPictureImportRGBA(&webpPicture, (const uint8_t*)outputFrame->constdata<uint8_t>(), outputFrame->strideBytes());
			break;

		case FrameType::FORMAT_BGR24:
			webpPictureImportStatus = WebPPictureImportBGR(&webpPicture, (const uint8_t*)outputFrame->constdata<uint8_t>(), outputFrame->strideBytes());
			break;

		case FrameType::FORMAT_BGRA32:
			webpPictureImportStatus = WebPPictureImportBGRA(&webpPicture, (const uint8_t*)outputFrame->constdata<uint8_t>(), outputFrame->strideBytes());
			break;

		default:
			ocean_assert(false && "Never be here!");
			return false;
	}

	if (webpPictureImportStatus <= 0)
	{
		return false;
	}

	WebPMemoryWriter writer;
	WebPMemoryWriterInit(&writer);
	webpPicture.writer = WebPMemoryWrite;
	webpPicture.custom_ptr = &writer;

	bool succeeded = false;

	if (WebPEncode(&webpConfig, &webpPicture) && writer.size != 0)
	{
		buffer.resize(writer.size);
		memcpy(buffer.data(), writer.mem, writer.size);

		succeeded = true;
	}

	WebPMemoryWriterClear(&writer);

	return succeeded;
}

std::string ImageWebp::translateVP8StatusCode(const int statusCode)
{
	switch (statusCode)
	{
		case VP8_STATUS_OK:
			return "STATUS_OK";

		case VP8_STATUS_OUT_OF_MEMORY:
			return "STATUS_OUT_OF_MEMORY";

		case VP8_STATUS_INVALID_PARAM:
			return "STATUS_INVALID_PARAM";

		case VP8_STATUS_BITSTREAM_ERROR:
			return "STATUS_BITSTREAM_ERROR";

		case VP8_STATUS_UNSUPPORTED_FEATURE:
			return "STATUS_UNSUPPORTED_FEATURE";

		case VP8_STATUS_SUSPENDED:
			return "STATUS_SUSPENDED";

		case VP8_STATUS_USER_ABORT:
			return "STATUS_USER_ABORT";

		case VP8_STATUS_NOT_ENOUGH_DATA:
			return "STATUS_NOT_ENOUGH_DATA";

		default:
			ocean_assert(false && "This should never happen!");
			break;
	}

	return "UNKNOWN";
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP
