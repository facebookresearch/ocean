/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FFM_MEDIUM_H
#define META_OCEAN_MEDIA_FFM_MEDIUM_H

#include "ocean/media/ffmpeg/FFmpeg.h"

#include "ocean/media/Medium.h"

// Forward declaration.
struct AVFormatContext;

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

/**
 * This is the base class for all FFmpeg mediums.
 * @ingroup mediaffm
 */
class FFMMedium : virtual public Medium
{
	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted()
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @return Start timestamp
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @return Pause timestamp
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @return Stop timestamp
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit FFMMedium(const std::string& url);

		/**
		 * Destructs a FFMMedium object.
		 */
		~FFMMedium() override;

		/**
		 * Starts the medium.
		 * The internalStart() function will be called inside.
		 * @see Medium::start()
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * The internalPause() function will be called inside.
		 * @see Medium::pause()
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * The internalStop() function will be called inside.
		 * @see Medium::stop()
		 */
		bool stop() override;

		/**
		 * Internally starts the medium.
		 * @return True, if succeeded or if the medium is already started
		 */
		virtual bool internalStart() = 0;

		/**
		 * Internally pauses the medium.
		 * @return True, if succeeded or if the medium is already paused
		 */
		virtual bool internalPause() = 0;

		/**
		 * Internally stops the medium.
		 * @return True, if succeeded or if the medium is already stopped
		 */
		virtual bool internalStop() = 0;

		/**
		 * Creates a FFmpeg context and opens a given file.
		 * @param filename The name of the file to open, must be valid
		 * @return True, if succeeded
		 */
		bool createContextAndOpenFile(const std::string& filename);

		/**
		 * Releases the FFmpeg context.
		 */
		void releaseContext();

	protected:

		/// FFmpeg's format context object.
		AVFormatContext* avFormatContext_ = nullptr;

		/// Start timestamp
		Timestamp startTimestamp_ = Timestamp(false);

		/// Pause timestamp
		Timestamp pauseTimestamp_ = Timestamp(false);

		/// Stop timestamp
		Timestamp stopTimestamp_ = Timestamp(false);
};

}

}

}

#endif // META_OCEAN_MEDIA_FFM_MEDIUM_H
