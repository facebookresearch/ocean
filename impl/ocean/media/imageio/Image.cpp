/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/Image.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/base/StringApple.h"

#include <CoreFoundation/CoreFoundation.h>

namespace Ocean
{

using namespace Platform::Apple;

namespace Media
{

namespace ImageIO
{

Frame Image::decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn, std::string* imageBufferTypeOut, Media::Image::Properties* properties)
{
	ocean_assert(buffer && size != 0);
	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	Frame result;

	const ScopedCFDataRef cfData(CFDataCreateWithBytesNoCopy(nullptr, (const UInt8*)buffer, CFIndex(size), kCFAllocatorNull));

	if (cfData.object() != nullptr)
	{
		ScopedCFDictionaryRef optionsDictionary;
		ScopedCFStringRef value;

		if (!imageBufferTypeIn.empty())
		{
			const CFStringRef key = kCGImageSourceTypeIdentifierHint;
			value = IIOObject::findUniformTypeIdentifier(imageBufferTypeIn);

			optionsDictionary = ScopedCFDictionaryRef(CFDictionaryCreate(nullptr, (const void**)(&key), (const void**)(&value.object()), 1, nullptr, nullptr));
		}

		const ScopedCGImageSourceRef cgImageSource(CGImageSourceCreateWithData(cfData.object(), optionsDictionary.object()));

		if (cgImageSource.object() != nullptr)
		{
			result = IIOObject::loadFrameFromImageSource(cgImageSource.object(), properties);

			if (result && imageBufferTypeOut)
			{
				CFStringRef sourceType = CGImageSourceGetType(cgImageSource.object());

				if (sourceType)
				{
					*imageBufferTypeOut = IIOObject::translateUniformTypeIdentifier(sourceType);
				}
			}
		}
	}

	return result;
}

bool Image::encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const Properties& properties)
{
	ocean_assert(properties.isValid());

	if (!frame.isValid())
	{
		return false;
	}

	const ScopedCFStringRef typeIdentifier = IIOObject::findUniformTypeIdentifier(imageType);

	if (typeIdentifier.object() == nullptr)
	{
		return false;
	}

	if (StringApple::toUTF8(typeIdentifier.object()) == "public.heic")
	{
		if (frame.width() == 1u && frame.height() == 1u)
		{
			Log::error() << "Heic images need to be larger than 1x1";
			return false;
		}
	}

	const ScopedCFMutableDataRef mutableData(CFDataCreateMutable(nullptr, 0));

	const ScopedCGImageDestinationRef imageDestination(CGImageDestinationCreateWithData(mutableData.object(), typeIdentifier.object(), 1, nullptr));

	if (mutableData.object() == nullptr || imageDestination.object() == nullptr)
	{
		return false;
	}

	if (!IIOObject::writeFrameToImageDestination(imageDestination.object(), frame, properties))
	{
		return false;
	}

	const uint8_t* data = CFDataGetMutableBytePtr(mutableData.object());
	const size_t size = size_t(CFDataGetLength(mutableData.object()));

	buffer.resize(size);
	memcpy(buffer.data(), data, size);

	return true;
}

bool Image::encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted, const Properties& properties)
{
	ocean_assert(properties.isValid());

	if (!frame.isValid())
	{
		return false;
	}

	const ScopedCFStringRef typeIdentifier = IIOObject::findUniformTypeIdentifier(imageType);

	if (typeIdentifier.object() == nullptr)
	{
		return false;
	}

	const ScopedCFMutableDataRef mutableData(CFDataCreateMutable(nullptr, 0));
	const ScopedCGImageDestinationRef imageDestination(CGImageDestinationCreateWithData(mutableData.object(), typeIdentifier.object(), 1, nullptr));

	if (mutableData.object() == nullptr || imageDestination.object() == nullptr)
	{
		return false;
	}

	if (!IIOObject::writeFrameToImageDestination(imageDestination.object(), frame, allowConversion, hasBeenConverted, properties))
	{
		return false;
	}

	const uint8_t* data = CFDataGetMutableBytePtr(mutableData.object());
	const size_t size = size_t(CFDataGetLength(mutableData.object()));

	buffer.resize(size);
	memcpy(buffer.data(), data, size);

	return true;
}

}

}

}
