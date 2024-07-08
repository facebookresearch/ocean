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

	if (sensorIsStarted)
		return true;

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
				if (sensorFirstUnixEventTimestamp.isInvalid())
				{
					sensorFirstUnixEventTimestamp.toNow();
					sensorFirstIOSEventTimestamp = [[NSProcessInfo processInfo] systemUptime];
				}

				const Timestamp timestamp(sensorFirstUnixEventTimestamp + gyroData.timestamp - sensorFirstIOSEventTimestamp);

				ObjectIds objectIds(1, sensorObjectId_);
				Gyro3DOFSample::Measurements measurements(1, Vector3(Scalar(gyroData.rotationRate.x), Scalar(gyroData.rotationRate.y), Scalar(gyroData.rotationRate.z)));

				postNewSample(SampleRef(new Gyro3DOFSample(timestamp, std::move(objectIds), std::move(measurements))));
			}];
	}

	sensorIsStarted = true;

	return sensorIsStarted;
}

bool IOSGyroSensor3DOF::pause()
{
	return stop();
}

bool IOSGyroSensor3DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!sensorIsStarted)
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

	sensorIsStarted = false;
	return true;
}

void IOSGyroSensor3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
{
	if (sensorFirstUnixEventTimestamp.isInvalid())
	{
		sensorFirstUnixEventTimestamp.toNow();
		sensorFirstIOSEventTimestamp = [[NSProcessInfo processInfo] systemUptime];
	}

	const Timestamp timestamp(sensorFirstUnixEventTimestamp + deviceMotion.timestamp - sensorFirstIOSEventTimestamp);

	ObjectIds objectIds(1, sensorObjectId_);
	Gyro3DOFSample::Measurements measurements(1, Vector3(Scalar(deviceMotion.rotationRate.x), Scalar(deviceMotion.rotationRate.y), Scalar(deviceMotion.rotationRate.z)));

	postNewSample(SampleRef(new Gyro3DOFSample(timestamp, std::move(objectIds), std::move(measurements))));
}

}

}

}
