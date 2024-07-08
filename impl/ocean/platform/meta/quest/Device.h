/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

			/// Exclusive end of device types.
			DT_QUEST_END
		};

	protected:

		/// Definition of an invalid enum value for quest devices.
		static constexpr uint32_t invalidQuestDeviceValue_ = 1024u;

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
		 * Returns the name of a device.
		 * @param deviceType The device type for which the name will be returned
		 * @return The device name, "Unknown" if unknown
		 */
		static std::string deviceName(const DeviceType deviceType);
};

inline std::string Device::deviceName()
{
	return deviceName(deviceType());
}

#ifdef OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME

/**
 * Returns the type of the device.
 * @param productModel The product model for which the enum type will be returned
 * @param productName The product name for which the enum type will be returned
 * @return The device type, DT_UNKNOWN if unknown
 */
uint32_t PlatformMetaDevice_externalDeviceType(const std::string& productModel, const std::string& productName);

/**
 * Returns the name of a device which is based on external knowledge.
 * @param deviceType The device type for which the name will be returned
 * @return The device name, "Unknown" if unknown
 */
std::string PlatformMetaDevice_externalDeviceName(const uint32_t deviceType);

#endif // OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME

} // Quest

} // Meta

} // Platform

} // Ocean

#endif // META_OCEAN_PLATFORM_META_QUEST_DEVICE_H
