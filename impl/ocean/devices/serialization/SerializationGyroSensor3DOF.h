/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GYRO_SENSOR_3DOF_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GYRO_SENSOR_3DOF_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationSensor.h"

#include "ocean/devices/GyroSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationGyroSensor3DOF;

/**
 * Definition of a smart object reference for a Serialization 3DOF gyro sensor.
 * @see SerializationGyroSensor3DOF.
 * @ingroup devicesserialization
 */
using SerializationGyroSensor3DOFRef = SmartDeviceRef<SerializationGyroSensor3DOF>;

/**
 * This class implements a Serialization 3DOF gyro sensor.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationGyroSensor3DOF :
	virtual public GyroSensor3DOF,
	virtual public SerializationSensor
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param measurements The gyro measurements to forward
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(Gyro3DOFSample::Measurements&& measurements, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this sensor.
		 * @param sensorType The sensor type
		 * @return Sensor type
		 */
		static inline DeviceType deviceTypeSerializationGyroSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new Serialization 3DOF gyro sensor object.
		 * @param name The name of the sensor
		 * @param sensorType The sensor type
		 */
		SerializationGyroSensor3DOF(const std::string& name, const SensorType sensorType);

		/**
		 * Destructs a Serialization 3DOF gyro sensor object.
		 */
		~SerializationGyroSensor3DOF() override;
};

inline SerializationGyroSensor3DOF::DeviceType SerializationGyroSensor3DOF::deviceTypeSerializationGyroSensor3DOF(const SensorType sensorType)
{
	return deviceTypeGyroSensor3DOF(sensorType);
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GYRO_SENSOR_3DOF_H
