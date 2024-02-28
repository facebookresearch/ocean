// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_RMV_DEVICE_H
#define META_OCEAN_DEVICES_RMV_DEVICE_H

#include "ocean/devices/rmv/RMV.h"

#include "ocean/devices/Device.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

/**
 * This class implements a device for the RMV library.
 * @ingroup devicesrmv
 */
class OCEAN_DEVICES_RMV_EXPORT RMVDevice : virtual public Device
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
		RMVDevice(const std::string& name, const DeviceType type);
};

}

}

}

#endif // META_OCEAN_DEVICES_RMV_DEVICE_H
