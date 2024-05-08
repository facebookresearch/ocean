/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ACCELERATION_SENSOR_3DOF_H
#define META_OCEAN_DEVICES_ACCELERATION_SENSOR_3DOF_H

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
class AccelerationSensor3DOF;

/**
 * Definition of a smart object reference for a 3DOF acceleration sensor.
 * @see AccelerationSensor3DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<AccelerationSensor3DOF> AccelerationSensor3DOFRef;

/**
 * This class is the base class for all 3DOF acceleration sensors.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT AccelerationSensor3DOF : virtual public Sensor
{
	public:

		/**
		 * Definition of a sample holding acceleration sensor 3DOF measurements.
		 * Each acceleration measurement value is defined in [m / s^2]
		 */
		class OCEAN_DEVICES_EXPORT Acceleration3DOFSample : virtual public Measurement::Sample
		{
			public:

				/**
				 * Definition of a vector holding acceleration measurements.
				 * One measurement holds three different acceleration values for three separated axis.
				 */
				typedef std::vector<Vector3> Measurements;

			public:

				/**
				 * Creates a new acceleration sensor 3DOF sample.
				 * @param timestamp Sample timestamp
				 * @param objectIds Measurement unit object ids each id corresponds to a different acceleration measurement
				 * @param measurements Sample measurements
				 * @param metadata Optional metadata of the new sample
				 */
				Acceleration3DOFSample(const Timestamp& timestamp, const ObjectIds& objectIds, const Measurements& measurements, const Metadata& metadata = Metadata());

				/**
				 * Creates a new acceleration sensor 3DOF sample.
				 * @param timestamp Sample timestamp
				 * @param objectIds Measurement unit object ids each id corresponds to a different acceleration measurement
				 * @param measurements Sample measurements
				 * @param metadata Optional metadata of the new sample
				 */
				Acceleration3DOFSample(const Timestamp& timestamp, ObjectIds&& objectIds, Measurements&& measurements, Metadata&& metadata = Metadata());

				/**
				 * Returns all measurement values, each acceleration value in [m / s^2]
				 * @return Measurement values
				 */
				inline const Measurements& measurements() const;

			protected:

				/// Measurement values.
				Measurements measurements_;
		};

		/**
		 * Definition of a smart object reference for 3DOF acceleration sensor samples.
		 */
		typedef SmartObjectRef<Acceleration3DOFSample, Sample> Acceleration3DOFSampleRef;

	public:

		/**
		 * Returns this device type.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 * @return Device type
		 */
		static inline DeviceType deviceTypeAccelerationSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new 3DOF acceleration sensor device.
		 * @param name The name of the acceleration sensor, must be valid
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 */
		AccelerationSensor3DOF(const std::string& name, const SensorType sensorType);

		/**
		 * Destructs a 3DOF acceleration sensor device.
		 */
		~AccelerationSensor3DOF() override;
};

inline AccelerationSensor3DOF::DeviceType AccelerationSensor3DOF::deviceTypeAccelerationSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_ACCELERATION_3DOF || sensorType == SENSOR_LINEAR_ACCELERATION_3DOF);

	return AccelerationSensor3DOF::DeviceType(DEVICE_SENSOR, sensorType);
}

inline const AccelerationSensor3DOF::Acceleration3DOFSample::Measurements& AccelerationSensor3DOF::Acceleration3DOFSample::measurements() const
{
	return measurements_;
}

}

}

#endif // META_OCEAN_DEVICES_ACCELERATION_SENSOR_3DOF_H
