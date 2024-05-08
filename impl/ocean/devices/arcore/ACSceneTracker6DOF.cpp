/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ACSceneTracker6DOF.h"
#include "ocean/devices/arcore/ARSessionManager.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ACSceneTracker6DOF::ACSceneTracker6DOF() :
	Device(deviceNameACSceneTracker6DOF(), deviceTypeACSceneTracker6DOF()),
	Measurement(deviceNameACSceneTracker6DOF(), deviceTypeACSceneTracker6DOF()),
	Tracker(deviceNameACSceneTracker6DOF(), deviceTypeACSceneTracker6DOF()),
	VisualTracker(deviceNameACSceneTracker6DOF(), deviceTypeACSceneTracker6DOF()),
	ACDevice(TC_SLAM, deviceNameACSceneTracker6DOF(), deviceTypeACSceneTracker6DOF()),
	OrientationTracker3DOF(deviceNameACSceneTracker6DOF()),
	PositionTracker3DOF(deviceNameACSceneTracker6DOF()),
	Tracker6DOF(deviceNameACSceneTracker6DOF()),
	SceneTracker6DOF(deviceNameACSceneTracker6DOF())
{
	if (!ARSessionManager::get().isARCoreAvailable())
	{
		deviceIsValid = false;
		return;
	}

	worldObjectId_ = addUniqueObjectId("World");

	ocean_assert(deviceIsValid);
}

ACSceneTracker6DOF::~ACSceneTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	if (hasBeenRegistered_)
	{
		stop();
	}
}

void ACSceneTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, Vectors3&& objectPoints, Indices64&& objectPointIds, const Timestamp& timestamp)
{
	ocean_assert(objectPoints.size() == objectPointIds.size());

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

		if (objectPoints.empty())
		{
			sceneElements.emplace_back(nullptr); // a pure 6-DOF pose element
		}
		else
		{
			sceneElements.emplace_back(std::make_shared<SceneElementObjectPoints>(std::move(objectPoints), std::move(objectPointIds)));
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
