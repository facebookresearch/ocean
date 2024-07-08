/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_BUFFER_IMAGE_H
#define META_OCEAN_MEDIA_BUFFER_IMAGE_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameMedium.h"

#include "ocean/base/Memory.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class BufferImage;

/**
 * Definition of a smart medium reference holding a buffer image object.
 * @see SmartMediumRef, BufferImage.
 * @ingroup media
 */
typedef SmartMediumRef<BufferImage> BufferImageRef;

/**
 * This class implements a frame medium object which is defined by a given buffer and image or frame type.
 * A buffer image can be used to define an explicit frame without loading the content from e.g. a file but from a given memory buffer.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT BufferImage : virtual public FrameMedium
{
	friend class Manager;

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium:isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see Medium::startTimestmap().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see Medium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see Medium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

		/**
		 * Sets or changes the buffer of this image, the given memory buffer is copied and stored internally.
		 * @param buffer The buffer to be copied, must be valid
		 * @param size The size of the given memory buffer in bytes, with range [1, infinity)
		 * @param imageBufferType Type of the image that is stored in the given buffer, should be specified if known (e.g. the file extension of a corresponding image file)
		 * @return True, if succeeded
		 */
		virtual bool setBufferImage(const void* buffer, const size_t size, const std::string& imageBufferType = std::string());

		/**
		 * Returns the image buffer type.
		 * @return Type of the image buffer, if known (e.g. the file extension of a corresponding image file)
		 */
		inline const std::string& getImageBufferType() const;

	protected:

		/**
		 * Creates a new buffer image by an arbitrary url defining the name of the resulting object.
		 * @param url Arbitrary name to identify this pixel image later, if necessary
		 */
		explicit BufferImage(const std::string& url);

		/**
		 * Destructs a buffer image object.
		 */
		~BufferImage() override = default;

	protected:

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;

		/// Determining whether this image is 'started' and holds valid image data.
		bool started_ = false;

		/// The object's memory.
		Memory memory_;

		/// The type of the buffer image.
		std::string bufferType_;
};

inline const std::string& BufferImage::getImageBufferType() const
{
	return bufferType_;
}

}

}

#endif // META_OCEAN_MEDIA_BUFFER_IMAGE_H
