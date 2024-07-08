/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/image/Image.h"

#include "ocean/media/special/Image.h"

#if defined(__APPLE__)
	#include "ocean/media/imageio/Image.h"
#elif defined(_WINDOWS)
	#include "ocean/media/wic/Image.h"
#else
	#include "ocean/media/openimagelibraries/Image.h"
#endif

namespace Ocean
{

namespace IO
{

namespace Image
{

Frame decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn, std::string* imageBufferTypeOut)
{
	if (buffer == nullptr || size == 0)
	{
		ocean_assert(false && "Invalid input!");
		return Frame();
	}

#if defined(__APPLE__)
	Frame frame = Media::ImageIO::Image::decodeImage(buffer, size, imageBufferTypeIn, imageBufferTypeOut);
#elif defined(_WINDOWS)
	Frame frame = Media::WIC::Image::decodeImage(buffer, size, imageBufferTypeIn, imageBufferTypeOut);
#else
	Frame frame = Media::OpenImageLibraries::Image::decodeImage(buffer, size, imageBufferTypeIn, imageBufferTypeOut);
#endif

	if (!frame.isValid())
	{
		frame = Media::Special::Image::decodeImage(buffer, size, imageBufferTypeIn, imageBufferTypeOut);
	}

	return frame;
}

bool encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const Properties& properties)
{
	OCEAN_SUPPRESS_UNUSED_WARNING(properties);

	if (!frame.isValid() || imageType.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

#if defined(__APPLE__)
	bool result = Media::ImageIO::Image::encodeImage(frame, imageType, buffer, properties);
#elif defined(_WINDOWS)
	bool result = Media::WIC::Image::encodeImage(frame, imageType, buffer /*, properties*/);
#else
	bool result = Media::OpenImageLibraries::Image::encodeImage(frame, imageType, buffer /*, properties*/);
#endif

	if (result == false)
	{
		result = Media::Special::Image::encodeImage(frame, imageType, buffer /*, properties*/);
	}

	return result;
}

Frame readImage(const std::string& filename)
{
	if (filename.empty())
	{
		ocean_assert(false && "Invalid input!");
		return Frame();
	}

#if defined(__APPLE__)
	Frame frame = Media::ImageIO::Image::readImage(filename);
#elif defined(_WINDOWS)
	Frame frame = Media::WIC::Image::readImage(filename);
#else
	Frame frame = Media::OpenImageLibraries::Image::readImage(filename);
#endif

	if (!frame.isValid())
	{
		frame = Media::Special::Image::readImage(filename);
	}

	return frame;
}

bool writeImage(const Frame& frame, const std::string& filename, const Properties& properties)
{
	OCEAN_SUPPRESS_UNUSED_WARNING(properties);

	if (!frame.isValid() || filename.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

#if defined(__APPLE__)
	bool result = Media::ImageIO::Image::writeImage(frame, filename, properties);
#elif defined(_WINDOWS)
	bool result = Media::WIC::Image::writeImage(frame, filename /*, properties*/);
#else
	bool result = Media::OpenImageLibraries::Image::writeImage(frame, filename /*, properties*/);
#endif

	if (result == false)
	{
		result = Media::Special::Image::writeImage(frame, filename /*, properties*/);
	}

	return result;
}

bool Comfort::encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted, const Properties& properties)
{
	OCEAN_SUPPRESS_UNUSED_WARNING(properties);

	if (!frame.isValid() || imageType.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

#if defined(__APPLE__)
	bool result = Media::ImageIO::Image::encodeImage(frame, imageType, buffer, allowConversion, hasBeenConverted, properties);
#elif defined(_WINDOWS)
	bool result = Media::WIC::Image::encodeImage(frame, imageType, buffer, allowConversion, hasBeenConverted /*, properties*/);
#else
	bool result = Media::OpenImageLibraries::Image::encodeImage(frame, imageType, buffer, allowConversion, hasBeenConverted /*, properties*/);
#endif

	if (result == false)
	{
		result = Media::Special::Image::encodeImage(frame, imageType, buffer, allowConversion, hasBeenConverted /*, properties*/);
	}

	return result;
}

bool Comfort::writeImage(const Frame& frame, const std::string& filename, const bool allowConversion, bool* hasBeenConverted, const Properties& properties)
{
	OCEAN_SUPPRESS_UNUSED_WARNING(properties);

	if (!frame.isValid() || filename.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

#if defined(__APPLE__)
	bool result = Media::ImageIO::Image::writeImage(frame, filename, allowConversion, hasBeenConverted, properties);
#elif defined(_WINDOWS)
	bool result = Media::WIC::Image::writeImage(frame, filename, allowConversion, hasBeenConverted /*, properties*/);
#else
	bool result = Media::OpenImageLibraries::Image::writeImage(frame, filename, allowConversion, hasBeenConverted /*, properties*/);
#endif

	if (result == false)
	{
		result = Media::Special::Image::writeImage(frame, filename, allowConversion, hasBeenConverted /*, properties*/);
	}

	return result;
}

}

}

}
