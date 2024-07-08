/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IMAGE_H
#define META_OCEAN_MEDIA_IMAGEIO_IMAGE_H

#include "ocean/media/imageio/ImageIO.h"

#include "ocean/base/Frame.h"

#include "ocean/media/Image.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements read, write, decocde, and encode functions for all file formats supported by the ImageIO media library.
 * This class is not derived from Media::Image as this class provides simple static functions to handle images.<br>
 * Instead, IIOImage is derived from Media::Image.
 *
 * ImageIO supports for the following image types: bmp, gif, jpg, png, tiff.
 * The following pixel formats are supported by the individual image types:
 * <pre>
 * ImageType:    Pixel format:     Pixel origin:          Supports compression quality:
 * bmp           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 *
 * gif           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 *
 * jpg           FORMAT_Y8         ORIGIN_UPPER_LEFT      yes
 * jpg           FORMAT_RGB24      ORIGIN_UPPER_LEFT      yes
 *
 * heic          FORMAT_RGB24      ORIGIN_UPPER_LEFT      yes
 * heic          FORMAT_RGBA32     ORIGIN_UPPER_LEFT      yes
 *
 * png           FORMAT_Y8         ORIGIN_UPPER_LEFT      no
 * png           FORMAT_YA16       ORIGIN_UPPER_LEFT      no
 * png           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 * png           FORMAT_RGBA32     ORIGIN_UPPER_LEFT      no
 *
 * tif           FORMAT_RGB24      ORIGIN_UPPER_LEFT      no
 * tif           FORMAT_RGBA32     ORIGIN_UPPER_LEFT      no
 * </pre>
 * @see IIOImage.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT Image
{
	public:

		/**
 		 * Re-definition of Media::Image::Properties.
 		 */
		using Properties = Media::Image::Properties;

	public:

		/**
		 * Decodes (reads/loads) an image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @param imageBufferTypeIn Type of the given image that is stored in the buffer, should be specified if known (e.g. the file extension of a corresponding image file)
		 * @param imageBufferTypeOut Optional type of the given image that is stored in the buffer, as determined by the decoder (if possible)
		 * @param properties Optional resulting properties which are encoded in the image; nullptr if not of interest
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 * @see writeImage().
		 */
		static Frame decodeImage(const void* buffer, const size_t size, const std::string& imageBufferTypeIn = std::string(), std::string* imageBufferTypeOut = nullptr, Media::Image::Properties* properties = nullptr);

		/**
		 * Encodes (writes) a given frame as image (with specified image type) to a resulting buffer.
		 * In case, the pixel format of the given frame is not supported by the destination, the function will fail.<br>
		 * As this function does not provide an automatic color space conversion, the binary impact when using this function will be quite small.<br>
		 * Note: Depending on the encoder, a provided quality value may not have any impact, e.g., png images do not support a loosely compression.
		 * @param frame The frame to be written, must be valid
		 * @param imageType The file extension of the image to be created (e.g. jpg, png, bmp, or tif), must be defined
		 * @param buffer The resulting buffer storing the binary information of the given image
		 * @param properties The properties to be used when writing the image, must be valid
		 * @return True, if succeeded; False, if the frame could not be encoded e.g., if the pixel format is not supported or if the `imageType` is unknown
		 * @see readImage().
		 */
		static bool encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const Properties& properties = Properties());

		/**
		 * Encodes (writes) a given frame as image (with specified image type) to a resulting buffer.
		 * In case, the pixel format of the given frame is not supported by the destination, the function may convert the frame internally (depending on 'allowConversion').
		 * As this function supports the conversion of color spaces, the binary impact when using this function may be quite high.<br>
		 * Note: Depending on the encoder, a provided quality value may not have any impact, e.g., png images do not support a loosely compression.
		 * @param frame The frame to be written, must be valid
		 * @param imageType The file extension of the image to be created (e.g. jpg, png, bmp, or tif), must be defined
		 * @param buffer The resulting buffer storing the binary information of the resulting image
		 * @param allowConversion True, to allow an internal conversion of the frame if the image type does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @param properties The properties to be used when writing the image, must be valid
		 * @return True, if succeeded; False, if the frame could not be encoded e.g., if the pixel format could not be converted to a valid pixel format or if the `imageType` is unknown
		 * @see readImage().
		 */
		static bool encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted = nullptr, const Properties& properties = Properties());

		/**
		 * Reads/loads an image from a specified file.
		 * @param filename The name of the file from which the image will be loaded, must be valid
		 * @param properties Optional resulting properties which are encoded in the image; nullptr if not of interest
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 * @see writeImage().
		 */
		static Frame readImage(const std::string& filename, Media::Image::Properties* properties = nullptr);

		/**
		 * Writes a given frame to a specified file.
		 * In case, the pixel format of the given frame is not supported by the destination, the function will fail.<br>
		 * As this function does not provide an automatic color space conversion, the binary impact when using this function will be quite small.<br>
		 * Note: Depending on the encoder, a provided quality value may not have any impact, e.g., png images do not support a loosely compression.
		 * @param frame The frame to be written, must be valid
		 * @param filename The name of the file to which the frame will be written, must contain a valid image extension like e.g. jpg, png, bmp, or tif), must be valid
		 * @param properties The properties to be used when writing the image, must be valid
		 * @return True, if succeeded
		 * @see readImage().
		 */
		static bool writeImage(const Frame& frame, const std::string& filename, const Properties& properties = Properties());

		/**
		 * Writes a given frame to a specified file.
		 * In case, the pixel format of the given frame is not supported by the destination, the function may convert the frame internally (depending on 'allowConversion').
		 * As this function supports the conversion of color spaces, the binary impact when using this function may be quite high.<br>
		 * Note: Depending on the encoder, a provided quality value may not have any impact, e.g., png images do not support a loosely compression.
		 * @param frame The frame to be written, must be valid
		 * @param filename The name of the file to which the frame will be written, must contain a valid image extension like e.g. jpg, png, bmp, or tif), must be valid
		 * @param allowConversion True, to allow an internal conversion of the frame if the image type does not support the given frame type; False, to prevent a conversion and to stop writing the image
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @param properties The properties to be used when writing the image, must be valid
		 * @return True, if succeeded
		 * @see readImage().
		 */
		static bool writeImage(const Frame& frame, const std::string& filename, const bool allowConversion, bool* hasBeenConverted = nullptr, const Properties& properties = Properties());

	protected:

		/**
		 * Disabled function to prevent wrong usage.
		 * @return Would return True on success
		 */
		static bool encodeImage(const Frame&, const std::string&, std::vector<uint8_t>&, const float) = delete;

		/**
		 * Disabled function to prevent wrong usage.
		 * @return Would return True on success
		 */
		static bool writeImage(const Frame&, const std::string&, const float) = delete;
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IMAGE_H
