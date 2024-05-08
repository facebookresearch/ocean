/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_PIXEL_IMAGE_H
#define META_OCEAN_MEDIA_PIXEL_IMAGE_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class PixelImage;

/**
 * Definition of a smart medium reference holding a pixel image object.
 * @see SmartMediumRef, PixelImage.
 * @ingroup media
 */
typedef SmartMediumRef<PixelImage> PixelImageRef;

/**
 * This class implements a frame medium object which uses an explicit frame only.
 * A pixel image can be used to define an explicit frame without any e.g. image or movie stream.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT PixelImage : virtual public FrameMedium
{
	friend class Manager;

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium:isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the transformation between the camera and device.
		 * @see PixelImage::device_T_camera(), setDevice_T_camera().
		 */
		HomogenousMatrixD4 device_T_camera() const override;

		/**
		 * Sets the transformation between the camera and device.
		 * @param device_T_camera The transformation to be set, must be valid
		 * @see device_T_camera().
		 */
		void setDevice_T_camera(const HomogenousMatrixD4& device_T_camera);

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause().
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

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
		 * Sets or changes the explicit frame of this pixel image.
		 * Beware: The frame will not be set if the medium is not started, further the frame should have a valid timestamp.<br>
		 * @param frame The frame to be set, the frame will be moved
		 * @param anyCamera The camera profile of the given frame, if known
		 * @return True, if succeeded
		 */
		virtual bool setPixelImage(Frame&& frame, SharedAnyCamera anyCamera = nullptr);

		/**
		 * Sets or changes the explicit frame of this pixel image.
		 * Beware: The frame will not be set if the medium is not started, further the frame should have a valid timestamp.<br>
		 * @param frame The frame to be set, the frame data will be copied
		 * @param anyCamera The camera profile of the given frame, if known
		 * @return True, if succeeded
		 */
		virtual bool setPixelImage(const Frame& frame, SharedAnyCamera anyCamera = nullptr);

	protected:

		/**
		 * Creates a new pixel image by an arbitrary url defining the name of the resulting object.
		 * @param url Arbitrary name to identify this pixel image later, if necessary
		 */
		explicit PixelImage(const std::string& url);

	protected:

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;

		/// Determining whether this image is 'started' and holds valid image data.
		bool isStarted_;

		/// The transformation between camera and device.
		HomogenousMatrixD4 device_T_camera_;
};

}

}

#endif // META_OCEAN_MEDIA_PIXEL_IMAGE_H
