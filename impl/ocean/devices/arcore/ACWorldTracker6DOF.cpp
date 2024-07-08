/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ACWorldTracker6DOF.h"
#include "ocean/devices/arcore/ARSessionManager.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ACWorldTracker6DOF::ACWorldTracker6DOF() :
	Device(deviceNameACWorldTracker6DOF(), deviceTypeACWorldTracker6DOF()),
	Measurement(deviceNameACWorldTracker6DOF(), deviceTypeACWorldTracker6DOF()),
	Tracker(deviceNameACWorldTracker6DOF(), deviceTypeACWorldTracker6DOF()),
	VisualTracker(deviceNameACWorldTracker6DOF(), deviceTypeACWorldTracker6DOF()),
	ACDevice(TC_SLAM, deviceNameACWorldTracker6DOF(), deviceTypeACWorldTracker6DOF()),
	OrientationTracker3DOF(deviceNameACWorldTracker6DOF()),
	PositionTracker3DOF(deviceNameACWorldTracker6DOF()),
	Tracker6DOF(deviceNameACWorldTracker6DOF())
{
	if (!ARSessionManager::get().isARCoreAvailable())
	{
		deviceIsValid = false;
		return;
	}

	worldObjectId_ = addUniqueObjectId("World");

	ocean_assert(deviceIsValid);
}

ACWorldTracker6DOF::~ACWorldTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	if (hasBeenRegistered_)
	{
		stop();
	}
}

void ACWorldTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp)
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (world_T_camera.isValid())
	{
		ocean_assert(worldObjectId_ != invalidObjectId());

		ObjectIdSet foundObjectIds;

		ObjectIds sampleObjectIds(1, worldObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, world_T_camera.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, world_T_camera.rotation());

		if (!worldIsTracked_)
		{
			foundObjectIds = {worldObjectId_};
			worldIsTracked_ = true;
		}

		scopedLock.release();

		postFoundTrackerObjects(foundObjectIds, timestamp);

		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions))));
	}
	else
	{
		ObjectIdSet lostObjectIds;

		if (worldIsTracked_)
		{
			lostObjectIds = {worldObjectId_};
			worldIsTracked_ = false;
		}

		scopedLock.release();

		postLostTrackerObjects(lostObjectIds, timestamp);
	}
}

}

}

}
