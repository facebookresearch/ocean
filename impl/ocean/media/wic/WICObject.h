/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_WIC_WIC_OBJECT_H
#define META_OCEAN_MEDIA_WIC_WIC_OBJECT_H

#include "ocean/media/wic/WIC.h"

#include "ocean/base/Frame.h"

#include <Wincodec.h>

namespace Ocean
{

namespace Media
{

namespace WIC
{

/**
 * This class is the base class for all object inside this library.
 * @ingroup mediawic
 */
class OCEAN_MEDIA_WIC_EXPORT WICObject
{
	public:

		/**
		 * Translates a pixel format from Windows Imaging Component to a framework pixel format.
		 * @param format The Windows Imaging Component pixel format to be translated
		 * @param pixelOrigin Optional resulting Ocean specific pixel origin of the given format
		 * @return Ocean pixel format, FORMAT_UNDEFINED if no corresponding pixel format exists
		 */
		static FrameType::PixelFormat translatePixelFormat(const WICPixelFormatGUID& format, FrameType::PixelOrigin* pixelOrigin = nullptr);

		/**
		 * Determines the best matching Windows Imaging Component pixel format for a given Ocean pixel format.
		 * @param format The pixel format for which the best matching pixel format will be determined
		 * @param adjustedFormat The resulting adjusted pixel format as compromise in the case a direct matching pixel format does not exist, otherwise identical with format
		 * @param supportedPixelFormats Optional set of supported pixel formats, if known
		 * @return The WIC pixel format, GUID_NULL if no corresponding pixel format exists
		 */
		static WICPixelFormatGUID matchingPixelFormat(const FrameType::PixelFormat format, FrameType::PixelFormat& adjustedFormat, const std::vector<GUID>& supportedPixelFormats = std::vector<GUID>());

		/**
		 * Returns whether a Windows Imaging Component pixel format has an alpha channel.
		 * @param imagingFactory The imaging factory owner of the decoder, must be valid
		 * @param bitmapDecoder The bitmap decoder object from which the frame will be loaded, must be valid
		 * @param frameDecode The actual frame to check, must be valid
		 * @return True, if so
		 */
		static bool hasAlphaChannel(IWICImagingFactory* imagingFactory, IWICBitmapDecoder* bitmapDecoder, IWICBitmapFrameDecode* frameDecode);

		/**
		 * Finds the corresponding container format for a specified file extension.
		 * @param fileExtension The file extension for which the container format is requested, e.g., "jpg", "bmp", or "gif"
		 * @return The container format for the specified file extension, GUID_NULL if no matching container could be found
		 */
		static GUID findContainerFormat(const std::string& fileExtension);

		/**
		 * Translates a container format to the corresponding file extension.
		 * @param containerFormat The container format to be translated
		 * @return The resulting file extension, e.g., "jpg", "bmp", or "gif", empty if unknown
		 */
		static std::string translateContainerFormat(const GUID& containerFormat);

		/**
		 * Loads a frame from a bitmap decoder object.
		 * @param imagingFactory The imaging factory owner of the decoder, must be valid
		 * @param bitmapDecoder The bitmap decoder object from which the frame will be loaded, must be valid
		 * @return The resulting frame, an invalid frame if the frame could not be loaded
		 */
		static Frame loadFrameFromBitmapDecoder(IWICImagingFactory* imagingFactory, IWICBitmapDecoder* bitmapDecoder);

		/**
		 * Writes a frame to a bitmap encoder object.
		 * @param imagingFactory The imaging factory owner of the decoder, must be valid
		 * @param bitmapEncoder The bitmap encoder object to which the frame will be written, must be valid
		 * @param frame The frame to which will be written to the decoder, must be valid
		 * @param allowConversion True, to allow an internal conversion of the frame if BMP does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @return True, if succeeded
		 */
		static bool writeFrameToBitmapDecoder(IWICImagingFactory* imagingFactory, IWICBitmapEncoder* bitmapEncoder, const Frame& frame, const bool allowConversion = true, bool* hasBeenConverted = nullptr);
};

}

}

}

#endif // META_OCEAN_MEDIA_WIC_WIC_OBJECT_H
