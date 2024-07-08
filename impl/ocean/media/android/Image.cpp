/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/Image.h"

#include "ocean/math/Numeric.h"

#include "ocean/platform/android/Bitmap.h"

#include <fcntl.h>

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30
	#include <android/imagedecoder.h>
#endif

namespace Ocean
{

namespace Media
{

namespace Android
{

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30

Frame Image::decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn, std::string* imageBufferTypeOut)
{
	ocean_assert(buffer != nullptr && size != 0);

	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	ScopedAImageDecoder aImageDecoder;
	if (AImageDecoder_createFromBuffer(buffer, size, &aImageDecoder.resetObject()) == ANDROID_IMAGE_DECODER_SUCCESS)
	{
		return decodeImage(*aImageDecoder);
	}

	return Frame();
}

#else

Frame Image::decodeImage(const void* /*buffer*/, const size_t /*size*/, const std::string& /*imageBufferTypeIn*/, std::string* /*imageBufferTypeOut*/)
{
	Log::debug() << "Media::Android::Image needs API 30+";

	return Frame();
}

#endif // __ANDROID_API__

Frame Image::readImage(const std::string& filename)
{
	ocean_assert(!filename.empty());

	if (filename.empty())
	{
		return Frame();
	}

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30

	const ScopedFileDescriptor fileDescriptor = ScopedFileDescriptor(open(filename.c_str(), O_RDONLY));

	if (*fileDescriptor >= 0)
	{
		ScopedAImageDecoder aImageDecoder;
		if (AImageDecoder_createFromFd(*fileDescriptor, &aImageDecoder.resetObject()) == ANDROID_IMAGE_DECODER_SUCCESS)
		{
			return decodeImage(*aImageDecoder);
		}
	}

#else
	Log::debug() << "Media::Android::Image needs API 30+";
#endif // __ANDROID_API__

	return Frame();
}

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30

Frame Image::decodeImage(AImageDecoder* aImageDecoder)
{
	ocean_assert(aImageDecoder != nullptr);

	const AImageDecoderHeaderInfo* aHeaderInfo = AImageDecoder_getHeaderInfo(aImageDecoder);

	const AndroidBitmapFormat androidBitmapFormat = AndroidBitmapFormat(AImageDecoderHeaderInfo_getAndroidBitmapFormat(aHeaderInfo));

	const FrameType::PixelFormat pixelFormat = Platform::Android::Bitmap::translateFormat(androidBitmapFormat);

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return Frame();
	}

	const int32_t width = AImageDecoderHeaderInfo_getWidth(aHeaderInfo);
	const int32_t height = AImageDecoderHeaderInfo_getHeight(aHeaderInfo);
	const int32_t minimumStrideBytes = AImageDecoder_getMinimumStride(aImageDecoder);

	if (width <= 0 || height <= 0 || minimumStrideBytes <= 0)
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	unsigned int planePaddingElements = 0u;
	if (!Frame::strideBytes2paddingElements(pixelFormat, (unsigned int)(width), (unsigned int)(minimumStrideBytes), planePaddingElements))
	{
		ocean_assert(false && "Invalid stride!");
		return Frame();
	}

	Frame frame(FrameType((unsigned int)(width), (unsigned int)(height), pixelFormat, FrameType::ORIGIN_UPPER_LEFT), planePaddingElements);

	const unsigned int planeStrideBytes = frame.strideBytes(0u);
	const unsigned int sizeBytes = frame.height() * planeStrideBytes;

	if (NumericT<size_t>::isInsideValueRange(planeStrideBytes) && NumericT<size_t>::isInsideValueRange(sizeBytes))
	{
		const int decodeResult = AImageDecoder_decodeImage(aImageDecoder, frame.data<void>(), size_t(planeStrideBytes), size_t(sizeBytes));

		if (decodeResult == ANDROID_IMAGE_DECODER_SUCCESS)
		{
			return frame;
		}

		Log::debug() << "Failed to decode image: " << decodeResult;
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	return Frame();
}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 30

}

}

}
