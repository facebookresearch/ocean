/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_WIC_WIC_BUFFER_IMAGE_H
#define META_OCEAN_MEDIA_WIC_WIC_BUFFER_IMAGE_H

#include "ocean/media/wic/WIC.h"
#include "ocean/media/wic/WICObject.h"

#include "ocean/media/BufferImage.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

/**
 * This class implements a buffer image class for WIC.
 * @ingroup mediawic
 */
class OCEAN_MEDIA_WIC_EXPORT WICBufferImage :
	virtual public WICObject,
	virtual public BufferImage
{
	friend class WICLibrary;

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
		explicit WICBufferImage(const std::string& url);

		/**
		 * Destructs an image object.
		 */
		virtual ~WICBufferImage();

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

	protected:

		/// Recent frame type.
		FrameType imageRecentFrameType;
};

}

}

}

#endif // META_OCEAN_MEDIA_WIC_WIC_BUFFER_IMAGE_H
