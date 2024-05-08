/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSFactory.h"
#include "ocean/devices/ios/IOSAccelerationSensor3DOF.h"
#include "ocean/devices/ios/IOSGPSTracker.h"
#include "ocean/devices/ios/IOSGravityTracker3DOF.h"
#include "ocean/devices/ios/IOSGyroSensor3DOF.h"
#include "ocean/devices/ios/IOSHeadingTracker3DOF.h"
#include "ocean/devices/ios/IOSOrientationTracker3DOF.h"
#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSFactory::IOSFactory() :
	Factory(nameIOSLibrary())
{
	registerDevices();
}

bool IOSFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new IOSFactory()));
}

bool IOSFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameIOSLibrary());
}

void IOSFactory::registerDevices()
{
	CMMotionManager* motionManager = MotionManager::get().object();

	if (motionManager.accelerometerAvailable)
	{
		Log::info() << "The device has an accelerometer";

		const Sensor::SensorType sensorType = Sensor::SENSOR_ACCELERATION_3DOF;

		registerDevice(IOSAccelerationSensor3DOF::deviceNameIOSAccelerationSensor3DOF(sensorType), IOSAccelerationSensor3DOF::deviceTypeIOSAccelerationSensor3DOF(sensorType), InstanceFunction::createStatic(createIOSAccelerationSensor3DOF));

		registerDevice(IOSGravityTracker3DOF::deviceNameIOSGravityTracker3DOF(), IOSGravityTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::createStatic(createIOSGravityTracker3DOF));

		if ([CMMotionManager availableAttitudeReferenceFrames] & CMAttitudeReferenceFrameXMagneticNorthZVertical)
		{
			registerDevice(IOSHeadingTracker3DOF::deviceNameIOSHeadingTracker3DOF(), IOSHeadingTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::createStatic(createIOSHeadingTracker3DOF));
		}
	}

	if (motionManager.gyroAvailable)
	{
		Log::info() << "The device has a gyroscope";

		const Sensor::SensorType sensorType = Sensor::SENSOR_GYRO_RAW_3DOF;

		registerDevice(IOSGyroSensor3DOF::deviceNameIOSGyroSensor3DOF(sensorType), IOSGyroSensor3DOF::deviceTypeIOSGyroSensor3DOF(sensorType), InstanceFunction::createStatic(createIOSGyroRawSensor3DOF));
	}

	if (motionManager.magnetometerAvailable)
	{
		Log::info() << "The device has a magnetometer";
	}

	if (motionManager.deviceMotionAvailable)
	{
		Log::info() << "The device has a motion device (including e.g., a linear accelerometer)";

		const Sensor::SensorType accelerationSensorType = Sensor::SENSOR_LINEAR_ACCELERATION_3DOF;
		registerDevice(IOSAccelerationSensor3DOF::deviceNameIOSAccelerationSensor3DOF(accelerationSensorType), IOSAccelerationSensor3DOF::deviceTypeIOSAccelerationSensor3DOF(accelerationSensorType), InstanceFunction::createStatic(createIOSLinearAccelerationSensor3DOF));

		const Sensor::SensorType gyroSensorType = Sensor::SENSOR_GYRO_UNBIASED_3DOF;
		registerDevice(IOSGyroSensor3DOF::deviceNameIOSGyroSensor3DOF(gyroSensorType), IOSGyroSensor3DOF::deviceTypeIOSGyroSensor3DOF(gyroSensorType), InstanceFunction::createStatic(createIOSGyroUnbiasedSensor3DOF));

		registerDevice(IOSOrientationTracker3DOF::deviceNameIOSOrientationTracker3DOF(), IOSOrientationTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::createStatic(createIOSOrientationTracker3DOF));
	}

	registerDevice(IOSGPSTracker::deviceNameIOSGPSTracker(), IOSGPSTracker::deviceTypeIOSGPSTracker(), InstanceFunction::createStatic(createIOSGPSTracker));
}

Device* IOSFactory::createIOSAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	return new IOSAccelerationSensor3DOF(Sensor::SENSOR_ACCELERATION_3DOF);
}

Device* IOSFactory::createIOSLinearAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	return new IOSAccelerationSensor3DOF(Sensor::SENSOR_LINEAR_ACCELERATION_3DOF);
}

Device* IOSFactory::createIOSOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	return new IOSOrientationTracker3DOF();
}

Device* IOSFactory::createIOSGyroRawSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	return new IOSGyroSensor3DOF(Sensor::SENSOR_GYRO_RAW_3DOF);
}

Device* IOSFactory::createIOSGyroUnbiasedSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	return new IOSGyroSensor3DOF(Sensor::SENSOR_GYRO_UNBIASED_3DOF);
}

Device* IOSFactory::createIOSGPSTracker(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == IOSGPSTracker::deviceNameIOSGPSTracker());
	ocean_assert(deviceType == IOSGPSTracker::deviceTypeIOSGPSTracker());

	return new IOSGPSTracker();
}

Device* IOSFactory::createIOSGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == IOSGravityTracker3DOF::deviceNameIOSGravityTracker3DOF());
	ocean_assert(deviceType == IOSGravityTracker3DOF::deviceTypeOrientationTracker3DOF());

	return new IOSGravityTracker3DOF();
}

Device* IOSFactory::createIOSHeadingTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == IOSHeadingTracker3DOF::deviceNameIOSHeadingTracker3DOF());
	ocean_assert(deviceType == IOSHeadingTracker3DOF::deviceTypeOrientationTracker3DOF());

	return new IOSHeadingTracker3DOF();
}

}

}

}
