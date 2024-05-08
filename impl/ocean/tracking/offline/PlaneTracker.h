/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_PLANE_TRACKER_H
#define META_OCEAN_TRACKING_OFFLINE_PLANE_TRACKER_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/OfflineTracker.h"
#include "ocean/tracking/offline/TrackerEvent.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class PlaneTracker;

/**
 * Definition of a smart object reference holding a PlaneTracker object.
 * @see PlaneTracker.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<PlaneTracker, OfflineTracker> PlaneTrackerRef;

/**
 * This class implements the abstract base class for all plane trackers.
 * @see PlaneTrackerRef.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT PlaneTracker : virtual public OfflineTracker
{
	public:

		/**
		 * Returns the plane of this tracker.
		 * @return Plane that is tracked
		 */
		virtual Plane3 plane() const;

		/**
		 * Determines the transformation that transforms points defined in the local coordinate of the tracked object into points defined in the world coordinate system.
		 * @param transformation Resulting transformation
		 * @return True, if succeeded
		 */
		virtual bool objectTransformation(HomogenousMatrix4& transformation) const;

	protected:

		/**
		 * Creates a new plane tracker object.
		 */
		PlaneTracker() = default;

		/**
		 * Creates a new plane tracker object.
		 * @param plane The plane of this tracker
		 */
		inline PlaneTracker(const Plane3& plane);

		/**
		 * Updates the plane of this tracker and invokes the corresponding state event(s).
		 * @param plane New plane for this tracker
		 */
		virtual void updatePlane(const Plane3& plane);

	protected:

		/// The plane of this tracker.
		Plane3 plane_;

		/// The tracker object transformation the tracker coordinate system into the world coordinate system.
		HomogenousMatrix4 objectTransformation_ = HomogenousMatrix4(false);
};

PlaneTracker::PlaneTracker(const Plane3& plane) :
	OfflineTracker(),
	plane_(plane),
	objectTransformation_(false)
{
	ocean_assert(plane_.isValid());
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_PLANE_TRACKER_H
