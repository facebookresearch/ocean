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

	if (isStarted_)
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
				Timestamp relativeTimestamp;
				const Timestamp unixTimestamp = convertTimestamp(accelerometerData.timestamp, relativeTimestamp);

				// X, Y, Z-axis acceleration in G’s (gravitational force).
				const Vector3 iosAcceleration = Vector3(Scalar(accelerometerData.acceleration.x), Scalar(accelerometerData.acceleration.y), Scalar(accelerometerData.acceleration.z)) * standardGravity_;

				// on iOS, the acceleration sensor returns [0, 0, -9.81], gravity is negative, however the phone is accelerated towards the earth, so it must be positive
				const Vector3 acceleration = -iosAcceleration;

				ObjectIds objectIds(1, sensorObjectId_);
				Acceleration3DOFSample::Measurements measurements(1, acceleration);

				const SampleRef sample(SampleRef(new Acceleration3DOFSample(unixTimestamp, std::move(objectIds), std::move(measurements), Metadata())));
				sample->setRelativeTimestamp(relativeTimestamp);

				postNewSample(sample);
			}];
	}

	isStarted_ = true;

	return true;
}

bool IOSAccelerationSensor3DOF::pause()
{
	return stop();
}

bool IOSAccelerationSensor3DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!isStarted_)
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

	isStarted_ = false;

	return true;
}

void IOSAccelerationSensor3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
{
	Timestamp relativeTimestamp;
	const Timestamp unixTimestamp = convertTimestamp(deviceMotion.timestamp, relativeTimestamp);

	// X, Y, Z-axis acceleration in G’s (gravitational force).
	const Vector3 iosAcceleration = Vector3(Scalar(deviceMotion.userAcceleration.x), Scalar(deviceMotion.userAcceleration.y), Scalar(deviceMotion.userAcceleration.z)) * standardGravity_;

	// on iOS, the acceleration sensor returns [0, 0, -9.81], gravity is negative, however the phone is accelerated towards the earth, so it must be positive
	const Vector3 acceleration = -iosAcceleration;

	ObjectIds objectIds(1, sensorObjectId_);
	Acceleration3DOFSample::Measurements measurements(1, acceleration);

	const SampleRef sample(new Acceleration3DOFSample(unixTimestamp, std::move(objectIds), std::move(measurements), Metadata()));
	sample->setRelativeTimestamp(relativeTimestamp);

	postNewSample(sample);
}

}

}

}
