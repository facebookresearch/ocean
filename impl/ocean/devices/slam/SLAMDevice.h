/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SLAM_DEVICE_H
#define META_OCEAN_DEVICES_SLAM_DEVICE_H

#include "ocean/devices/slam/SLAM.h"

#include "ocean/devices/Device.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

/**
 * This class implements a device for the SLAM library.
 * @ingroup devicesslam
 */
class OCEAN_DEVICES_SLAM_EXPORT SLAMDevice : virtual public Device
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
		SLAMDevice(const std::string& name, const DeviceType type);
};

}

}

}

#endif // META_OCEAN_DEVICES_SLAM_DEVICE_H
