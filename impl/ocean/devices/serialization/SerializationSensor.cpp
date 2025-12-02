/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationSensor.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

std::string nameSerializationLibrary();

SerializationSensor::SerializationSensor(const std::string& name, const DeviceType& type) :
	Device(name, type),
	Measurement(name, type),
	Sensor(name, type)
{
	library_ = nameSerializationLibrary();
}

SerializationSensor::~SerializationSensor()
{
	// nothing to do here
}

bool SerializationSensor::isStarted() const
{
	return isStarted_;
}

bool SerializationSensor::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	isStarted_ = true;

	return isStarted_;
}

bool SerializationSensor::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	isStarted_ = false;

	return true;
}

const std::string& SerializationSensor::library() const
{
	return library_;
}

}

}

}
