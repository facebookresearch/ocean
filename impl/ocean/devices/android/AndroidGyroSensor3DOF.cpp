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
	const ScopedLock scopedLock(deviceLock);

	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		ocean_assert(sensorEvent.type == AST_GYROSCOPE || sensorEvent.type == AST_GYROSCOPE_UNCALIBRATED);

		if (firstUnixEventTimestamp_.isInvalid())
		{
			// pairing both timestamp may not be ideal but it seems to be the best solution as the Android timestamp seem to be arbitrary for individual sensors
			// **NOTE** perhaps the timestamp of the Android event may restart/change after waking up - this may result in wrong timestamps

			firstUnixEventTimestamp_.toNow();
			firstAndroidEventTimestamp_ = sensorEvent.timestamp;
		}

		const Timestamp timestamp(firstUnixEventTimestamp_ + double(sensorEvent.timestamp - firstAndroidEventTimestamp_) / 1000000000.0);

		const float* sensorData = sensorEvent.data;

		ObjectIds objectIds(1, sensorObjectId_);

		Gyro3DOFSample::Measurements measurements(1, Vector3(Scalar(sensorData[0]), Scalar(sensorData[1]), Scalar(sensorData[2])));

		postNewSample(SampleRef(new Gyro3DOFSample(timestamp, std::move(objectIds), std::move(measurements))));
	}

	return 1;
}

}

}

}
