/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_IMAGE_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_IMAGE_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements read and write functions for all file formats supported by the OpenImageLibraries media library.
 * This class is not derived from Media::Image as this class is a simple wrapper for ImageBmp, ImageJpg, ImagePng, and ImageTif only.<br>
 * Instead, OILImage is derived from Media::Image.
 *
 * OpenImageLibraries supports for the following image types: bmp, jpg, png, tiff.
 * The following pixel formats are supported by the individual image types:
 * <pre>
 * ImageType:    Pixel format:     Pixel origin:          Supports compression quality:
 * bmp           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 *
 * jpg           FORMAT_Y8         ORIGIN_UPPER_LEFT      yes
 * jpg           FORMAT_Y16        ORIGIN_UPPER_LEFT      yes
 * jpg           FORMAT_RGB24      ORIGIN_UPPER_LEFT      yes
 * jpg           FORMAT_YUV24      ORIGIN_UPPER_LEFT      yes
 *
 * png           FORMAT_Y8         ORIGIN_UPPER_LEFT      no
 * png           FORMAT_Y16        ORIGIN_UPPER_LEFT      no
 * png           FORMAT_YA16       ORIGIN_UPPER_LEFT      no
 * png           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 * png           FORMAT_RGBA32     ORIGIN_UPPER_LEFT      no
 * png           FORMAT_RGBA64     ORIGIN_UPPER_LEFT      no
 *
 * webp          FORMAT_RGB24      ORIGIN_UPPER_LEFT      yes
 * webp          FORMAT_BGR24      ORIGIN_UPPER_LEFT      yes
 *
 * tif           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 * tif           FORMAT_RGBA32     ORIGIN_UPPER_LEFT      no
 * </pre>
 * @see OILImage.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT Image
{
	public:

		/**
		 * Decodes (reads/loads) an image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @param imageBufferTypeIn Type of the given image that is stored in the buffer, should be specified if known (e.g. the file extension of a corresponding image file)
		 * @param imageBufferTypeOut Optional type of the given image that is stored in the buffer, as determined by the decoder (if possible)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 * @see writeImage().
		 */
		static Frame decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn = std::string(), std::string* imageBufferTypeOut = nullptr);

		/**
		 * Encodes (writes) a given frame as image (with specified image type) to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param imageType The file extension of the image to be created (e.g. jpg, png, gif, or tif), must be defined
		 * @param buffer The resulting buffer storing the binary information of the image
		 * @param allowConversion True, to allow an internal conversion of the frame if does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @return True, if succeeded; False, if the frame could not be written as image e.g., if the frame contained an alpha channel
		 * @see readImage().
		 */
		static bool encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const bool allowConversion = true, bool* hasBeenConverted = nullptr);

		/**
		 * Reads/loads an image from a specified file.
		 * @param filename The name of the file from which the image will be loaded, must be valid
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 * @see writeImage().
		 */
		static Frame readImage(const std::string& filename);

		/**
		 * Writes a given frame to a specified file.
		 * @param frame The frame to be written, must be valid
		 * @param filename The name of the file to which the frame will be written, must contain a valid image extension like e.g. jpg, png, gif, or tif), must be valid
		 * @param allowConversion True, to allow an internal conversion of the frame if the image format does not support the given frame type; False, to prevent a conversion and to stop writing the file
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @return True, if succeeded
		 * @see readImage().
		 */
		static bool writeImage(const Frame& frame, const std::string& filename, const bool allowConversion = true, bool* hasBeenConverted = nullptr);
};

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_IMAGE_H
