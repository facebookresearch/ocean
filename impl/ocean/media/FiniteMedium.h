// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_FINITE_MEDIUM_H
#define META_OCEAN_MEDIA_FINITE_MEDIUM_H

#include "ocean/media/Media.h"
#include "ocean/media/Medium.h"
#include "ocean/media/MediumRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class FiniteMedium;

/**
 * Definition of a smart medium reference holding a finite medium object.
 * @see SmartMediumRef, FiniteMedium.
 * @ingroup media
 */
typedef SmartMediumRef<FiniteMedium> FiniteMediumRef;

/**
 * This class is the base class for all finite mediums.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT FiniteMedium : virtual public Medium
{
	public:

		/**
		 * Returns the duration of the finite medium.
		 * @return Duration in seconds
		 */
		virtual double duration() const = 0;

		/**
		 * Returns the duration without speed consideration.
		 * @return Duration in seconds
		 */
		virtual double normalDuration() const = 0;

		/**
		 * Returns the recent position of the finite medium.
		 * @return Recent position in seconds
		 */
		virtual double position() const = 0;

		/**
		 * Returns the speed of the finite medium.
		 * A value of 1.0 means normal speed, 2.0 means double speed.
		 * @return Speed as a factor
		 */
		virtual float speed() const = 0;

		/**
		 * Returns whether the medium is played in a loop.
		 * By default a finite medium will not be looped.
		 * @return True, if so
		 * @see setLoop().
		 */
		inline bool loop() const;

		/**
		 * Sets the recent position of the finit medium.
		 * @param position New position in seconds
		 * @return True, if succeeded
		 */
		virtual bool setPosition(const double position);

		/**
		 * Sets the speed of the finite medium.
		 * A value of 1.0 means normal speed, 2.0 means double speed.
		 * @param speed Speed as a factor
		 * @return True, if the speed is accepted
		 */
		virtual bool setSpeed(const float speed);

		/**
		 * Sets whether the medium has to be played in a loop.
		 * @param value True, if looping
		 * @return True, if succeeded
		 * @see loop().
		 */
		virtual bool setLoop(const bool value);

		/**
		 * Returns whether a previous task has been finished.
		 * A task has been finished if all data that has been requested before has been provided (or e.g. rendered).
		 * @return True, if so
		 */
		virtual bool taskFinished() const;

	protected:

		/**
		 * Creates a new finite medium by a given url.
		 * @param url Url of the finite medium
		 */
		explicit FiniteMedium(const std::string& url);

	protected:

		/// Determines whether the medium is played in a loop.
		bool loop_ = false;
};

inline bool FiniteMedium::loop() const
{
	const ScopedLock scopedLock(lock_);

	return loop_;
}

}

}

#endif // META_OCEAN_MEDIA_FINITE_MEDIUM_H
