/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_FILE_SEQUENCE_H
#define META_OCEAN_MEDIA_IMAGE_FILE_SEQUENCE_H

#include "ocean/media/Media.h"
#include "ocean/media/ImageSequence.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Media
{

/*
 * This class implements the base class for all image sequences based on actual files (and not on databases containing images).
 * the class is mainly an intermediate helper class to simplify the implementation of an ImageSequence based on files.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImageFileSequence :
	public virtual ImageSequence,
	protected Thread
{
	public:

		/**
		 * Returns the duration of the finite medium.
		 * @see FiniteMedium::duration().
		 */
		double duration() const override;

		/**
		 * Returns the duration without speed consideration.
		 * @see FiniteMedium::normalDuration().
		 */
		double normalDuration() const override;

		/**
		 * Returns the recent position of the finite medium.
		 * @see FiniteMedium::position().
		 */
		double position() const override;

		/**
		 * Returns the speed of the finite medium.
		 * The speed of an image sequence is identical with the preferred frame rate (fps).
		 * @return The current speed of this image sequence in fps, with range [0, infinity)
		 */
		float speed() const override;

		/**
		 * Returns the url of the current image.
		 * @see ImageSequence::currentUrl().
		 */
		std::string currentUrl() const override;

		/**
		 * Returns the number of images part of the sequence.
		 * @see ImageSequence::images().
		 */
		unsigned int images() const override;

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
		 * Sets the recent position of the finit medium.
		 * @see FiniteMedium::setPosition().
		 */
		bool setPosition(const double position) override;

		/**
		 * Sets the speed of the finite medium.
		 * The speed of an image sequence is identical with the preferred frame rate (fps).
		 * @param speed The speed to be set in fps, with range [0, infinity)
		 * @see setPreferredFrameFrequency().
		 */
		bool setSpeed(const float speed) override;

		/**
		 * Forces the loading of the next image in the sequence.
		 * @see ImageSequence::forceNextFrame().
		 */
		bool forceNextFrame() override;

	protected:

		/**
		 * Creates a new image file sequence by a given url.
		 * @param url Url of the image
		 */
		explicit ImageFileSequence(const std::string& url);

		/**
		 * Destructs an image file sequence object.
		 */
		~ImageFileSequence() override;

		/**
		 * Thread run function.
		 * @see Thread::threadRun().
		 */
		void threadRun()  override;

		/**
		 * Determines the image sequence parameters.
		 * @return True, if succeeded
		 */
		bool determineSequence();

		/**
		 * Returns the filename of a specific sequence image.
		 * @param index Index of the sequence image to create the filename for
		 * @return Sequence image filename
		 */
		std::string imageFilename(const unsigned int index) const;

		/**
		 * Loads a new image specified by the filename.
		 * @param filename Filename of the image to be loaded
		 * @param timestamp Frame timestamp to be used
		 * @param frame Optional frame receiving the image data, otherwise the frame will be added to the frame container
		 * @return True, if succeeded
		 */
		virtual bool loadImage(const std::string& filename, const Timestamp timestamp, Frame* frame = nullptr) = 0;

	protected:

		/// Start timestamp.
		Timestamp mediumStartTimestamp;

		/// Pause timestamp.
		Timestamp mediumPauseTimestamp;

		/// Stop timestamp.
		Timestamp mediumStopTimestamp;

		/// Image sequence prefix filename.
		std::string mediumFilenamePrefix;

		/// Image sequence filename type.
		std::string mediumFilenameType;

		/// Next frame in the sequence.
		Frame mediumNextFrame;
};

}

}

#endif // META_OCEAN_MEDIA_IMAGE_FILE_SEQUENCE_H
