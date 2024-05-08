/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/Image.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/base/StringApple.h"

#include "ocean/platform/apple/Apple.h"

#import <Foundation/Foundation.h>

#include <ImageIO/ImageIO.h>

namespace Ocean
{

using namespace Platform::Apple;

namespace Media
{

namespace ImageIO
{

Frame Image::readImage(const std::string& filename, Media::Image::Properties* properties)
{
	ocean_assert(!filename.empty());
	if (filename.empty())
	{
		return Frame();
	}

	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(filename)];

	const ScopedCGImageSourceRef cgImageSource(CGImageSourceCreateWithURL((CFURLRef)nsUrl, nullptr));

	Frame result;

	if (cgImageSource.object() != nullptr)
	{
		result = IIOObject::loadFrameFromImageSource(cgImageSource.object(), properties);
	}

	return result;
}

bool Image::writeImage(const Frame& frame, const std::string& filename, const Media::Image::Properties& properties)
{
	ocean_assert(properties.isValid());

	const std::string::size_type fileExtensionPos = filename.rfind('.');

	if (fileExtensionPos == std::string::npos || fileExtensionPos + 1 == filename.size())
	{
		return false;
	}

	const std::string filenameExtension = filename.substr(fileExtensionPos + 1);

	const ScopedCFStringRef typeIdentifier(IIOObject::findUniformTypeIdentifier(filenameExtension));

	if (typeIdentifier.object() == nullptr)
	{
		return false;
	}

	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(filename)];

	const ScopedCGImageDestinationRef imageDestination(CGImageDestinationCreateWithURL((CFURLRef)(nsUrl), typeIdentifier.object(), 1, nullptr));

	if (imageDestination.object() == nullptr)
	{
		return false;
	}

	return IIOObject::writeFrameToImageDestination(imageDestination.object(), frame, properties);
}

bool Image::writeImage(const Frame& frame, const std::string& filename, const bool allowConversion, bool* hasBeenConverted, const Media::Image::Properties& properties)
{
	ocean_assert(properties.isValid());

	const std::string::size_type fileExtensionPos = filename.rfind('.');

	if (fileExtensionPos == std::string::npos || fileExtensionPos + 1 == filename.size())
	{
		return false;
	}

	const std::string filenameExtension = filename.substr(fileExtensionPos + 1);

	const ScopedCFStringRef typeIdentifier(IIOObject::findUniformTypeIdentifier(filenameExtension));

	if (typeIdentifier.object() == nullptr)
	{
		return false;
	}

	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(filename)];

	const ScopedCGImageDestinationRef imageDestination(CGImageDestinationCreateWithURL((CFURLRef)(nsUrl), typeIdentifier.object(), 1, nullptr));

	if (imageDestination.object() == nullptr)
	{
		return false;
	}

	return IIOObject::writeFrameToImageDestination(imageDestination.object(), frame, allowConversion, hasBeenConverted, properties);
}

}

}

}
