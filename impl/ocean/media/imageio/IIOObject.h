/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_OBJECT_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_OBJECT_H

#include "ocean/media/imageio/ImageIO.h"

#include "ocean/base/Frame.h"

#include "ocean/media/Image.h"

#include "ocean/platform/apple/Apple.h"

#include <ImageIO/ImageIO.h>

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class is the base class for all object inside this library.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOObject
{
	protected:

		/**
		 * Definition of a pair combining a pixel format with an Apple string.
		 */
		typedef std::pair<FrameType::PixelFormat, const CFStringRef> PixelFormatPair;

		/**
		 * Definition of an unordered map mapping color profile names to a pair with pixel formats.
		 */
		typedef std::unordered_map<std::string, PixelFormatPair> ColorProfileMap;

	public:

		/**
		 * Loads a frame from an image source.
		 * The resulting frame can have one of the following pixel formats:<br>
		 * FORMAT_Y8, FORMAT_RGB24, FORMAT_YA16, FORMAT_RGBA32, FORMAT_ARGB32.
		 * @param imageSource The image source from which the frame will be loaded, must be valid
		 * @param properties Optional resulting properties which are encoded in the image; nullptr if not of interest
		 * @return The resulting frame
		 */
		static Frame loadFrameFromImageSource(CGImageSourceRef imageSource, Media::Image::Properties* properties = nullptr);

		/**
		 * Loads a frame from an image source.
		 * The resulting frame can have one of the following pixel formats:<br>
		 * FORMAT_Y8, FORMAT_RGB24, FORMAT_YA16, FORMAT_RGBA32, FORMAT_ARGB32.
		 * @param image The image from which the frame will be loaded, must be valid
		 * @return The resulting frame
		 */
		static Frame loadFrameFromImage(CGImageRef image);

		/**
		 * Writes a frame to an image destination, in case the pixel format of the given frame is not supported by the destination, the function will fail.
		 * As this function does not provide an automatic color space conversion, the binary impact when using this function will be quite small.
		 * @param imageDestination The image destination to which the frame will be written, must be valid
		 * @param frame The frame which will be written to the destination, must be valid
		 * @param properties The properties to be used when writing the image, must be valid
		 * @return True, if succeeded
		 */
		static bool writeFrameToImageDestination(CGImageDestinationRef imageDestination, const Frame& frame, const Media::Image::Properties& properties = Media::Image::Properties());

		/**
		 * Writes a frame to an image destination, in case the pixel format of the given frame is not supported by the destination, the function may convert the frame internally (depending on 'allowConversion').
		 * As this function supports the conversion of color spaces, the binary impact when using this function may be quite high.
		 * @param imageDestination The image destination to which the frame will be written, must be valid
		 * @param frame The frame which will be written to the destination, must be valid
		 * @param allowConversion True, to allow an internal conversion of the frame if BMP does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @param properties The properties to be used when writing the image, must be valid
		 * @return True, if succeeded
		 */
		static bool writeFrameToImageDestination(CGImageDestinationRef imageDestination, const Frame& frame, const bool allowConversion, bool* hasBeenConverted = nullptr, const Media::Image::Properties& properties = Media::Image::Properties());

		/**
		 * Finds the corresponding uniform type identifier for a specified file extension.
		 * @param fileExtension The file extension for which the identifier is requested, possible values are "bmp", "gif", "jpg", "png", "tif"
		 * @return The uniform type identifier for the specified file extension, an invalid object if not matching identifer could be found
		 */
		static Platform::Apple::ScopedCFStringRef findUniformTypeIdentifier(const std::string& fileExtension);

		/**
		 * Translates a uniform type identifier to a corresponding file extension.
		 * @param typeIdentifier The uniform type identifier to translate
		 * @return The corresponding file extension, possible values are "bmp", "gif", "jpg", "png", "tif"
		 */
		static std::string translateUniformTypeIdentifier(CFStringRef typeIdentifier);

		/**
		 * Translates a Ocean-based pixel format to a CG-based color space and bitmap info.
		 * Some pixel formats need to be converted to an intermediate Ocean-based pixel format before a corresponding color space and bitmap info can be determined.
		 * @param pixelFormat The pixel format for which the corresponding color space and bimtap info will be determined
		 * @param colorProfileName The name of the color profile to be used, empty to use the default profile
		 * @param colorSpace The resulting CG-based color space matching to the given pixel format, must be released by the caller
		 * @param bitmapInfo The resulting CG-based bitmap info matching to the given pixel format
		 * @param targetPixelFormat The resulting target pixel format in which the given pixel format need to be converted before a corresponding color space and bitmap info can be determined
		 * @return True, if a corresponding color space and bitmap info could be determined; False, otherwise
		 */
		static bool translatePixelFormat(const FrameType::PixelFormat pixelFormat, const std::string& colorProfileName, Platform::Apple::ScopedCGColorSpaceRef& colorSpace, CGBitmapInfo& bitmapInfo, FrameType::PixelFormat& targetPixelFormat);

		/**
		 * Determines the color space.
		 * @param mainPixelFormat The main pixel format defining the color space, either FORMAT_RGB24 or FORMAT_Y8
		 * @param colorProfileName The optional color profile name, empty to use a default profile
		 * @return The color space
		 */
		static CGColorSpaceRef determineColorSpace(const FrameType::PixelFormat mainPixelFormat, const std::string& colorProfileName);

	protected:

		/**
		 * Disabled function to prevent wrong usage.
		 * @return Would return True on success
		 */
		static bool writeFrameToImageDestination(CGImageDestinationRef, const Frame&, const float) = delete;

		/**
		 * Disabled function to prevent wrong usage.
		 * @return Would return True on success
		 */
		static bool writeFrameToImageDestination(CGImageDestinationRef, const Frame&, const bool, bool*, const float) = delete;
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_OBJECT_H
