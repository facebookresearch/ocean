/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_IMAGE_H
#define META_OCEAN_MEDIA_ANDROID_IMAGE_H

#include "ocean/media/android/Android.h"

#include "ocean/base/Frame.h"
#include "ocean/base/ScopedObject.h"

#include <unistd.h>

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30
	#include <android/imagedecoder.h>

	#ifndef OCEAN_MEDIA_ANDROID_IMAGE_AVAILABLE
		#define OCEAN_MEDIA_ANDROID_IMAGE_AVAILABLE
	#endif
#endif


namespace Ocean
{

namespace Media
{

namespace Android
{

class Image
{
	protected:

		/**
		 * Definition of a scoped file based on a file descriptor.
		 */
		using ScopedFileDescriptor = ScopedObjectCompileTimeT<int, int, int, close, 0, true, -1>;

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30

		/**
		 * Definition of a scoped object wrapping an AImageDecoder object.
		 */
		using ScopedAImageDecoder = ScopedObjectCompileTimeVoidT<AImageDecoder*, AImageDecoder_delete>;
#endif

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
		 * Reads/loads an image from a specified file.
		 * @param filename The name of the file from which the image will be loaded, must be valid
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 * @see writeImage().
		 */
		static Frame readImage(const std::string& filename);

	protected:

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 30

		/**
		 * Decodes an image from a given Android image decoder object.
		 * @param aImageDecoder The Android image decoder from which the image will be decoded, must be valid
		 * @return The resulting frame, invalid in case of an error
		 */
		static Frame decodeImage(AImageDecoder* aImageDecoder);

#endif
};

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_IMAGE_H
