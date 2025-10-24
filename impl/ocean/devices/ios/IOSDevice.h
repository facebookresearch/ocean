/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_DEVICE_H
#define META_OCEAN_DEVICES_IOS_IOS_DEVICE_H

#include "ocean/devices/ios/IOS.h"

#include "ocean/devices/Device.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements a device for the IOS library.
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSDevice : virtual public Device
{
	public:

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		virtual const std::string& library() const;

	protected:

		/**
		 * Creates a new device by is name.
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		IOSDevice(const std::string& name, const DeviceType type);

		/**
		 * Returns the relative and unix timestamp of an Android sensor event.
		 * @param cmLogItemTimestamp The timestamp of the base class for all motion-related data objects, in seconds since the device booted
		 * @param relativeTimestamp The resulting relative timestamp
		 * @return The resulting unix timestamp
		 */
		Timestamp convertTimestamp(const double cmLogItemTimestamp, Timestamp& relativeTimestamp);

		/**
		 * Returns the timestamp converter for all android sensors.
		 * @return The timestamp converter
		 */
		static Timestamp::TimestampConverter& timestampConverter();

	protected:

		/// True, if this sensor is started.
		bool isStarted_ = false;

		/// The timestamp converter of this sensor.
		Timestamp::TimestampConverter& timestampConverter_;
};

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_DEVICE_H
