/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSAccelerationSensor3DOF.h"

#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSAccelerationSensor3DOF::IOSAccelerationSensor3DOF(const SensorType sensorType) :
	Device(deviceNameIOSAccelerationSensor3DOF(sensorType), deviceTypeIOSAccelerationSensor3DOF(sensorType)),
	IOSDevice(deviceNameIOSAccelerationSensor3DOF(sensorType), deviceTypeIOSAccelerationSensor3DOF(sensorType)),
	Measurement(deviceNameIOSAccelerationSensor3DOF(sensorType), deviceTypeIOSAccelerationSensor3DOF(sensorType)),
	Sensor(deviceNameIOSAccelerationSensor3DOF(sensorType), deviceTypeIOSAccelerationSensor3DOF(sensorType)),
	IOSSensor(deviceNameIOSAccelerationSensor3DOF(sensorType), deviceTypeIOSAccelerationSensor3DOF(sensorType)),
	AccelerationSensor3DOF(deviceNameIOSAccelerationSensor3DOF(sensorType), sensorType)
{
	ocean_assert(MotionManager::get().object().isAccelerometerAvailable);
	ocean_assert(deviceType.minorType() == Sensor::SENSOR_ACCELERATION_3DOF || deviceType.minorType() == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF);
}

IOSAccelerationSensor3DOF::~IOSAccelerationSensor3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSAccelerationSensor3DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (sensorIsStarted)
	{
		return true;
	}

	if (deviceType.minorType() == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF)
	{
		deviceMotionListenerId_ = MotionManager::get().addListener(MotionManager::DeviceMotionCallback(*this, &IOSAccelerationSensor3DOF::onDeviceMotion));
	}
	else
	{
		CMMotionManager* motionManager = MotionManager::get().object();

		ocean_assert(deviceType.minorType() == Sensor::SENSOR_ACCELERATION_3DOF);
		ocean_assert(motionManager.isAccelerometerAvailable);

		[motionManager startAccelerometerUpdatesToQueue:[[NSOperationQueue alloc] init] withHandler:^(CMAccelerometerData* accelerometerData, NSError* nsError)
			{
				if (sensorFirstUnixEventTimestamp.isInvalid())
				{
					sensorFirstUnixEventTimestamp.toNow();
					sensorFirstIOSEventTimestamp = [[NSProcessInfo processInfo] systemUptime];
				}

				const Timestamp timestamp(sensorFirstUnixEventTimestamp + accelerometerData.timestamp - sensorFirstIOSEventTimestamp);

				ObjectIds objectIds(1, sensorObjectId_);
				Acceleration3DOFSample::Measurements measurements(1, Vector3(Scalar(accelerometerData.acceleration.x * 9.81), Scalar(accelerometerData.acceleration.y * 9.81), Scalar(accelerometerData.acceleration.z * 9.81)));

				postNewSample(SampleRef(new Acceleration3DOFSample(timestamp, std::move(objectIds), std::move(measurements), Metadata())));
			}];
	}

	sensorIsStarted = true;

	return sensorIsStarted;
}

bool IOSAccelerationSensor3DOF::pause()
{
	return stop();
}

bool IOSAccelerationSensor3DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!sensorIsStarted)
	{
		return true;
	}

	if (deviceType.minorType() == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF)
	{
		MotionManager::get().removeListener(deviceMotionListenerId_);
	}
	else
	{
		ocean_assert(deviceType.minorType() == Sensor::SENSOR_ACCELERATION_3DOF);
		[MotionManager::get().object() stopAccelerometerUpdates];
	}

	sensorIsStarted = false;
	return true;
}

void IOSAccelerationSensor3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
{
	if (sensorFirstUnixEventTimestamp.isInvalid())
	{
		sensorFirstUnixEventTimestamp.toNow();
		sensorFirstIOSEventTimestamp = [[NSProcessInfo processInfo] systemUptime];
	}

	const Timestamp timestamp(sensorFirstUnixEventTimestamp + deviceMotion.timestamp - sensorFirstIOSEventTimestamp);

	ObjectIds objectIds(1, sensorObjectId_);
	Acceleration3DOFSample::Measurements measurements(1, Vector3(Scalar(deviceMotion.userAcceleration.x * 9.81), Scalar(deviceMotion.userAcceleration.y * 9.81), Scalar(deviceMotion.userAcceleration.z * 9.81)));

	postNewSample(SampleRef(new Acceleration3DOFSample(timestamp, std::move(objectIds), std::move(measurements), Metadata())));
}

}

}

}
