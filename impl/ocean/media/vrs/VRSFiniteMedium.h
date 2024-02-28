// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_FINITE_MEDIUM_H
#define META_OCEAN_MEDIA_VRS_FINITE_MEDIUM_H

#include "ocean/media/vrs/VRS.h"
#include "ocean/media/vrs/VRSMedium.h"

#include "ocean/media/FiniteMedium.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This class is the base class for all VRS finite mediums.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT VRSFiniteMedium :
	virtual public FiniteMedium,
	virtual public VRSMedium
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
		 * Sets the recent position of the finit medium.
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

	protected:

		/**
		 * Creates a new finite medium by a given url.
		 * @param url Url of the finite medium
		 */
		explicit VRSFiniteMedium(const std::string& url);

		/**
		 * Destructs a VRSFiniteMedium object.
		 */
		~VRSFiniteMedium() override;

	protected:

		/// The number of frames of this medium, with range [0, infinity)
		size_t numberFrames_ = 0;

		/// The timestamp of the first frame of this medium.
		Timestamp firstFrameTimestamp_;

		/// The timestamp of the last frame of this medium.
		Timestamp lastFrameTimestamp_;

		/// The duration of the entire sequence of this medium with default speed, in seconds, with range [0, infinity)
		double normalDuration_ = 0.0;

		/// The current position within the entire sequence of this medium, with range [0, normalDuration()]
		double position_ = 0.0;

		/// The new desired position within the entire sequence of this medium, with range [0, normalDuration()], -1 if there is no new desired position
		double newDesiredPosition_ = -1.0;
};

}

}

}

#endif // META_OCEAN_MEDIA_VRS_FINITE_MEDIUM_H
