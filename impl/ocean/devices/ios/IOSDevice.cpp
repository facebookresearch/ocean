/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSDevice.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSDevice::IOSDevice(const std::string& name, const DeviceType type) :
	Device(name, type),
	timestampConverter_(timestampConverter())
{
	// nothing to do here
}

const std::string& IOSDevice::library() const
{
	return nameIOSLibrary();
}

Timestamp::TimestampConverter& IOSDevice::timestampConverter()
{
	static Timestamp::TimestampConverter timestampConverter(Timestamp::TimestampConverter::TD_UPTIME_RAW);

	return timestampConverter;
}

Timestamp IOSDevice::convertTimestamp(const double cmLogItemTimestamp, Timestamp& relativeTimestamp)
{
	// The timestamp is the amount of time in seconds since the device booted.
	ocean_assert(timestampConverter_.timeDomain() == Timestamp::TimestampConverter::TD_UPTIME_RAW);

#ifdef OCEAN_DEBUG
	double debugDistance;
	if (!timestampConverter_.isWithinRange(Timestamp::seconds2nanoseconds(cmLogItemTimestamp), 0.1, &debugDistance))
	{
		Log::debug() << "IOSDevice: Timestamp is not within range of 0.1 seconds, actual distance: " << debugDistance << "s";
	}
#endif

	relativeTimestamp = Timestamp(cmLogItemTimestamp);

	return timestampConverter_.toUnix(cmLogItemTimestamp);
}

}

}

}
