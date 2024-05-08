/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SMOOTH_TRANSFORMATION_H
#define META_OCEAN_TRACKING_SMOOTH_TRANSFORMATION_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a smoother for a 12-DOF transformations (3D translation, 3D rotation, 3D scale, and 3D shear).
 * The transformation can be updated as often as necessary, while an internal smoothing interval is used to create a smooth transition between the current transformation and the given transformation.
 * <pre>
 *                                                                                  new transformation
 *           old transformation                 new transformation                 + smoothing interval
 *                  |                                   |                                  |
 *                  V                                   V                                  V
 * timeline: ------------------------------------------------------------------------------------------------------------
 *                 the old transformation               |  the interpolated transformation |  the new transformation
 *                     will be reported                 |         will be reported         |     will be reported
 * </pre>
 *
 * The class is thread-safe.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT SmoothedTransformation
{
	public:

		/**
		 * Creates a new transformation object.
		 * @param smoothingInterval The time interval in which the old transformation will be merged/inerpolated into the new transformation, in seconds, with range (0, infinity)
		 */
		explicit SmoothedTransformation(const double smoothingInterval = 1.0);

		/**
		 * Sets or updates a new transformation.
		 * @param transformation The new (e.g., the latest) 12-DOF transformation to be set, must be valid
		 * @param timestamp The timestamp of the given transformation, must be valid, with range [timestamp(), infinity)
		 */
		void setTransformation(const HomogenousMatrix4& transformation, const Timestamp& timestamp);

		/**
		 * Sets a new smoothing interval.
		 * @param smoothingInterval The time interval in which the old transformation will be merged/inerpolated into the new transformation, in seconds, with range (0, infinity)
		 */
		inline void setSmoothingInterval(const double smoothingInterval);

		/**
		 * Returns the smoothed 12-DOF transformation of this object.
		 * @param timestamp The timestamp for which the smoothed transformation is requested, with range (-infinity, infinity), must be valid
		 * @return The smoothed 12-DOF transformation, invalid if no transformation was set
		 */
		HomogenousMatrix4 transformation(const Timestamp& timestamp) const;

		/**
		 * Returns the timestamp of the latest transformation.
		 * @param transformation Optional resulting transformation associated with the latest transformation, invalid if the resulting timestamp is invalid
		 * @return The latest timestamp, may be invalid
		 */
		inline Timestamp timestamp(HomogenousMatrix4* transformation = nullptr) const;

		/**
		 * Resets this transformation object and removes all previously set transformations.
		 * The smoothing interval is untouched.
		 */
		void reset();

		/**
		 * Returns whether this object holds a valid transformation.
		 * @return True, if so
		 */
		inline bool isValid() const;

	private:

		/// The time interval in which the old transformation will be merged/inerpolated into the new transformation, in seconds, with range (0, infinity).
		double smoothingInterval_;

		/// The old 12-DOF transformation connected with the old timestamp.
		HomogenousMatrix4 oldTransformation_;

		/// The new 12-DOF transformation connected with the new timestamp.
		HomogenousMatrix4 newTransformation_;

		/// The timestamp of the old transformation.
		Timestamp oldTimestamp_;

		/// The timestamp of the new transformation.
		Timestamp newTimestamp_;

		/// The lock of this transformation object.
		mutable Lock lock_;
};

inline void SmoothedTransformation::setSmoothingInterval(const double smoothingInterval)
{
	ocean_assert(smoothingInterval > NumericD::eps());

	const ScopedLock scopedLock(lock_);

	smoothingInterval_ = smoothingInterval;
}

inline Timestamp SmoothedTransformation::timestamp(HomogenousMatrix4* transformation) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(oldTransformation_.isValid() == oldTimestamp_.isValid());
	ocean_assert(newTransformation_.isValid() == newTimestamp_.isValid());

	if (newTimestamp_.isValid())
	{
		if (transformation != nullptr)
		{
			*transformation = newTransformation_;
		}

		return newTimestamp_;
	}

	if (transformation != nullptr)
	{
		*transformation = oldTransformation_;
	}

	return oldTimestamp_;
}

inline bool SmoothedTransformation::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return oldTransformation_.isValid();
}

}

}

#endif // META_OCEAN_TRACKING_SMOOTH_TRANSFORMATION_H
