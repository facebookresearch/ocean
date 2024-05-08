/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_H

#include "ocean/media/imageio/ImageIO.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/media/Image.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements an image class for ImageIO.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOImage :
	virtual public IIOObject,
	virtual public Image
{
	friend class IIOLibrary;

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

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
		explicit IIOImage(const std::string& url);

		/**
		 * Destructs an image object.
		 */
		~IIOImage() override;

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

		/// Start timestamp.
		Timestamp mediumStartTimestamp;

		/// Pause timestamp.
		Timestamp mediumPauseTimestamp;

		/// Stop timestamp.
		Timestamp mediumStopTimestamp;

		/// Determining whether this image is 'started' and holds valid image data.
		bool imageStarted;
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_H
