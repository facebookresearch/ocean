/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SENSOR_H
#define META_OCEAN_DEVICES_SENSOR_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Measurement.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class Sensor;

/**
 * Definition of a sensor reference object.
 * @see Sensor.
 * @ingroup devices
 */
typedef SmartObjectRef<Sensor, Device> SensorRef;

/**
 * This class implements the base class for all sensors.
 * Sensor provides measurement values, their types depend on the sensors.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Sensor : virtual public Measurement
{
	public:

		/**
		 * Definition of different sensor types.
		 */
		enum SensorType : uint32_t
		{
			/// Invalid sensor.
			SENSOR_INVALID = MINOR_INVALID,
			/// 3DOF acceleration sensor.
			SENSOR_ACCELERATION_3DOF,
			/// 3DOF linear acceleration sensor (excluding gravity).
			SENSOR_LINEAR_ACCELERATION_3DOF,
			/// 3DOF gyro sensor (raw samples).
			SENSOR_GYRO_RAW_3DOF,
			/// 3DOF gyro sensor (unbiased samples).
			SENSOR_GYRO_UNBIASED_3DOF
		};

		/**
		 * Translates the sensor type to a readable string.
		 * @param sensorType The sensor type to translate
		 * @return The readable string, empty if the type is unknown
		 */
		static std::string translateSensorType(const SensorType sensorType);

		/**
		 * Translates the sensor type from a readable string to a value.
		 * @param sensorType The sensor type to translate
		 * @return The tanslated value
		 */
		static SensorType translateSensorType(const std::string& sensorType);

	protected:

		/**
		 * Creates a new sensor object.
		 * @param name The name of the sensor
		 * @param type Major and minor device type of the device
		 */
		Sensor(const std::string& name, const DeviceType type);

		/**
		 * Destructs a sensor object.
		 */
		virtual ~Sensor();
};

}

}

#endif // META_OCEAN_DEVICES_SENSOR_H
