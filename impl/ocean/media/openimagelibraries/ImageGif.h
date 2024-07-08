/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OIL_IMAGE_GIF_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OIL_IMAGE_GIF_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements read and write functions for Gif images.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT ImageGif
{
	protected:

		/// Forward declaration.
		class SourceBuffer;

	public:

		/**
		 * Decodes a GIF image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @param maximalImages The maximal number of images that will be returned, with range [0, infinity), 0 to return all images
		 * @return The frames containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frames decodeImages(const void* buffer, const size_t size, const size_t maximalImages = 0u);

		/**
		 * Reads/loads all images from a specified gif file.
		 * @param filename The name of the file from which the image will be loaded, must be valid
		 * @param maximalImages The maximal number of images that will be returned, with range [0, infinity), 0 to return all images
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 * @see writeImage().
		 */
		static Frames readImages(const std::string& filename, const size_t maximalImages = 0u);

	protected:

		/**
		 * Verifies whether the provided GIF is valid or corrupt.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return True, if the GIF is valid
		 */
		static bool verifyGif(const void* buffer, const size_t size);
};

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OIL_IMAGE_GIF_H
