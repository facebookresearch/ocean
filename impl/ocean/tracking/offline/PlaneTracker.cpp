// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/offline/PlaneTracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

Plane3 PlaneTracker::plane() const
{
	const ScopedLock scopedLock(lock_);

	return plane_;
}

bool PlaneTracker::objectTransformation(HomogenousMatrix4& transformation) const
{
	const ScopedLock scopedLock(lock_);

	if (objectTransformation_.isValid())
	{
		transformation = objectTransformation_;
		return true;
	}

	return false;
}

void PlaneTracker::updatePlane(const Plane3& plane)
{
	const ScopedLock scopedLock(lock_);

	plane_ = plane;
	eventCallbacks_(TrackerPlaneStateEvent(id_, plane_));
}

}

}

}
