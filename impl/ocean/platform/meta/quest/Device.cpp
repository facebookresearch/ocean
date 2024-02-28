// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/Device.h"

#include <vros/sys/VrDeviceManager.h>

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
	std::shared_ptr<OSSDK::VrDevice::v6::IVrDeviceManager> vrDeviceManager = OSSDK::VrDevice::v6::createVrDeviceManager();

	if (vrDeviceManager != nullptr)
	{
		const std::string vrDeviceType = String::toLower(vrDeviceManager->getDeviceType().c_str());

		if (!vrDeviceType.empty())
		{
			if (vrDeviceType == "monterey")
			{
				return DT_QUEST;
			}
			else if (vrDeviceType == "hollywood")
			{
				return DT_QUEST_2;
			}
			else if (vrDeviceType == "seacliff")
			{
				return DT_QUEST_PRO;
			}
			else if (vrDeviceType == "eureka865" || vrDeviceType == "eureka")
			{
				return DT_QUEST_3;
			}
			else if (vrDeviceType == "panther")
			{
				return DT_VENTURA;
			}
			else
			{
				Log::debug() << "Received the unknown device string '" << vrDeviceType << "'";
			}
		}
	}
	else
	{
		Log::error() << "No access to the device manager, is permission requested via '<uses-native-library android:name=\"libossdk.oculus.so\" android:required=\"true\" />'?";
	}

	Log::error() << "The type of the Oculus device could not be determined";

	ocean_assert(false && "This should never happen!");

	return DT_UNKNOWN;
}

std::string Device::deviceName(const DeviceType type)
{
	switch (type)
	{
		case DT_QUEST:
			return std::string("Quest");

		case DT_QUEST_2:
			return std::string("Quest 2");

		case DT_QUEST_3:
			return std::string("Quest 3");

		case DT_QUEST_PRO:
			return std::string("Quest Pro");

		case DT_VENTURA:
			return std::string("Ventura");

		case DT_UNKNOWN:
			return std::string("Unknown");
	}

	ocean_assert(false && "Invalid device type!");
	return std::string("Unknown");
}

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
