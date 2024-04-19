// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/FloorTracker6DOF.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix4.h"

#include "metaonly/ocean/platform/meta/quest/vrapi/HeadsetPoses.h"

namespace Ocean
{

using namespace Platform::Meta::Quest::VrApi;

namespace Devices
{

namespace Quest
{

namespace VrApi
{

FloorTracker6DOF::FloorTracker6DOF() :
	Device(deviceNameFloorTracker6DOF(), deviceTypeFloorTracker6DOF()),
	VrApiDevice(deviceNameFloorTracker6DOF(), deviceTypeFloorTracker6DOF()),
	Measurement(deviceNameFloorTracker6DOF(), deviceTypeFloorTracker6DOF()),
	Tracker(deviceNameFloorTracker6DOF(), deviceTypeFloorTracker6DOF()),
	OrientationTracker3DOF(deviceNameFloorTracker6DOF()),
	PositionTracker3DOF(deviceNameFloorTracker6DOF()),
	Tracker6DOF(deviceNameFloorTracker6DOF())
{
	floorObjectId_ = addUniqueObjectId("Quest Floor");
}

FloorTracker6DOF::~FloorTracker6DOF()
{
	// nothing to do here
}

bool FloorTracker6DOF::isStarted() const
{
	return isStarted_;
}

bool FloorTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = true;
	return true;
}

bool FloorTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = false;
	return true;
}

bool FloorTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return floorIsTracked_;
}

void FloorTracker6DOF::update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType /*deviceType*/, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& /*trackedRemoteDevice*/, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(deviceLock);

	if (ovr == nullptr || !isStarted_)
	{
		if (floorIsTracked_)
		{
			postLostTrackerObjects({floorObjectId_}, timestamp);
		}

		floorIsTracked_ = false;
	}

	if (!isStarted_)
	{
		return;
	}

	const HomogenousMatrix4 world_T_floor = HeadsetPoses::world_T_floor(ovr);

	if (world_T_floor.isValid())
	{
		ObjectIds sampleObjectIds(1, floorObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, world_T_floor.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, world_T_floor.rotation());

		if (!floorIsTracked_)
		{
			postFoundTrackerObjects({floorObjectId_}, timestamp);
		}

		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_OBJECT_IN_DEVICE, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions))));
	}
	else if (floorIsTracked_)
	{
		postLostTrackerObjects({floorObjectId_}, timestamp);
	}

	floorIsTracked_ = world_T_floor.isValid();
}

}

}

}

}
