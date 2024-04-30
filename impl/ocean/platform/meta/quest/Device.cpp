// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/Device.h"

#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

Device::DeviceType Device::deviceType()
{
	static_assert(DT_QUEST_END < invalidQuestDeviceValue_, "Invalid device type!");

	std::string deviceName;
	if (!Platform::Android::Utilities::systemPropertyValue("ro.product.model", deviceName))
	{
		Log::error() << "Failed to read the 'ro.product.model' system property";

		return DT_UNKNOWN;
	}

	deviceName = String::toLower(deviceName);

	if (deviceName == "quest")
	{
		return DT_QUEST;
	}

	if (deviceName == "quest 2")
	{
		return DT_QUEST_2;
	}

	if (deviceName == "quest 3")
	{
		return DT_QUEST_3;
	}

	if (deviceName == "quest pro")
	{
		return DT_QUEST_PRO;
	}

#ifdef OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME

	const uint32_t externalDeviceType = PlatformMetaDevice_externalDeviceType(deviceName);

	if (externalDeviceType == invalidQuestDeviceValue_)
	{
		return DT_UNKNOWN;
	}

	return Device::DeviceType(externalDeviceType);

#else

	Log::error() << "The type of the Meta device could not be determined, unknown model name '" << deviceName << "'";

	ocean_assert(false && "This should never happen!");

	return DT_UNKNOWN;

#endif // OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME
}

std::string Device::deviceName(const DeviceType deviceType)
{
	static_assert(DT_QUEST_END < invalidQuestDeviceValue_, "Invalid device type!");

	switch (deviceType)
	{
		case DT_QUEST:
			return std::string("Quest");

		case DT_QUEST_2:
			return std::string("Quest 2");

		case DT_QUEST_3:
			return std::string("Quest 3");

		case DT_QUEST_PRO:
			return std::string("Quest Pro");

		case DT_UNKNOWN:
			return std::string("Unknown");

		case DT_QUEST_END:
			break;

#ifdef OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME
		default:
			return PlatformMetaDevice_externalDeviceName(deviceType);
#endif
	}

	ocean_assert(false && "Invalid device type!");
	return std::string("Unknown");
}

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
