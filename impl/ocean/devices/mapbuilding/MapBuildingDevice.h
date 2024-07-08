/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_DEVICE_H
#define META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_DEVICE_H

#include "ocean/devices/mapbuilding/MapBuilding.h"

#include "ocean/devices/Device.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

/**
 * This class implements a base class for all devices of the MapBuilding library.
 * @ingroup devicesmapbuilding
 */
class OCEAN_DEVICES_MAPBUILDING_EXPORT MapBuildingDevice : virtual public Device
{
	public:

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

	protected:

		/**
		 * Creates a new device by is name.
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		MapBuildingDevice(const std::string& name, const DeviceType type);
};

}

}

}

#endif // META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_DEVICE_H
