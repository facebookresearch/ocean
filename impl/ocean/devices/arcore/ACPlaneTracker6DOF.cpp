/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ACPlaneTracker6DOF.h"
#include "ocean/devices/arcore/ARSessionManager.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ACPlaneTracker6DOF::ACPlaneTracker6DOF() :
	Device(deviceNameACPlaneTracker6DOF(), deviceTypeACPlaneTracker6DOF()),
	Measurement(deviceNameACPlaneTracker6DOF(), deviceTypeACPlaneTracker6DOF()),
	Tracker(deviceNameACPlaneTracker6DOF(), deviceTypeACPlaneTracker6DOF()),
	VisualTracker(deviceNameACPlaneTracker6DOF(), deviceTypeACPlaneTracker6DOF()),
	ACDevice(TrackerCapabilities(TC_SLAM | TC_PLANE_DETECTION), deviceNameACPlaneTracker6DOF(), deviceTypeACPlaneTracker6DOF()),
	OrientationTracker3DOF(deviceNameACPlaneTracker6DOF()),
	PositionTracker3DOF(deviceNameACPlaneTracker6DOF()),
	Tracker6DOF(deviceNameACPlaneTracker6DOF()),
	SceneTracker6DOF(deviceNameACPlaneTracker6DOF())
{
	if (!ARSessionManager::get().isARCoreAvailable())
	{
		deviceIsValid = false;
		return;
	}

	worldObjectId_ = addUniqueObjectId("World");

	ocean_assert(deviceIsValid);
}

ACPlaneTracker6DOF::~ACPlaneTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	if (hasBeenRegistered_)
	{
		stop();
	}
}

void ACPlaneTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, SceneTracker6DOF::SceneElementPlanes::Planes&& planes, const Timestamp& timestamp)
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

		SharedSceneElements sceneElements;

		if (planes.empty())
		{
			sceneElements.emplace_back(nullptr); // a pure 6-DOF pose element
		}
		else
		{
			sceneElements.emplace_back(std::make_shared<SceneTracker6DOF::SceneElementPlanes>(std::move(planes)));
		}

		postFoundTrackerObjects(foundObjectIds, timestamp);

		postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(sceneElements))));
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
