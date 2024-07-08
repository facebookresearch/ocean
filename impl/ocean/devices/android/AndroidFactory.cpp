/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidFactory.h"
#include "ocean/devices/android/AndroidAccelerationSensor3DOF.h"
#include "ocean/devices/android/AndroidGPSTracker.h"
#include "ocean/devices/android/AndroidGravityTracker3DOF.h"
#include "ocean/devices/android/AndroidGyroSensor3DOF.h"
#include "ocean/devices/android/AndroidHeadingTracker3DOF.h"
#include "ocean/devices/android/AndroidOrientationTracker3DOF.h"
#include "ocean/devices/android/AndroidSensor.h"

#include <android/api-level.h>

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidFactory::AndroidFactory() :
	Factory(nameAndroidLibrary())
{
#if defined(__NDK_MAJOR__) && defined(__ANDROID_API__)
	Log::debug() << "Devices::Android uses Android SDK version " << __NDK_MAJOR__ << " and API level " << __ANDROID_API__;
#endif

	registerDevices();
}

bool AndroidFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new AndroidFactory()));
}

bool AndroidFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameAndroidLibrary());
}

void AndroidFactory::newGPSLocation(const double latitude, const double longitude, const float altitude, const float direction, const float speed, const float accuracy, const float altitudeAccuracy, const float directionAccuracy, const float speedAccuracy, const Timestamp& timestamp)
{
	const DeviceRef device = DeviceRefManager::get().device(AndroidGPSTracker::deviceNameAndroidGPSTracker());

	if (device)
	{
		device.force<AndroidGPSTracker>().newGPSLocation(AndroidGPSTracker::Location(latitude, longitude, altitude, direction, speed, accuracy, altitudeAccuracy, directionAccuracy, speedAccuracy), timestamp);
	}
}

void AndroidFactory::registerDevices()
{
	ASensorManager* sensorManager = AndroidSensor::sensorManager();

	if (!sensorManager)
	{
		Log::error() << "No sensor manager!";
		return;
	}

	ASensorList sensorList;

	const int number = ASensorManager_getSensorList(sensorManager, &sensorList);

	Log::debug() << "The sensor manager provides " << number << " individual sensors:";
	Log::debug() << " ";

	for (int n = 0; n < number; ++n)
	{
		const ASensor* sensor = sensorList[n];
		const char* name = ASensor_getName(sensor);

		if (name)
		{
			Log::debug() << "Name: " << name;
		}

		const char* vendor = ASensor_getVendor(sensor);

		if (vendor)
		{
			Log::debug() << "Vendor: " << vendor;
		}

		const int type = ASensor_getType(sensor);

		switch (type)
		{
			case AndroidSensor::AST_ACCELEROMETER:
			{
				Log::debug() << "Type: Accelerometer";

				if (sensorAccelerometer_ == nullptr)
				{
					const Sensor::SensorType sensorType = Sensor::SENSOR_ACCELERATION_3DOF;

					sensorAccelerometer_ = sensor;
					registerDevice(AndroidAccelerationSensor3DOF::deviceNameAndroidAccelerationSensor3DOF(sensorType), AndroidAccelerationSensor3DOF::deviceTypeAndroidAccelerationSensor3DOF(sensorType), InstanceFunction::create(*this, &AndroidFactory::createAndroidAccelerationSensor3DOF));
				}

				break;
			}

			case AndroidSensor::AST_MAGNETIC_FIELD:
				Log::debug() << "Type: Magnetic field";
				break;

			case AndroidSensor::AST_ORIENTATION:
				Log::debug() << "Type: Orientation (deprecated)";
				break;

			case AndroidSensor::AST_GYROSCOPE:
			{
				Log::debug() << "Type: Gyroscope";

				if (sensorGyroUnbiased_ == nullptr)
				{
					const Sensor::SensorType sensorType = Sensor::SENSOR_GYRO_UNBIASED_3DOF;

					sensorGyroUnbiased_ = sensor;
					registerDevice(AndroidGyroSensor3DOF::deviceNameAndroidGyroSensor3DOF(sensorType), AndroidGyroSensor3DOF::deviceTypeAndroidGyroSensor3DOF(sensorType), InstanceFunction::create(*this, &AndroidFactory::createAndroidGyroUnbiasedSensor3DOF));
				}

				break;
			}

			case AndroidSensor::AST_LIGHT:
				Log::debug() << "Type: Light";
				break;

			case AndroidSensor::AST_PRESSURE:
				Log::debug() << "Type: Pressure";
				break;

			case AndroidSensor::AST_TEMPERATURE:
				Log::debug() << "Type Temperature";
				break;

			case AndroidSensor::AST_PROXIMITY:
				Log::debug() << "Type: Proximity";
				break;

			case AndroidSensor::AST_GRAVITY:
				Log::debug() << "Type: Gravity";

				if (sensorGravity_ == nullptr)
				{
					sensorGravity_ = sensor;
					registerDevice(AndroidGravityTracker3DOF::deviceNameAndroidGravityTracker3DOF(), AndroidGravityTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::create(*this, &AndroidFactory::createAndroidGravityTracker3DOF));
				}

				break;

			case AndroidSensor::AST_LINEAR_ACCELERATION:
			{
				Log::debug() << "Type: Linear Acceleration";

				if (sensorLinearAccelerometer_ == nullptr)
				{
					const Sensor::SensorType sensorType = Sensor::SENSOR_LINEAR_ACCELERATION_3DOF;

					sensorLinearAccelerometer_ = sensor;
					registerDevice(AndroidAccelerationSensor3DOF::deviceNameAndroidAccelerationSensor3DOF(sensorType), AndroidAccelerationSensor3DOF::deviceTypeAndroidAccelerationSensor3DOF(sensorType), InstanceFunction::create(*this, &AndroidFactory::createAndroidLinearAccelerationSensor3DOF));
				}

				break;
			}

			case AndroidSensor::AST_ROTATION_VECTOR:
			{
				Log::debug() << "Type: Rotation Vector";

				if (sensorRotationVector_ == nullptr)
				{
					sensorRotationVector_ = sensor;
					registerDevice(AndroidOrientationTracker3DOF::deviceNameAndroidOrientationTracker3DOF(), AndroidOrientationTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::create(*this, &AndroidFactory::createAndroidOrientationTracker3DOF));

					registerDevice(AndroidHeadingTracker3DOF::deviceNameAndroidHeadingTracker3DOF(), AndroidHeadingTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::create(*this, &AndroidFactory::createAndroidHeadingTracker3DOF));
				}

				break;
			}

			case AndroidSensor::AST_RELATIVE_HUMIDITY:
				Log::debug() << "Type: Relative Humidity";
				break;

			case AndroidSensor::AST_AMBIENT_TEMPERATURE:
				Log::debug() << "Type: Ambient Temperature";
				break;

			case AndroidSensor::AST_MAGNETIC_FIELD_UNCALIBRATED:
				Log::debug() << "Type: Magnetic Field Uncalibrated";
				break;

			case AndroidSensor::AST_GAME_ROTATION_VECTOR:
				Log::debug() << "Type: Game Rotation Vector";

				if (sensorGameRotationVector_ == nullptr)
				{
					sensorGameRotationVector_ = sensor;
					registerDevice(AndroidOrientationTracker3DOF::deviceNameAndroidOrientationTracker3DOF(), AndroidOrientationTracker3DOF::deviceTypeOrientationTracker3DOF(), InstanceFunction::create(*this, &AndroidFactory::createAndroidOrientationTracker3DOF));
				}

				break;

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

			case AndroidSensor::AST_GYROSCOPE_UNCALIBRATED:
			{
				Log::debug() << "Type: Gyroscope Uncalibrated";

				if (sensorGyroRaw_ == nullptr)
				{
					const Sensor::SensorType sensorType = Sensor::SENSOR_GYRO_RAW_3DOF;

					sensorGyroRaw_ = sensor;
					registerDevice(AndroidGyroSensor3DOF::deviceNameAndroidGyroSensor3DOF(sensorType), AndroidGyroSensor3DOF::deviceTypeAndroidGyroSensor3DOF(sensorType), InstanceFunction::create(*this, &AndroidFactory::createAndroidGyroRawSensor3DOF));
				}

				break;
			}
#endif // __ANDROID_API__ >= 24

			case AndroidSensor::AST_SIGNIFICANT_MOTION:
				Log::debug() << "Type: Significant Motion";
				break;

			case AndroidSensor::AST_STEP_DETECTOR:
				Log::debug() << "Type: Step Detector";
				break;

			case AndroidSensor::AST_STEP_COUNTER:
				Log::debug() << "Type: Step Counter";
				break;

			case AndroidSensor::AST_GEOMAGNETIC_ROTATION_VECTOR:
				Log::debug() << "Type: Geomagnetic Rotation Vector";
				break;

			case AndroidSensor::AST_HEART_RATE:
				Log::debug() << "Type: Heart Rate";
				break;

			case AndroidSensor::AST_POSE_6DOF:
				Log::debug() << "Type: Pose 6DOF";
				break;

			case AndroidSensor::AST_STATIONARY_DETECT:
				Log::debug() << "Type: Stationary Detect";
				break;

			case AndroidSensor::AST_MOTION_DETECT:
				Log::debug() << "Type: Motion Detect";
				break;

			case AndroidSensor::AST_HEART_BEAT:
				Log::debug() << "Type: Heart Beat";
				break;

			default:
#if defined(__ANDROID_API__) && __ANDROID_API__ >= 21
				Log::debug() << "Type: UNKNOWN: " << type << ", \"" << ASensor_getStringType(sensor) << "\"";
#else
				Log::debug() << "Type: UNKNOWN: " << type;
#endif
				break;
		}

		Log::debug() << "Resolution: " << ASensor_getResolution(sensor);
		Log::debug() << "Min delay: " << double(ASensor_getMinDelay(sensor)) / double(1000.0) << "ms";

		Log::debug() << " ";
	}

	registerDevice(AndroidGPSTracker::deviceNameAndroidGPSTracker(), AndroidGPSTracker::deviceTypeAndroidGPSTracker(), InstanceFunction::create(*this, &AndroidFactory::createAndroidGPSTracker));
}

Device* AndroidFactory::createAndroidAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(sensorAccelerometer_ != nullptr);

	return new AndroidAccelerationSensor3DOF(Sensor::SENSOR_ACCELERATION_3DOF, sensorAccelerometer_);
}

Device* AndroidFactory::createAndroidLinearAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(sensorLinearAccelerometer_ != nullptr);

	return new AndroidAccelerationSensor3DOF(Sensor::SENSOR_LINEAR_ACCELERATION_3DOF, sensorLinearAccelerometer_);
}

Device* AndroidFactory::createAndroidOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	if (sensorGameRotationVector_ != nullptr)
	{
		// the game rotation vector is preferred because it is not sensitive to changes in the magnetic field
		// and seems to have a significant higher update rate on some Android devices

		return new AndroidOrientationTracker3DOF(sensorGameRotationVector_);
	}
	else
	{
		ocean_assert(sensorRotationVector_ != nullptr);
		return new AndroidOrientationTracker3DOF(sensorRotationVector_);
	}
}

Device* AndroidFactory::createAndroidGyroRawSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(sensorGyroRaw_ != nullptr);

	return new AndroidGyroSensor3DOF(Sensor::SENSOR_GYRO_RAW_3DOF, sensorGyroRaw_);
}

Device* AndroidFactory::createAndroidGyroUnbiasedSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(sensorGyroUnbiased_ != nullptr);

	return new AndroidGyroSensor3DOF(Sensor::SENSOR_GYRO_UNBIASED_3DOF, sensorGyroUnbiased_);
}

Device* AndroidFactory::createAndroidGPSTracker(const std::string& name, const Device::DeviceType& deviceType)
{
	return new AndroidGPSTracker();
}

Device* AndroidFactory::createAndroidGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(sensorGravity_ != nullptr);

	return new AndroidGravityTracker3DOF(sensorGravity_);
}

Device* AndroidFactory::createAndroidHeadingTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(sensorRotationVector_ != nullptr);

	return new AndroidHeadingTracker3DOF(sensorRotationVector_);
}

}

}

}
