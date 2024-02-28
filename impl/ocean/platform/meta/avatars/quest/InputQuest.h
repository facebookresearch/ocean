// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_QUEST_INPUT_QUEST_H
#define META_OCEAN_PLATFORM_META_AVATARS_QUEST_INPUT_QUEST_H

#include "ocean/platform/meta/avatars/Avatars.h"
#include "ocean/platform/meta/avatars/Input.h"

#include "ocean/platform/meta/quest/Device.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

#include <ovrAvatar2/System.h>

#include <VrApi.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

namespace Quest
{

/**
 * This class holds the relevant information for one Avatar.
 * This class is not thread-safe as this class is never exposed.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT InputQuest : public Input
{
	public:

		/**
		 * Creates a body tracking data object for a local user on Quest.
		 * @param ovr The OVR mobile object, must be valid
		 * @param deviceType The type of the device
		 * @param trackedRemoteDevice The current tracked remote device, must be valid
		 * @param timestamp The current timestamp in host domain
		 * @return The resulting body tracking data object
		 */
		static SharedBodyTrackingData createBodyTrackingData(ovrMobile* ovr, const Meta::Quest::Device::DeviceType deviceType, const Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp);

		/**
		 * Returns the filled input tracking state object.
		 * @param ovr OVR mobile object, must be valid
		 * @param trackedRemoteDevice The current tracked remote device, must be valid
		 * @param timestamp The current timestamp in host domain
		 * @return The filled object
		 */
		static ovrAvatar2InputTrackingState inputTrackingState(ovrMobile* ovr, const Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp);

		/**
		 * Returns the filled input control state object.
		 * @param deviceType The type of the device
		 * @param trackedRemoteDevice The current tracked remote device, must be valid
		 * @return The filled object
		 */
		static ovrAvatar2InputControlState inputControlState(const Meta::Quest::Device::DeviceType deviceType, const Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice);

		/**
		 * Returns the filled hand tracking state object.
		 * @param inputTrackingState The filled input tracking state which is used to apply some workarounds
		 * @param ovr OVR mobile object, must be valid
		 * @param timestamp The current timestamp in host domain
		 * @return The filled object
		 */
		static ovrAvatar2HandTrackingState handTrackingState(const ovrAvatar2InputTrackingState& inputTrackingState, ovrMobile* ovr, const Timestamp& timestamp);

		/**
		 * Returns the Avatar platform for a given Quest device type.
		 * @param deviceType The type of the device for which the platform will be returned
		 * @return The resulting Avatar platform
		 */
		static ovrAvatar2Platform translateDeviceType(const Meta::Quest::Device::DeviceType deviceType);
};

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_QUEST_INPUT_QUEST_H
