/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSOrientationTracker3DOF.h"

#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSOrientationTracker3DOF::IOSOrientationTracker3DOF() :
	Device(deviceNameIOSOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	IOSDevice(deviceNameIOSOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Measurement(deviceNameIOSOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Tracker(deviceNameIOSOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	OrientationTracker3DOF(deviceNameIOSOrientationTracker3DOF())
{
	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	trackerObjectId_ = addUniqueObjectId(deviceNameIOSOrientationTracker3DOF());
}

IOSOrientationTracker3DOF::~IOSOrientationTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSOrientationTracker3DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (sensorIsStarted)
	{
		return true;
	}

	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	deviceMotionListenerId_ = MotionManager::get().addListener(MotionManager::DeviceMotionCallback(*this, &IOSOrientationTracker3DOF::onDeviceMotion));

	sensorIsStarted = true;

	return sensorIsStarted;
}

bool IOSOrientationTracker3DOF::pause()
{
	return stop();
}

bool IOSOrientationTracker3DOF::stop()
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

void IOSOrientationTracker3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
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

	const CMQuaternion& cmQuaternion = deviceMotion.attitude.quaternion;

	Quaternion quaternion(cmQuaternion.w, cmQuaternion.x, cmQuaternion.y, cmQuaternion.z);

	if (quaternion.normalize())
	{
		if (firstSample)
		{
			postFoundTrackerObjects({trackerObjectId_}, timestamp);
		}

		Quaternions orientations(1, quaternion);

		postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(orientations))));
	}
}

}

}

}
