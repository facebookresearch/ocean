/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_FACTORY_H
#define META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_FACTORY_H

#include "ocean/devices/mapbuilding/MapBuilding.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

/**
 * This class implements a device factory for the MapBuilding feature-based tracking system.
 * @ingroup devicesmapbuilding
 */
class OCEAN_DEVICES_MAPBUILDING_EXPORT MapBuildingFactory : public Factory
{
	public:

		/**
		 * Registers this factory at the manager.
		 * Beware: Unregister this factory if not needed anymore.
		 * @return True, if this factory hasn't been registered before
		 */
		static bool registerFactory();

		/**
		 * Unregisters this factory at the manger.
		 * This un-registration should be done after all created devices have been released.
		 * @return True, if this factory could be unregistered
		 */
		static bool unregisterFactory();

	private:

		/**
		 * Creates a new factory.
		 */
		MapBuildingFactory();

		/**
		 * Creates a tracker object.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return New created device object.
		 */
		static Device* createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_FACTORY_H
