// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/ControllerTracker6DOF.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

using namespace Platform::Meta::Quest::VrApi;

namespace Devices
{

namespace Quest
{

namespace VrApi
{

ControllerTracker6DOF::ControllerTracker6DOF() :
	Device(deviceNameControllerTracker6DOF(), deviceTypeControllerTracker6DOF()),
	VrApiDevice(deviceNameControllerTracker6DOF(), deviceTypeControllerTracker6DOF()),
	Measurement(deviceNameControllerTracker6DOF(), deviceTypeControllerTracker6DOF()),
	Tracker(deviceNameControllerTracker6DOF(), deviceTypeControllerTracker6DOF()),
	OrientationTracker3DOF(deviceNameControllerTracker6DOF()),
	PositionTracker3DOF(deviceNameControllerTracker6DOF()),
	Tracker6DOF(deviceNameControllerTracker6DOF())
{
	objectIdLeft_ = addUniqueObjectId("Quest Controller Left");
	objectIdRight_ = addUniqueObjectId("Quest Controller Right");
}

ControllerTracker6DOF::~ControllerTracker6DOF()
{
	// nothing to do here
}

bool ControllerTracker6DOF::isStarted() const
{
	return isStarted_;
}

bool ControllerTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = true;
	return true;
}

bool ControllerTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = false;
	return true;
}

bool ControllerTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return trackedIds_.find(objectId) != trackedIds_.cend();
}

void ControllerTracker6DOF::update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(deviceLock);

	if (ovr == nullptr)
	{
		postLostTrackerObjects(trackedIds_, timestamp);
		return;
	}

	if (!isStarted_)
	{
		postLostTrackerObjects(trackedIds_, timestamp);
		trackedIds_.clear();

		return;
	}

	ObjectIdSet currentIds;

	ObjectIds sampleObjectIds;
	Tracker6DOFSample::Positions samplePositions;
	Tracker6DOFSample::Orientations sampleOrientations;

	constexpr TrackedRemoteDevice::RemoteType remoteTypes[2] = {TrackedRemoteDevice::RT_LEFT, TrackedRemoteDevice::RT_RIGHT};
	const ObjectId objectIds[2] = {objectIdLeft_, objectIdRight_};

	for (unsigned int deviceIndex = 0u; deviceIndex < 2u; ++deviceIndex)
	{
		const TrackedRemoteDevice::RemoteType remoteType = remoteTypes[deviceIndex];
		const ObjectId objectId = objectIds[deviceIndex];

		HomogenousMatrix4 world_T_removeDevice;
		if (trackedRemoteDevice.pose(remoteType, &world_T_removeDevice))
		{
			sampleObjectIds.emplace_back(objectId);
			samplePositions.emplace_back(world_T_removeDevice.translation());
			sampleOrientations.emplace_back(world_T_removeDevice.rotation());

			currentIds.emplace(objectId);
		}
	}

	postFoundTrackerObjects(determineFoundObjects(trackedIds_, currentIds), timestamp);

	// even empty samples are posted because of the internal sample container
	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions))));

	postLostTrackerObjects(determineLostObjects(trackedIds_, currentIds), timestamp);

	trackedIds_ = currentIds;
}

}

}

}

}
