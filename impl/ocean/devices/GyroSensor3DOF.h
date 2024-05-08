/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_GYRO_SENSOR_3DOF_H
#define META_OCEAN_DEVICES_GYRO_SENSOR_3DOF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Sensor.h"

#include "ocean/math/Vector3.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class GyroSensor3DOF;

/**
 * Definition of a smart object reference for a 3DOF gyro sensor.
 * @see GyroSensor3DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<GyroSensor3DOF> GyroSensor3DOFRef;

/**
 * This class is the base class for all 3DOF gyro sensors.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT GyroSensor3DOF : virtual public Sensor
{
	public:

		/**
		 * Definition of a sample holding gyro sensor 3DOF measurements.
		 * Each gyro measurement value is defined in [rad / s]
		 */
		class OCEAN_DEVICES_EXPORT Gyro3DOFSample : virtual public Measurement::Sample
		{
			public:

				/**
				 * Definition of a vector holding gyro measurements.
				 * One measurement holds three different gyro values for three separated axis.
				 */
				typedef std::vector<Vector3> Measurements;

			public:

				/**
				 * Creates a new gyro sensor 3DOF sample.
				 * @param timestamp Sample timestamp
				 * @param objectIds Measurement unit object ids each id corresponds to a different gyro measurement
				 * @param measurements Sample measurements
				 */
				Gyro3DOFSample(const Timestamp& timestamp, const ObjectIds& objectIds, const Measurements& measurements);

				/**
				 * Creates a new gyro sensor 3DOF sample.
				 * @param timestamp Sample timestamp
				 * @param objectIds Measurement unit object ids each id corresponds to a different gyro measurement
				 * @param measurements Sample measurements
				 */
				Gyro3DOFSample(const Timestamp& timestamp, ObjectIds&& objectIds, Measurements&& measurements);

				/**
				 * Returns all measurement values, each value in [rad / s]
				 * @return Measurement values
				 */
				inline const Measurements& measurements() const;

			protected:

				/// Measurement values.
				Measurements measurements_;
		};

		/**
		 * Definition of a smart object reference for 3DOF gyro sensor samples.
		 */
		typedef SmartObjectRef<Gyro3DOFSample, Sample> Gyro3DOFSampleRef;

	public:

		/**
		 * Returns this device type.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_GYRO_RAW_3DOF or SENSOR_GYRO_UNBIASED_3DOF
		 * @return Device type
		 */
		static inline DeviceType deviceTypeGyroSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new 3DOF gyro sensor device.
		 * @param name The name of the gyro sensor
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_GYRO_RAW_3DOF or SENSOR_GYRO_UNBIASED_3DOF
		 */
		GyroSensor3DOF(const std::string& name, const SensorType sensorType);

		/**
		 * Destructs a 3DOF gyro sensor device.
		 */
		~GyroSensor3DOF() override;
};

inline GyroSensor3DOF::DeviceType GyroSensor3DOF::deviceTypeGyroSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_GYRO_RAW_3DOF || sensorType == SENSOR_GYRO_UNBIASED_3DOF);

	return GyroSensor3DOF::DeviceType(DEVICE_SENSOR, sensorType);
}

inline const GyroSensor3DOF::Gyro3DOFSample::Measurements& GyroSensor3DOF::Gyro3DOFSample::measurements() const
{
	return measurements_;
}

}

}

#endif // META_OCEAN_DEVICES_GYRO_SENSOR_3DOF_H
