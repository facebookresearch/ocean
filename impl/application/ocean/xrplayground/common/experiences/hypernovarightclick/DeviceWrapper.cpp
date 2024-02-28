// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceWrapper.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include "SmartThings/device/Ecobee3Lite.hpp"
	#include "SmartThings/device/JetBotRobotVacuum.hpp"
	#include "SmartThings/device/KasaSmartLightBulb.hpp"
	#include "SmartThings/device/KasaSmartLightStrip.hpp"
	#include "SmartThings/device/KasaSmartPlugMini.hpp"
	#include "SmartThings/device/SonosOneGen2.hpp"

using facebook::cvmm::smart_things::Ecobee3Lite;
using facebook::cvmm::smart_things::JetBotRobotVacuum;
using facebook::cvmm::smart_things::KasaSmartLightBulb;
using facebook::cvmm::smart_things::KasaSmartLightStrip;
using facebook::cvmm::smart_things::KasaSmartPlugMini;
using facebook::cvmm::smart_things::SonosOneGen2;

namespace Ocean::XRPlayground
{

DeviceWrapper::DeviceWrapper(std::shared_ptr<IDevice> device, std::string deviceName) :
	device_(device), deviceName_(deviceName)
{
}

std::shared_ptr<DeviceWrapper> getDevice(std::string deviceType, std::string deviceId, std::string deviceName)
{
	if (deviceType == "Ecobee3Lite")
	{
		return std::make_shared<DeviceWrapper>(std::make_shared<Ecobee3Lite>(deviceId), deviceName);
	}
	else if (deviceType == "JetBotRobotVacuum")
	{
		return std::make_shared<DeviceWrapper>(std::make_shared<JetBotRobotVacuum>(deviceId), deviceName);
	}
	else if (deviceType == "KasaSmartLightBulb")
	{
		return std::make_shared<DeviceWrapper>(std::make_shared<KasaSmartLightBulb>(deviceId), deviceName);
	}
	else if (deviceType == "KasaSmartLightStrip")
	{
		return std::make_shared<DeviceWrapper>(std::make_shared<KasaSmartLightStrip>(deviceId), deviceName);
	}
	else if (deviceType == "KasaSmartPlugMini")
	{
		return std::make_shared<DeviceWrapper>(std::make_shared<KasaSmartPlugMini>(deviceId), deviceName);
	}
	else if (deviceType == "SonosOneGen2")
	{
		return std::make_shared<DeviceWrapper>(std::make_shared<SonosOneGen2>(deviceId), deviceName);
	}
	else
	{
		return std::shared_ptr<DeviceWrapper>(nullptr);
	}
}


} // namespace Ocean::XRPlayground


#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
