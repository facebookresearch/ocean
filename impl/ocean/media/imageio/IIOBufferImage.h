/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_BUFFER_IMAGE_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_BUFFER_IMAGE_H

#include "ocean/media/imageio/ImageIO.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/media/BufferImage.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements a buffer image class for ImageIO.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOBufferImage :
	virtual public IIOObject,
	virtual public BufferImage
{
	friend class IIOLibrary;

	public:

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Clones this medium and returns a new independent instance of this medium.
		 * @see Medium::clone().
		 */
		MediumRef clone() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit IIOBufferImage(const std::string& url);

		/**
		 * Destructs an image object.
		 */
		~IIOBufferImage() override;

		/**
		 * Sets the preferred frame pixel format.
		 * @see FrameMedium::setPreferredFramePixelFormat().
		 */
		bool setPreferredFramePixelFormat(const FrameType::PixelFormat format) override;

	protected:

		/**
		 * (Re-)loads the image.
		 * @return True, if succeeded
		 */
		bool loadImage();

		/**
		 * Releases the image.
		 */
		void release();
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_BUFFER_IMAGE_H
