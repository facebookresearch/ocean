/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SLAM_FACTORY_H
#define META_OCEAN_DEVICES_SLAM_FACTORY_H

#include "ocean/devices/slam/SLAM.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

/**
 * This class implements a device factory for the SLAM feature based tracking system.
 * @ingroup devicesslam
 */
class OCEAN_DEVICES_SLAM_EXPORT SLAMFactory : public Factory
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
		 * This unregistration should be done after all created devices have been released.
		 * @return True, if this factory chould be unregistered
		 */
		static bool unregisterFactory();

	private:

		/**
		 * Creates a new factory.
		 */
		SLAMFactory();

		/**
		 * Creates a SLAMTracker6DOF tracker object.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return New created device object.
		 */
		static Device* createSLAMTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_SLAM_FACTORY_H
