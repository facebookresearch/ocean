/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ACDepthTracker6DOF.h"
#include "ocean/devices/arcore/ARSessionManager.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ACDepthTracker6DOF::ACDepthTracker6DOF() :
	Device(deviceNameACDepthTracker6DOF(), deviceTypeACDepthTracker6DOF()),
	Measurement(deviceNameACDepthTracker6DOF(), deviceTypeACDepthTracker6DOF()),
	Tracker(deviceNameACDepthTracker6DOF(), deviceTypeACDepthTracker6DOF()),
	VisualTracker(deviceNameACDepthTracker6DOF(), deviceTypeACDepthTracker6DOF()),
	ACDevice(TrackerCapabilities(TC_SLAM | TC_DEPTH), deviceNameACDepthTracker6DOF(), deviceTypeACDepthTracker6DOF()),
	OrientationTracker3DOF(deviceNameACDepthTracker6DOF()),
	PositionTracker3DOF(deviceNameACDepthTracker6DOF()),
	Tracker6DOF(deviceNameACDepthTracker6DOF()),
	SceneTracker6DOF(deviceNameACDepthTracker6DOF())
{
	if (!ARSessionManager::get().isARCoreAvailable())
	{
		deviceIsValid = false;
		return;
	}

	worldObjectId_ = addUniqueObjectId("World");

	ocean_assert(deviceIsValid);
}

ACDepthTracker6DOF::~ACDepthTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	if (hasBeenRegistered_)
	{
		stop();
	}
}

void ACDepthTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, Frame&& depth, SharedAnyCamera&& depthCamera, const HomogenousMatrix4& device_T_depth, const Timestamp& timestamp)
{
	ocean_assert(world_T_camera.isValid());
	ocean_assert(depth && depthCamera);
	ocean_assert(depth.width() == depthCamera->width() && depth.height() == depthCamera->height());
	ocean_assert(device_T_depth.isValid());
	ocean_assert(timestamp.isValid());

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

		SharedSceneElements sceneElements =
		{
			std::make_shared<SceneElementDepth>(std::move(depthCamera), device_T_depth, std::make_shared<Frame>(std::move(depth)))
		};

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
