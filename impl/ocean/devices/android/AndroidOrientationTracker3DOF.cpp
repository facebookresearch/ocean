/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidOrientationTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidOrientationTracker3DOF::AndroidOrientationTracker3DOF(const ASensor* sensor) :
	Device(deviceNameAndroidOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	AndroidDevice(deviceNameAndroidOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Measurement(deviceNameAndroidOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Sensor(deviceNameAndroidOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	AndroidSensor(deviceNameAndroidOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	Tracker(deviceNameAndroidOrientationTracker3DOF(), deviceTypeOrientationTracker3DOF()),
	OrientationTracker3DOF(deviceNameAndroidOrientationTracker3DOF())
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

AndroidOrientationTracker3DOF::~AndroidOrientationTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();

	ocean_assert(sensorManager_ && eventQueue_);
	ASensorManager_destroyEventQueue(sensorManager_, eventQueue_);
}

int AndroidOrientationTracker3DOF::onEventFunction()
{
	TemporaryScopedLock scopedLock(deviceLock);

	bool firstSample = false;
	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		ocean_assert(sensorEvent.type == AST_ROTATION_VECTOR || sensorEvent.type == AST_GAME_ROTATION_VECTOR);

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

		Quaternion quaternion = Quaternion(Scalar(w), Scalar(x), Scalar(y), Scalar(z));

		if (quaternion.normalize())
		{
			if (firstSample)
			{
				postFoundTrackerObjects({sensorObjectId_}, timestamp);
				firstSample = false;
			}

			const ObjectIds objectIds(1, sensorObjectId_);
			const Quaternions quaternions(1, quaternion);

			postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, RS_DEVICE_IN_OBJECT, objectIds, quaternions)));
		}

		scopedLock.relock(deviceLock);
	}

	return 1;
}

}

}

}
