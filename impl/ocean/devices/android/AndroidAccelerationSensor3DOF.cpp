/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidAccelerationSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidAccelerationSensor3DOF::AndroidAccelerationSensor3DOF(const SensorType sensorType, const ASensor* sensor) :
	Device(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
	AndroidDevice(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
	Measurement(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
	Sensor(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
	AndroidSensor(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
	AccelerationSensor3DOF(deviceNameAndroidAccelerationSensor3DOF(sensorType), sensorType)
{
	ocean_assert(sensor != nullptr);

	ocean_assert(sensor_ == nullptr);
	sensor_ = sensor;

	if (!registerForEventFunction(sensorManager_))
	{
		deviceIsValid = false;
		return;
	}
}

AndroidAccelerationSensor3DOF::~AndroidAccelerationSensor3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();

	ocean_assert(sensorManager_ && eventQueue_);
	ASensorManager_destroyEventQueue(sensorManager_, eventQueue_);
}

int AndroidAccelerationSensor3DOF::onEventFunction()
{
	const ScopedLock scopedLock(deviceLock);

	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		ocean_assert(sensorEvent.type == AST_ACCELEROMETER || sensorEvent.type == AST_LINEAR_ACCELERATION);

		if (firstUnixEventTimestamp_.isInvalid())
		{
			// pairing both timestamp may not be ideal but it seems to be the best solution as the Android timestamp seem to be arbitrary for individual sensors
			// **NOTE** perhaps the timestamp of the Android event may restart/change after waking up - this may result in wrong timestamps

			firstUnixEventTimestamp_.toNow();
			firstAndroidEventTimestamp_ = sensorEvent.timestamp;
		}

		const Timestamp timestamp(firstUnixEventTimestamp_ + double(sensorEvent.timestamp - firstAndroidEventTimestamp_) / 1000000000.0);

		const ASensorVector& acceleration = sensorEvent.acceleration;

		const ObjectIds objectIds(1, sensorObjectId_);
		const Acceleration3DOFSample::Measurements measurements(1, Vector3(Scalar(acceleration.x), Scalar(acceleration.y), Scalar(acceleration.z)));

		postNewSample(SampleRef(new Acceleration3DOFSample(timestamp, objectIds, measurements)));
	}

	return 1;
}

}

}

}
