/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_SENSOR_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_SENSOR_H

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/devices/Sensor.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationSensor;

/**
 * Definition of a smart object reference for a Serialization sensor.
 * @see SerializationSensor.
 * @ingroup devicesserialization
 */
using SerializationSensorRef = SmartDeviceRef<SerializationSensor>;

/**
 * This class implements a Serialization sensor.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationSensor : virtual public Sensor
{
	friend class SerializationFactory;

	public:

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

	protected:

		/**
		 * Creates a new sensor object.
		 * @param name The name of the sensor
		 * @param type Major and minor device type of the device
		 */
		SerializationSensor(const std::string& name, const DeviceType& type);

		/**
		 * Destructs a new Serialization sensor object.
		 */
		~SerializationSensor() override;

	protected:

		/// True, if the sensor is started.
		bool isStarted_ = false;

		/// The name of the library of this device.
		std::string library_;
};

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_SENSOR_H
