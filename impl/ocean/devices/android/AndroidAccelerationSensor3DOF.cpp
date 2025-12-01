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
	AndroidEventDevice(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
	Sensor(deviceNameAndroidAccelerationSensor3DOF(sensorType), deviceTypeAndroidAccelerationSensor3DOF(sensorType)),
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
	TemporaryScopedLock scopedLock(deviceLock);

	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		scopedLock.release();

		ocean_assert(sensorEvent.type == AST_ACCELEROMETER || sensorEvent.type == AST_LINEAR_ACCELERATION);

		Timestamp relativeTimestamp;
		const Timestamp unixTimestamp = convertTimestamp(sensorEvent, relativeTimestamp);

		const ASensorVector& acceleration = sensorEvent.acceleration;

		const ObjectIds objectIds(1, sensorObjectId_);
		const Acceleration3DOFSample::Measurements measurements(1, Vector3(Scalar(acceleration.x), Scalar(acceleration.y), Scalar(acceleration.z)));

		SampleRef sample(new Acceleration3DOFSample(unixTimestamp, objectIds, measurements));
		sample->setRelativeTimestamp(relativeTimestamp);

		postNewSample(sample);

		scopedLock.relock(deviceLock);
	}

	return 1;
}

}

}

}
