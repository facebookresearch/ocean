/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_FINITE_MEDIUM_H
#define META_OCEAN_MEDIA_MF_FINITE_MEDIUM_H

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/MFMedium.h"

#include "ocean/media/FiniteMedium.h"

#include "ocean/base/Scheduler.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class is the base class for all Media Foundation finite mediums.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFFiniteMedium :
	virtual public FiniteMedium,
	virtual public MFMedium
{
	friend class MFFrameMedium;

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
		 * Sets the recent position of the finite medium.
		 * @see FiniteMedium::setPosition().
		 */
		bool setPosition(const double position) override;

		/**
		 * Returns the speed of the finite medium.
		 * @see FiniteMedium::speed().
		 */
		float speed() const override;

		/**
		 * Sets the speed of the finite medium.
		 * @see FiniteMedium::setSpeed().
		 */
		bool setSpeed(const float speed) override;

		/**
		 * Returns whether a previous task has been finished.
		 * @see FiniteMedium::taskFinished().
		 */
		bool taskFinished() const override;

	protected:

		/**
		 * Creates a new finite medium by a given URL.
		 * @param url The URL of the finite medium
		 */
		explicit MFFiniteMedium(const std::string& url);

		/**
		 * Destructs a MFFiniteMedium object.
		 */
		~MFFiniteMedium() override;

		/**
		 * Informs this finite medium object that the medium has been stopped (without knowledge of this finite medium).
		 */
		void mediumHasStopped();

		/**
		 * Starts the media session.
		 * @return True, if succeeded or if the session is already started
		 */
		bool startMediaSession() override;

		/**
		 * Session ended event function.
		 * @see MFMedium::onSessionEnded().
		 */
		void onSessionEnded() override;

		/**
		 * Event function called for the scheduler, will be called on a regular basis.
		 */
		virtual void onScheduler();

	protected:

		/// Explicit state that the medium has stopped.
		bool hasStopped_ = false;

		/// Medium start position in 100 nanoseconds.
		LONGLONG startPosition_ = LONGLONG(-1);

		/// Medium speed.
		float speed_ = 1.0;

		/// Recent session ended event.
		bool eventSessionEnded_ = false;
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_FINITE_MEDIUM_H
