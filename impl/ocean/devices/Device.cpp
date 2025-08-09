/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Device.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Sensor.h"
#include "ocean/devices/Tracker.h"

namespace Ocean
{

namespace Devices
{

Device::DeviceType Device::DeviceType::translateDeviceType(const std::string& majorType, const std::string& minorType)
{
	const MajorType majorTypeValue = translateMajorType(majorType);
	MinorType minorTypeValue = MINOR_INVALID;

	switch (majorTypeValue)
	{
		case DEVICE_SENSOR:
			minorTypeValue = MinorType(Sensor::translateSensorType(minorType));
			break;

		case DEVICE_TRACKER:
			minorTypeValue = MinorType(Tracker::translateTrackerType(minorType));
			break;

		default:
			break;
	}

	return DeviceType(majorTypeValue, minorTypeValue);
}

Device::Device(const std::string& name, const DeviceType type) :
	deviceName(name),
	deviceType(type),
	deviceIsValid(true)
{
	// nothing to do here
}

Device::~Device()
{
	// nothing to do here
}

bool Device::isStarted() const
{
	return true;
}

bool Device::isExclusive() const
{
	return !DeviceRefManager::get().isExclusive(this);
}

bool Device::start()
{
	return true;
}

bool Device::pause()
{
	return false;
}

bool Device::stop()
{
	return true;
}

bool Device::setParameter(const std::string& name, const Value& /*value*/)
{
	Log::warning() << "Unknown device parameter: \"" << name << "\".";
	return false;
}

bool Device::parameter(const std::string& name, Value& /*value*/)
{
	Log::warning() << "Unknown device parameter: \"" << name << "\".";
	return false;
}

std::string Device::translateMajorType(const MajorType majorType)
{
	switch (majorType)
	{
		case DEVICE_INVALID:
			return std::string("DEVICE_INVALID");

		case DEVICE_MEASUREMENT:
			return std::string("DEVICE_MEASUREMENT");

		case DEVICE_SENSOR:
			return std::string("DEVICE_SENSOR");

		case DEVICE_TRACKER:
			return std::string("DEVICE_TRACKER");
	}

	ocean_assert(false && "Unknown major type!");
	return std::string("DEVICE_INVALID");
}

Device::MajorType Device::translateMajorType(const std::string& majorType)
{
	if (majorType == "DEVICE_INVALID")
		return DEVICE_INVALID;

	if (majorType == "DEVICE_MEASUREMENT")
		return DEVICE_MEASUREMENT;

	if (majorType == "DEVICE_SENSOR")
		return DEVICE_SENSOR;

	if (majorType == "DEVICE_TRACKER")
		return DEVICE_TRACKER;

	ocean_assert(false && "Unknown major type!");
	return DEVICE_INVALID;
}

}

}
