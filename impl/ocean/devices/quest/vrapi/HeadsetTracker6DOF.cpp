// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/HeadsetTracker6DOF.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix4.h"

#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"

namespace Ocean
{

using namespace Platform::Meta::Quest::VrApi;

namespace Devices
{

namespace Quest
{

namespace VrApi
{

HeadsetTracker6DOF::HeadsetTracker6DOF() :
	Device(deviceNameHeadsetTracker6DOF(), deviceTypeHeadsetTracker6DOF()),
	VrApiDevice(deviceNameHeadsetTracker6DOF(), deviceTypeHeadsetTracker6DOF()),
	Measurement(deviceNameHeadsetTracker6DOF(), deviceTypeHeadsetTracker6DOF()),
	Tracker(deviceNameHeadsetTracker6DOF(), deviceTypeHeadsetTracker6DOF()),
	OrientationTracker3DOF(deviceNameHeadsetTracker6DOF()),
	PositionTracker3DOF(deviceNameHeadsetTracker6DOF()),
	Tracker6DOF(deviceNameHeadsetTracker6DOF())
{
	deviceObjectId_ = addUniqueObjectId("Device");

    setSampleCapacity(120); // 120 samples ~1-2 seconds
}

HeadsetTracker6DOF::~HeadsetTracker6DOF()
{
	// nothing to do here
}

bool HeadsetTracker6DOF::isStarted() const
{
	return isStarted_;
}

bool HeadsetTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = true;
	return true;
}

bool HeadsetTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = false;
	return true;
}

bool HeadsetTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return deviceIsTracked_;
}

void HeadsetTracker6DOF::update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType /*deviceType*/, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& /*trackedRemoteDevice*/, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(deviceLock);

	if (ovr == nullptr || !isStarted_)
	{
		if (deviceIsTracked_)
		{
			postLostTrackerObjects({deviceObjectId_}, timestamp);
		}

		deviceIsTracked_ = false;
	}

	if (!isStarted_)
	{
		return;
	}

	const HomogenousMatrix4 world_T_device = HeadsetPoses::world_T_device(ovr, timestamp);

	if (world_T_device.isValid())
	{
		ObjectIds sampleObjectIds(1, deviceObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, world_T_device.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, world_T_device.rotation());

		if (!deviceIsTracked_)
		{
			postFoundTrackerObjects({deviceObjectId_}, timestamp);
		}

		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_OBJECT_IN_DEVICE, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions))));
	}
	else if (deviceIsTracked_)
	{
		postLostTrackerObjects({deviceObjectId_}, timestamp);
	}

	deviceIsTracked_ = world_T_device.isValid();
}

}

}

}

}
