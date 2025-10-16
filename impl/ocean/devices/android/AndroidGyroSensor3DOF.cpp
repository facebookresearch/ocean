/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidGyroSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidGyroSensor3DOF::AndroidGyroSensor3DOF(const SensorType sensorType, const ASensor* sensor) :
	Device(deviceNameAndroidGyroSensor3DOF(sensorType), deviceTypeAndroidGyroSensor3DOF(sensorType)),
	AndroidDevice(deviceNameAndroidGyroSensor3DOF(sensorType), deviceTypeAndroidGyroSensor3DOF(sensorType)),
	Measurement(deviceNameAndroidGyroSensor3DOF(sensorType), deviceTypeAndroidGyroSensor3DOF(sensorType)),
	Sensor(deviceNameAndroidGyroSensor3DOF(sensorType), deviceTypeAndroidGyroSensor3DOF(sensorType)),
	AndroidSensor(deviceNameAndroidGyroSensor3DOF(sensorType), deviceTypeAndroidGyroSensor3DOF(sensorType)),
	GyroSensor3DOF(deviceNameAndroidGyroSensor3DOF(sensorType), sensorType)
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

AndroidGyroSensor3DOF::~AndroidGyroSensor3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();

	ocean_assert(sensorManager_ != nullptr && eventQueue_ != nullptr);
	ASensorManager_destroyEventQueue(sensorManager_, eventQueue_);
}

int AndroidGyroSensor3DOF::onEventFunction()
{
	TemporaryScopedLock scopedLock(deviceLock);

	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		scopedLock.release();

		ocean_assert(sensorEvent.type == AST_GYROSCOPE || sensorEvent.type == AST_GYROSCOPE_UNCALIBRATED);

		Timestamp relativeTimestamp;
		const Timestamp unixTimestamp = convertTimestamp(sensorEvent, relativeTimestamp);

		const float* sensorData = sensorEvent.data;

		ObjectIds objectIds(1, sensorObjectId_);

		Gyro3DOFSample::Measurements measurements(1, Vector3(Scalar(sensorData[0]), Scalar(sensorData[1]), Scalar(sensorData[2])));

		SampleRef sample(new Gyro3DOFSample(unixTimestamp, std::move(objectIds), std::move(measurements)));
		sample->setRelativeTimestamp(relativeTimestamp);

		postNewSample(sample);

		scopedLock.relock(deviceLock);
	}

	return 1;
}

}

}

}
