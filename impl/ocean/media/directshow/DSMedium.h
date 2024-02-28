// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
