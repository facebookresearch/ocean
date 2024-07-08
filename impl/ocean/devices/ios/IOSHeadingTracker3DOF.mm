/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSHeadingTracker3DOF.h"

#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSHeadingTracker3DOF::IOSHeadingTracker3DOF() :
	Device(deviceNameIOSHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	IOSDevice(deviceNameIOSHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Measurement(deviceNameIOSHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Tracker(deviceNameIOSHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	OrientationTracker3DOF(deviceNameIOSHeadingTracker3DOF())
{
	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	ocean_assert([CMMotionManager availableAttitudeReferenceFrames] & CMAttitudeReferenceFrameXMagneticNorthZVertical);

	trackerObjectId_ = addUniqueObjectId(deviceNameIOSHeadingTracker3DOF());
}

IOSHeadingTracker3DOF::~IOSHeadingTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSHeadingTracker3DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (sensorIsStarted)
	{
		return true;
	}

	ocean_assert(deviceType.minorType() == Tracker::TRACKER_ORIENTATION_3DOF);

	deviceMotionListenerId_ = MotionManager::get().addListener(MotionManager::DeviceMotionCallback(*this, &IOSHeadingTracker3DOF::onDeviceMotion));

	sensorIsStarted = true;

	return sensorIsStarted;
}

bool IOSHeadingTracker3DOF::pause()
{
	return stop();
}

bool IOSHeadingTracker3DOF::stop()
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

void IOSHeadingTracker3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
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

	Quaternion object_Q_device(cmQuaternion.w, cmQuaternion.x, cmQuaternion.y, cmQuaternion.z);

	if (object_Q_device.normalize())
	{
		if (firstSample)
		{
			postFoundTrackerObjects({trackerObjectId_}, timestamp);
		}

		// attitude's frame is CMAttitudeReferenceFrameXTrueNorthZVertical

		const Quaternion zSouthySky_Q_xNorthzSky = Quaternion(Vector3(0, 1, 0), Numeric::pi_2()) * Quaternion(Vector3(1, 0, 0), -Numeric::pi_2());

		Quaternions orientations(1, zSouthySky_Q_xNorthzSky * object_Q_device);

		postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(orientations))));
	}
}

}

}

}
