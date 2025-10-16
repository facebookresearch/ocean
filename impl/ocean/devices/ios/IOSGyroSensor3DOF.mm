/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSGyroSensor3DOF.h"

#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSGyroSensor3DOF::IOSGyroSensor3DOF(const SensorType sensorType) :
	Device(deviceNameIOSGyroSensor3DOF(sensorType), deviceTypeIOSGyroSensor3DOF(sensorType)),
	IOSDevice(deviceNameIOSGyroSensor3DOF(sensorType), deviceTypeIOSGyroSensor3DOF(sensorType)),
	Measurement(deviceNameIOSGyroSensor3DOF(sensorType), deviceTypeIOSGyroSensor3DOF(sensorType)),
	Sensor(deviceNameIOSGyroSensor3DOF(sensorType), deviceTypeIOSGyroSensor3DOF(sensorType)),
	IOSSensor(deviceNameIOSGyroSensor3DOF(sensorType), deviceTypeIOSGyroSensor3DOF(sensorType)),
	GyroSensor3DOF(deviceNameIOSGyroSensor3DOF(sensorType), sensorType)
{
	ocean_assert(MotionManager::get().object().isAccelerometerAvailable);
	ocean_assert(deviceType.minorType() == Sensor::SENSOR_GYRO_RAW_3DOF || deviceType.minorType() == Sensor::SENSOR_GYRO_UNBIASED_3DOF);
}

IOSGyroSensor3DOF::~IOSGyroSensor3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSGyroSensor3DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	if (deviceType.minorType() == Sensor::SENSOR_GYRO_UNBIASED_3DOF)
	{
		deviceMotionListenerId_ = MotionManager::get().addListener(MotionManager::DeviceMotionCallback(*this, &IOSGyroSensor3DOF::onDeviceMotion));
	}
	else
	{
		CMMotionManager* motionManager = MotionManager::get().object();

		ocean_assert(deviceType.minorType() == Sensor::SENSOR_GYRO_RAW_3DOF);
		ocean_assert(motionManager.gyroAvailable);

		[motionManager startGyroUpdatesToQueue:[[NSOperationQueue alloc] init] withHandler:^(CMGyroData* gyroData, NSError* nsError)
			{
				Timestamp relativeTimestamp;
				const Timestamp unixTimestamp = convertTimestamp(gyroData.timestamp, relativeTimestamp);

				ObjectIds objectIds(1, sensorObjectId_);
				Gyro3DOFSample::Measurements measurements(1, Vector3(Scalar(gyroData.rotationRate.x), Scalar(gyroData.rotationRate.y), Scalar(gyroData.rotationRate.z)));

				const SampleRef sample(new Gyro3DOFSample(unixTimestamp, std::move(objectIds), std::move(measurements)));
				sample->setRelativeTimestamp(relativeTimestamp);

				postNewSample(sample);
			}];
	}

	isStarted_ = true;

	return true;
}

bool IOSGyroSensor3DOF::pause()
{
	return stop();
}

bool IOSGyroSensor3DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	if (deviceType.minorType() == Sensor::SENSOR_GYRO_UNBIASED_3DOF)
	{
		MotionManager::get().removeListener(deviceMotionListenerId_);
	}
	else
	{
		ocean_assert(deviceType.minorType() == Sensor::SENSOR_GYRO_RAW_3DOF);
		[MotionManager::get().object() stopGyroUpdates];
	}

	isStarted_ = false;

	return true;
}

void IOSGyroSensor3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
{
	Timestamp relativeTimestamp;
	const Timestamp unixTimestamp = convertTimestamp(deviceMotion.timestamp, relativeTimestamp);

	ObjectIds objectIds(1, sensorObjectId_);
	Gyro3DOFSample::Measurements measurements(1, Vector3(Scalar(deviceMotion.rotationRate.x), Scalar(deviceMotion.rotationRate.y), Scalar(deviceMotion.rotationRate.z)));

	const SampleRef sample(new Gyro3DOFSample(unixTimestamp, std::move(objectIds), std::move(measurements)));
	sample->setRelativeTimestamp(relativeTimestamp);

	postNewSample(sample);
}

}

}

}
