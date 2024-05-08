/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidGravityTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidGravityTracker3DOF::AndroidGravityTracker3DOF(const ASensor* sensor) :
	Device(deviceNameAndroidGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	AndroidDevice(deviceNameAndroidGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Measurement(deviceNameAndroidGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Sensor(deviceNameAndroidGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	AndroidSensor(deviceNameAndroidGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Tracker(deviceNameAndroidGravityTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	OrientationTracker3DOF(deviceNameAndroidGravityTracker3DOF())
{
	ocean_assert(sensor != nullptr);

	ocean_assert(sensor_ == nullptr);
	sensor_ = sensor;

	ocean_assert(sensorManager_ != nullptr);

	if (!registerForEventFunction(sensorManager_))
	{
		deviceIsValid = false;
		return;
	}
}

AndroidGravityTracker3DOF::~AndroidGravityTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();

	ocean_assert(sensorManager_ && eventQueue_);
	ASensorManager_destroyEventQueue(sensorManager_, eventQueue_);
}

int AndroidGravityTracker3DOF::onEventFunction()
{
	TemporaryScopedLock scopedLock(deviceLock);

	bool firstSample = false;
	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		ocean_assert(sensorEvent.type == AST_GRAVITY);

		if (firstUnixEventTimestamp_.isInvalid())
		{
			// pairing both timestamp may not be ideal but it seems to be the best solution as the Android timestamp seem to be arbitrary for individual sensors
			// **NOTE** perhaps the timestamp of the Android event may restart/change after waking up - this may result in wrong timestamps

			firstUnixEventTimestamp_.toNow();
			firstAndroidEventTimestamp_ = sensorEvent.timestamp;

			firstSample = true;
		}

		const Timestamp timestamp(firstUnixEventTimestamp_ + double(sensorEvent.timestamp - firstAndroidEventTimestamp_) / 1000000000.0);

		scopedLock.release();

		// We need to negate the gravity direction as Android provides gravity vector ~(0, 9.8, 0) if device is in default position
		Vector3 gravity(-sensorEvent.acceleration.x, -sensorEvent.acceleration.y, -sensorEvent.acceleration.z);

		if (gravity.normalize())
		{
			if (firstSample)
			{
				postFoundTrackerObjects({sensorObjectId_}, timestamp);
				firstSample = false;
			}

			const Quaternion device_Q_gravity(Vector3(0, -1, 0), gravity); // negative y-axis is uses as default gravity vector

			ObjectIds objectIds(1, sensorObjectId_);
			Quaternions orientations(1, Quaternion(device_Q_gravity));

			postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, RS_OBJECT_IN_DEVICE, std::move(objectIds), std::move(orientations))));
		}

		scopedLock.relock(deviceLock);
	}

	return 1;
}

}

}

}
