/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_MEDIUM_H
#define META_OCEAN_MEDIA_DS_MEDIUM_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSGraphObject.h"

#include "ocean/media/Medium.h"

DISABLE_WARNINGS_BEGIN
	#include <Dshow.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This is the base class for all direct show mediums.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSMedium :
	virtual public DSGraphObject,
	virtual public Medium
{
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

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit DSMedium(const std::string& url);

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
		 * Specifies whether the media playback time will be respected or whether the media content will be provided as fast as possible.
		 * @param respectPlaybackTime True, to deliver the media content based on the presentation time; False, to ignore the presentation clock and to deliver the media content as fast as possible
		 */
		virtual bool setRespectPlaybackTime(const bool respectPlaybackTime);

	protected:

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_MEDIUM_H
