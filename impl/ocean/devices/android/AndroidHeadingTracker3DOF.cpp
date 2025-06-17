/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidHeadingTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidHeadingTracker3DOF::AndroidHeadingTracker3DOF(const ASensor* sensor) :
	Device(deviceNameAndroidHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	AndroidDevice(deviceNameAndroidHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Measurement(deviceNameAndroidHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Sensor(deviceNameAndroidHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	AndroidSensor(deviceNameAndroidHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Tracker(deviceNameAndroidHeadingTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	OrientationTracker3DOF(deviceNameAndroidHeadingTracker3DOF())
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

AndroidHeadingTracker3DOF::~AndroidHeadingTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();

	ocean_assert(sensorManager_ && eventQueue_);
	ASensorManager_destroyEventQueue(sensorManager_, eventQueue_);
}

int AndroidHeadingTracker3DOF::onEventFunction()
{
	TemporaryScopedLock scopedLock(deviceLock);

	bool firstSample = false;
	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		ocean_assert(sensorEvent.type == AST_ROTATION_VECTOR);

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

		const float x = sensorEvent.data[0];
		const float y = sensorEvent.data[1];
		const float z = sensorEvent.data[2];
		const float w = sensorEvent.data[3];

		Quaternion object_Q_device = Quaternion(Scalar(w), Scalar(x), Scalar(y), Scalar(z));

		if (object_Q_device.normalize())
		{
			if (firstSample)
			{
				postFoundTrackerObjects({sensorObjectId_}, timestamp);
				firstSample = false;
			}

			// X is defined as the vector product Y x Z. It is tangential to the ground at the device's current location and points approximately East.
			// Y is tangential to the ground at the device's current location and points toward the geomagnetic North Pole.
			// Z points toward the sky and is perpendicular to the ground plane.

			const Quaternion zSouthySky_Q_yNorthzSky = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2());

			const ObjectIds objectIds(1, sensorObjectId_);
			const Quaternions quaternions(1, zSouthySky_Q_yNorthzSky * object_Q_device);

			postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, RS_DEVICE_IN_OBJECT, objectIds, quaternions)));
		}

		scopedLock.relock(deviceLock);
	}

	return 1;
}

}

}

}
