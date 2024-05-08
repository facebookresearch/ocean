/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/special/Image.h"
#include "ocean/media/special/ImageBmp.h"
#include "ocean/media/special/ImageNpy.h"
#include "ocean/media/special/ImageOcn.h"
#include "ocean/media/special/ImagePfm.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

Frame Image::decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn, std::string* imageBufferTypeOut)
{
	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	const std::string fileExtension(String::toLower(imageBufferTypeIn));

	Frame result;

	if (!result.isValid() && (imageBufferTypeIn.empty()|| imageBufferTypeIn == "bmp"))
	{
		result = ImageBmp::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "bmp";
		}
	}

	if (!result.isValid() && (imageBufferTypeIn.empty()|| imageBufferTypeIn == "pfm"))
	{
		result = ImagePfm::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "pfm";
		}
	}

	if (!result.isValid() && (imageBufferTypeIn.empty()|| imageBufferTypeIn == "npy"))
	{
		result = ImageNpy::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "npy";
		}
	}

	if (!result.isValid() && (imageBufferTypeIn.empty()|| imageBufferTypeIn == "ocn"))
	{
		result = ImageOcn::decodeImage(buffer, size);

		if (result && imageBufferTypeOut)
		{
			*imageBufferTypeOut = "ocn";
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

	if (imageTypeOut == "bmp")
	{
		if (ImageBmp::encodeImage(frame, buffer, allowConversion, hasBeenConverted))
		{
			if (hasBeenConverted)
			{
				*hasBeenConverted = false;
			}

			return true;
		}

		return false;
	}

	if (imageTypeOut == "pfm")
	{
		if (ImagePfm::encodeImage(frame, buffer))
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
		if (ImageNpy::encodeImage(frame, buffer))
		{
			if (hasBeenConverted)
			{
				*hasBeenConverted = false;
			}

			return true;
		}

		return false;
	}

	if (imageTypeOut == "ocn")
	{
		if (ImageOcn::encodeImage(frame, buffer))
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

	if (fileSize == 0)
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
