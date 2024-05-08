/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_SENSOR_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_SENSOR_H

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidDevice.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/devices/Sensor.h"

#include <android/sensor.h>

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * This class implements a sensor for the Android library.
 * The coordinate system of each Android sensor is defined so that the origin of the coordinate system is located in the center of the device.<br>
 * The x-axis is horizontal and pointing to the right of the device (if the device is held in default orientation).<br>
 * The y-axis is vertical and pointing to the top of the device.<br>
 * The z-axis is perpendicular to the screen plane and pointing towards the user (a right handed coordinate system).
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_ANDROID_EXPORT AndroidSensor :
	virtual public AndroidDevice,
	virtual public Sensor
{
	protected:

		/**
		 * This class implements the manager for the looper thread.
		 */
		class LooperManager :
			public Singleton<LooperManager>,
			protected Thread
		{
			friend class Singleton<LooperManager>;

			public:

				/**
				 * Returns the looper of the manager.
				 * @return Looper of the manager, will be valid
				 */
				ALooper* looper();

			protected:

				/**
				 * Creates a new manager object and creates a looper.
				 */
				LooperManager();

				/**
				 * Thread run function.
				 * @see Thread::threadRun().
				 */
				void threadRun() override;

			protected:

				/// The looper of this manager.
				ALooper* looper_ = nullptr;
		};

	public:

		/**
		 * Definition of Android sensor types as defined by the Android API.
		 * The values are defined by the NDK Android API and thus must not changed.
		 */
		enum AndroidSensorType
		{
			/// Measures the acceleration force in m/s2 that is applied to a device on all three physical axes (x, y, and z), including the force of gravity.
			AST_ACCELEROMETER = 1,
			/// Measures the ambient geomagnetic field for all three physical axes (x, y, z) in microT.
			AST_MAGNETIC_FIELD = 2,
			/// Measures degrees of rotation that a device makes around all three physical axes (x, y, z), deprecated since 2.2.
			AST_ORIENTATION = 3,
			/// Measures a device's rate of rotation in rad/s around each of the three physical axes (x, y, and z).
			AST_GYROSCOPE = 4,
			/// Measures the ambient light level (illumination) in lx.
			AST_LIGHT = 5,
			/// Measures the ambient air pressure in hPa or mbar.
			AST_PRESSURE = 6,
			/// Measures the temperature of the device in degrees Celsius. This sensor implementation varies across devices and this sensor was replaced with the TYPE_AMBIENT_TEMPERATURE sensor in API Level 14
			AST_TEMPERATURE = 7,
			/// Measures the proximity of an object in cm relative to the view screen of a device. This sensor is typically used to determine whether a handset is being held up to a person's ear.
			AST_PROXIMITY = 8,
			/// Measures the force of gravity in m/s2 that is applied to a device on all three physical axes (x, y, z).
			AST_GRAVITY = 9,
			/// Measures the acceleration force in m/s2 that is applied to a device on all three physical axes (x, y, and z), excluding the force of gravity.
			AST_LINEAR_ACCELERATION = 10,
			/// Measures the orientation of a device by providing the three elements of the device's rotation vector.
			AST_ROTATION_VECTOR = 11,
			/// Measures the relative ambient humidity in percent (%).
			AST_RELATIVE_HUMIDITY = 12,
			/// Measures the ambient room temperature in degrees Celsius.
			AST_AMBIENT_TEMPERATURE = 13,
			AST_MAGNETIC_FIELD_UNCALIBRATED = 14,
			AST_GAME_ROTATION_VECTOR = 15,
			AST_GYROSCOPE_UNCALIBRATED = 16,
			AST_SIGNIFICANT_MOTION = 17,
			AST_STEP_DETECTOR = 18,
			AST_STEP_COUNTER = 19,
			AST_GEOMAGNETIC_ROTATION_VECTOR = 20,
			AST_HEART_RATE = 21,
			AST_POSE_6DOF = 28,
			AST_STATIONARY_DETECT = 29,
			AST_MOTION_DETECT = 30,
			AST_HEART_BEAT = 31
		};

	public:

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		bool pause() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns the instance of the sensor manager.
		 * @return The sensor manager instance
		 */
		static ASensorManager* sensorManager();

	protected:

		/**
		 * Creates a new sensor by its name and type.
		 * @param name The name of the sensor
		 * @param type Major and minor device type of the sensor
		 */
		AndroidSensor(const std::string& name, const DeviceType type);

		/**
		 * Registers this sensor for the event function.
		 * @param sensorManager The sensor manager which has been used to create the sensor, must be valid
		 */
		bool registerForEventFunction(ASensorManager* sensorManager);

		/**
		 * The actual event function of this device.
		 * @return 1 to receive further events, 0 to stop receiving events
		 */
		virtual int onEventFunction() = 0;

	private:

		/**
		 * The static event function.
		 * @param fd File descriptor
		 * @param events The number of events
		 * @param data The event data information, if any
		 * @return 1 to receive further events, 0 to stop receiving events
		 */
		static inline int onEventFunctionStatic(int fd, int events, void* data);

	protected:

		/// The object id of this sensor.
		ObjectId sensorObjectId_ = invalidObjectId();

		/// The sensor manager of this device.
		ASensorManager* sensorManager_ = nullptr;

		/// The Android NDK sensor of this device.
		const ASensor* sensor_ = nullptr;

		/// The Android NDK event queue of this device.
		ASensorEventQueue* eventQueue_ = nullptr;

		/// True, if this sensor is started.
		bool isStarted_ = false;
};

inline int AndroidSensor::onEventFunctionStatic(int fd, int events, void* data)
{
	if (data == nullptr)
	{
		return 0;
	}

	AndroidDevice* androidDevice = static_cast<AndroidDevice*>(data);

	if (androidDevice == nullptr)
	{
		return 0;
	}

	AndroidSensor* androidSensor = dynamic_cast<AndroidSensor*>(androidDevice);
	ocean_assert(androidSensor != nullptr);

	if (androidSensor)
	{
		return androidSensor->onEventFunction();
	}

	return 0;
}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_SENSOR_H
