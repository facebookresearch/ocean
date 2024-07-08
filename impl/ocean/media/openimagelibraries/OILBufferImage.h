/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_BUFFER_IMAGE_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_BUFFER_IMAGE_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/media/BufferImage.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements a buffer image class for OpenImageLibraries.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT OILBufferImage : virtual public BufferImage
{
	friend class OILLibrary;

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

		/**
		 * Releases the image.
		 */
		void release();

		/**
		 * Sets the preferred frame pixel format.
		 * @see FrameMedium::setPreferredFramePixelFormat().
		 */
		bool setPreferredFramePixelFormat(const FrameType::PixelFormat pixelFormat) override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit OILBufferImage(const std::string& url);

		/**
		 * Destructs an image object.
		 */
		~OILBufferImage() override;

		/**
		 * (Re-)loads the image.
		 * @return True, if succeeded
		 */
		bool loadImage();
};

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_BUFFER_IMAGE_H
