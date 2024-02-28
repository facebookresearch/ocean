// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_DEVICE_H
#define META_OCEAN_PLATFORM_META_QUEST_DEVICE_H

#include "ocean/platform/meta/quest/Quest.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

/**
 * This class implements functions for Meta's Quest devices.
 * @ingroup platformmetaquest
 */
class Device
{
	public:

		/**
		 * Definition of individual device types.
		 */
		enum DeviceType : uint32_t
		{
			/// Unknown device.
			DT_UNKNOWN = 0u,
			/// Quest device, version 1.
			DT_QUEST,
			/// Quest device, version 2.
			DT_QUEST_2,
			/// Quest device, version 3.
			DT_QUEST_3,
			/// Quest Pro device.
			DT_QUEST_PRO,
			// Ventura or Panther
			DT_VENTURA
		};

	public:

		/**
		 * Returns the type of the device.
		 * @return The device type, DT_UNKNOWN if unknown
		 */
		static DeviceType deviceType();

		/**
		 * Returns the name of the device.
		 * @return The device name, "Unknown" if unknown
		 */
		static inline std::string deviceName();

		/**
		 * Returns the name of the device.
		 * @param type The device type
		 * @return The device name, "Unknown" if unknown
		 */
		static std::string deviceName(const DeviceType type);
};

inline std::string Device::deviceName()
{
	return deviceName(deviceType());
}

} // Quest

} // Meta

} // Platform

} // Ocean

#endif // META_OCEAN_PLATFORM_META_QUEST_DEVICE_H
