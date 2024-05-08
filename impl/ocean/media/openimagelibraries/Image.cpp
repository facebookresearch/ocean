/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/Image.h"
#include "ocean/media/openimagelibraries/ImageGif.h"
#include "ocean/media/openimagelibraries/ImageJpg.h"
#include "ocean/media/openimagelibraries/ImagePng.h"
#include "ocean/media/openimagelibraries/ImageTif.h"
#include "ocean/media/openimagelibraries/ImageWebp.h"

#include "ocean/media/special/ImageBmp.h"
#include "ocean/media/special/ImageNpy.h"
#include "ocean/media/special/ImagePfm.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

Frame Image::decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn, std::string* imageBufferTypeOut)
{
	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	const std::string fileExtension(String::toLower(imageBufferTypeIn));

	Frame result;

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "jpg" || imageBufferTypeIn == "jpeg" || imageBufferTypeIn == "jpe"))
	{
		result = ImageJpg::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "jpg";
		}
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "png"))
	{
		result = ImagePng::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "png";
		}
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG

	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "bmp"))
	{
		result = Media::Special::ImageBmp::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "bmp";
		}
	}

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF
	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "tif" || imageBufferTypeIn == "tiff"))
	{
		result = ImageTif::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "tif";
		}
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_TIF

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF
	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "gif"))
	{
		constexpr size_t maximalImages = 1;

		Frames frames = ImageGif::decodeImages(buffer, size, maximalImages);

		if (frames.size() == 1)
		{
			result = std::move(frames.front());

			if (imageBufferTypeOut)
			{
				*imageBufferTypeOut = "gif";
			}
		}
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP
	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "webp"))
	{
		result = ImageWebp::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "webp";
		}
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP

	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "pfm"))
	{
		result = Media::Special::ImagePfm::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "pfm";
		}
	}

	if (!result.isValid() && (imageBufferTypeIn.empty() || imageBufferTypeIn == "npy"))
	{
		result = Media::Special::ImageNpy::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "npy";
		}
	}

	return result;
}

bool Image::encodeImage(const Frame& frame, const std::string& imageType, std::vector<unsigned char>& buffer, const bool allowConversion, bool* hasBeenConverted)
{
	if (!frame.isValid() || imageType.empty())
	{
		return false;
	}

	const std::string imageTypeOut(String::toLower(imageType));

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	if (imageTypeOut == "jpg" || imageTypeOut == "jpeg" || imageTypeOut == "jpe")
	{
		return ImageJpg::encodeImage(frame, buffer, allowConversion, hasBeenConverted);
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	if (imageTypeOut == "png")
	{
		return ImagePng::encodeImage(frame, buffer, allowConversion, hasBeenConverted);
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG

	if (imageTypeOut == "bmp")
	{
		return Media::Special::ImageBmp::encodeImage(frame, buffer, allowConversion, hasBeenConverted);
	}

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF
	if (imageTypeOut == "tif" || imageTypeOut == "tiff")
	{
		return ImageTif::encodeImage(frame, buffer, allowConversion, hasBeenConverted);
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_TIF

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP
	if (imageTypeOut == "webp")
	{
		return ImageWebp::encodeImage(frame, buffer, allowConversion, hasBeenConverted);
	}
#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP

	if (imageTypeOut == "pfm")
	{
		if (Media::Special::ImagePfm::encodeImage(frame, buffer))
		{
			if (hasBeenConverted)
			{
				*hasBeenConverted = false;
			}

			return true;
		}

		return false;
	}

	if (imageTypeOut == "npy")
	{
		if (Media::Special::ImageNpy::encodeImage(frame, buffer))
		{
			if (hasBeenConverted)
			{
				*hasBeenConverted = false;
			}

			return true;
		}

		return false;
	}

	return false;
}

Frame Image::readImage(const std::string& filename)
{
	const std::string::size_type fileExtensionPos = filename.rfind('.');

	if (fileExtensionPos == std::string::npos || fileExtensionPos + 1 == filename.size())
	{
		return Frame();
	}

	std::ifstream inputStream(filename.c_str(), std::ios::binary);

	if (!inputStream.is_open())
	{
		Log::warning() << "Could not open image file \"" << filename << "\"";
		return Frame();
	}

	inputStream.seekg(0, inputStream.end);
	const uint64_t fileSize = uint64_t(inputStream.tellg());

	if (fileSize >= uint64_t(NumericT<size_t>::maxValue()))
	{
		// the file is too large for the memory
		return Frame();
	}

	if (fileSize == uint64_t(0))
	{
		Log::warning() << "The image \"" << filename << "\" does not contain any data";
		return Frame();
	}

	inputStream.seekg(0, inputStream.beg);

	std::vector<uint8_t> buffer = std::vector<uint8_t>(size_t(fileSize));
	inputStream.read((char*)buffer.data(), buffer.size());

	if (inputStream.bad())
	{
		return Frame();
	}

	return decodeImage(buffer.data(), buffer.size(), filename.substr(fileExtensionPos + 1));
}

bool Image::writeImage(const Frame& frame, const std::string& filename, const bool allowConversion, bool* hasBeenConverted)
{
	const std::string::size_type fileExtensionPos = filename.rfind('.');

	if (fileExtensionPos == std::string::npos || fileExtensionPos + 1 == filename.size())
	{
		return false;
	}

	std::vector<uint8_t> buffer;
	if (!encodeImage(frame, filename.substr(fileExtensionPos + 1), buffer, allowConversion, hasBeenConverted))
	{
		return false;
	}

	std::ofstream outputStream(filename.c_str(), std::ios::binary);

	if (!outputStream.is_open())
	{
		Log::warning() << "Could not open image file \"" << filename << "\"";
		return false;
	}

	outputStream.write((char*)buffer.data(), buffer.size());

	return outputStream.good();
}

}

}

}
