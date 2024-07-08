/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSGravityTracker3DOF.h"

#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSGravityTracker3DOF::IOSGravityTracker3DOF() :
	Device(deviceNameIOSGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	IOSDevice(deviceNameIOSGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Measurement(deviceNameIOSGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Tracker(deviceNameIOSGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	OrientationTracker3DOF(deviceNameIOSGravityTracker3DOF())
{
	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	trackerObjectId_ = addUniqueObjectId(deviceNameIOSGravityTracker3DOF());
}

IOSGravityTracker3DOF::~IOSGravityTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSGravityTracker3DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (sensorIsStarted)
	{
		return true;
	}

	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	deviceMotionListenerId_ = MotionManager::get().addListener(MotionManager::DeviceMotionCallback(*this, &IOSGravityTracker3DOF::onDeviceMotion));

	sensorIsStarted = true;

	return sensorIsStarted;
}

bool IOSGravityTracker3DOF::pause()
{
	return stop();
}

bool IOSGravityTracker3DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!sensorIsStarted)
	{
		return true;
	}

	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	MotionManager::get().removeListener(deviceMotionListenerId_);

	sensorIsStarted = false;
	return true;
}

void IOSGravityTracker3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
{
	ocean_assert(deviceMotion != nullptr);

	bool firstSample = false;

	if (sensorFirstUnixEventTimestamp.isInvalid())
	{
		sensorFirstUnixEventTimestamp.toNow();
		sensorFirstIOSEventTimestamp = [[NSProcessInfo processInfo] systemUptime];

		firstSample = true;
	}

	const Timestamp timestamp(sensorFirstUnixEventTimestamp + deviceMotion.timestamp - sensorFirstIOSEventTimestamp);

	ObjectIds objectIds(1, trackerObjectId_);

	VectorD3 gravity(deviceMotion.gravity.x, deviceMotion.gravity.y, deviceMotion.gravity.z);

	if (gravity.normalize())
	{
		if (firstSample)
		{
			postFoundTrackerObjects({trackerObjectId_}, timestamp);
		}

		const QuaternionD device_Q_gravity(VectorD3(0.0, -1.0, 0.0), gravity); // negative y-axis is uses as default gravity vector

		Quaternions orientations(1, Quaternion(device_Q_gravity));

		postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, RS_OBJECT_IN_DEVICE, std::move(objectIds), std::move(orientations))));
	}
}

}

}

}
