// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_QUEST_FACTORY_H
#define META_OCEAN_DEVICES_QUEST_QUEST_FACTORY_H

#include "ocean/devices/quest/Quest.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

/**
 * This class implements a device factory for the Quest devices.
 * @ingroup devicesquest
 */
class OCEAN_DEVICES_QUEST_EXPORT QuestFactory : public Factory
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
		QuestFactory();

		/**
		 * Creates the GPS tracker for Quest devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 * @return The new 6DOF tracker
		 */
		static Device* createQuestGPSTracker(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_QUEST_FACTORY_H
