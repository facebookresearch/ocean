/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKWorldTracker6DOF.h"

#include "ocean/media/avfoundation/AVFoundation.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKWorldTracker6DOF::AKWorldTracker6DOF() :
	Device(deviceNameAKWorldTracker6DOF(), deviceTypeAKWorldTracker6DOF()),
	AKDevice(TC_SLAM, deviceNameAKWorldTracker6DOF(), deviceTypeAKWorldTracker6DOF()),
	Measurement(deviceNameAKWorldTracker6DOF(), deviceTypeAKWorldTracker6DOF()),
	Tracker(deviceNameAKWorldTracker6DOF(), deviceTypeAKWorldTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKWorldTracker6DOF()),
	PositionTracker3DOF(deviceNameAKWorldTracker6DOF()),
	Tracker6DOF(deviceNameAKWorldTracker6DOF()),
	VisualTracker(deviceNameAKWorldTracker6DOF(), deviceTypeAKWorldTracker6DOF())
{
	worldObjectId_ = addUniqueObjectId("World");
}

AKWorldTracker6DOF::~AKWorldTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKWorldTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	if (frameMediums_.size() == 1 && frameMediums_.front())
	{
		isStarted_ = ARSessionManager::get().start(this, frameMediums_.front());
	}
	else
	{
		Log::error() << "AKWorldTracker6DOF needs a valid input medium before it can be started";
	}

	return isStarted_;
}

bool AKWorldTracker6DOF::pause()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().pause(this))
	{
		return false;
	}

	ObjectIdSet lostObjects;

	if (worldIsTracked_)
	{
		lostObjects.emplace(worldObjectId_);
		worldIsTracked_ = false;
	}

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKWorldTracker6DOF::stop()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	if (!ARSessionManager::get().stop(this))
	{
		return false;
	}

	if (!worldIsTracked_)
	{
		return true;
	}

	ObjectIdSet lostObjects = {worldObjectId_};
	worldIsTracked_ = false;

	isStarted_ = false;

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKWorldTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == worldObjectId_);

	return worldIsTracked_ && objectId == worldObjectId_;
}

void AKWorldTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() == 1 && frameMediums.front())
	{
		const Media::FrameMediumRef& frameMedium = frameMediums.front();

		if (frameMedium->library() == Media::AVFoundation::nameAVFLibrary() && (frameMedium->type() & Media::Medium::LIVE_VIDEO) == Media::Medium::LIVE_VIDEO)
		{
			if (frameMedium->url() == "Back Camera" || frameMedium->url() == "Front Camera")
			{
				VisualTracker::setInput(std::move(frameMediums));
				return;
			}
		}
	}

	ocean_assert(false && "Invalid frameMedium!");
}

void AKWorldTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame)
{
	ocean_assert(arFrame != nullptr);

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

		Metadata metadata;
		if (arFrame.lightEstimate != nullptr)
		{
			metadata["ambientColorTemperature"] = Value(arFrame.lightEstimate.ambientColorTemperature);
			metadata["ambientIntensity"] = Value(arFrame.lightEstimate.ambientIntensity);
		}

		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(metadata))));
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
