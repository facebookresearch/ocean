/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_PATTERN_DEVICE_H
#define META_OCEAN_DEVICES_PATTERN_DEVICE_H

#include "ocean/devices/pattern/Pattern.h"

#include "ocean/devices/Device.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

/**
 * This class implements a base class for all devices of the Pattern library.
 * @ingroup devicespattern
 */
class OCEAN_DEVICES_PATTERN_EXPORT PatternDevice : virtual public Device
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
		PatternDevice(const std::string& name, const DeviceType type);
};

}

}

}

#endif // META_OCEAN_DEVICES_PATTERN_DEVICE_H
