/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_ACCELERATION_SENSOR_3DOF_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_ACCELERATION_SENSOR_3DOF_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationSensor.h"

#include "ocean/devices/AccelerationSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationAccelerationSensor3DOF;

/**
 * Definition of a smart object reference for a Serialization 3DOF acceleration sensor.
 * @see SerializationAccelerationSensor3DOF.
 * @ingroup devicesserialization
 */
using SerializationAccelerationSensor3DOFRef = SmartDeviceRef<SerializationAccelerationSensor3DOF>;

/**
 * This class implements a Serialization 3DOF acceleration sensor.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationAccelerationSensor3DOF :
	virtual public AccelerationSensor3DOF,
	virtual public SerializationSensor
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param measurements The acceleration measurements to forward
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(Acceleration3DOFSample::Measurements&& measurements, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this sensor.
		 * @param sensorType The sensor type
		 * @return Sensor type
		 */
		static inline DeviceType deviceTypeSerializationAccelerationSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new Serialization 3DOF acceleration sensor object.
		 * @param name The name of the sensor
		 * @param sensorType The sensor type
		 */
		SerializationAccelerationSensor3DOF(const std::string& name, const SensorType sensorType);

		/**
		 * Destructs a Serialization 3DOF acceleration sensor object.
		 */
		~SerializationAccelerationSensor3DOF() override;
};

inline SerializationAccelerationSensor3DOF::DeviceType SerializationAccelerationSensor3DOF::deviceTypeSerializationAccelerationSensor3DOF(const SensorType sensorType)
{
	return deviceTypeAccelerationSensor3DOF(sensorType);
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_ACCELERATION_SENSOR_3DOF_H
