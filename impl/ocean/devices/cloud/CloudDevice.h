// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_DEVICE_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_DEVICE_H

#include "ocean/devices/cloud/Cloud.h"

#include "ocean/devices/Device.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements a device for the Cloud library.
 * @ingroup devicescloud
 */
class OCEAN_DEVICES_CLOUD_EXPORT CloudDevice : virtual public Device
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
		CloudDevice(const std::string& name, const DeviceType type);
};

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_DEVICE_H
