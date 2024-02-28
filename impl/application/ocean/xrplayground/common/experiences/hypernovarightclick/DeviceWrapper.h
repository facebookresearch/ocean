// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include "SmartThings/device/Device.hpp"
	#include <memory>
	#include <string>
	#include <vector>


using facebook::cvmm::smart_things::IDevice;

namespace Ocean::XRPlayground
{

class DeviceWrapper
{
	private:
	std::shared_ptr<IDevice> device_;
	std::string deviceName_;

	public:
	DeviceWrapper(std::shared_ptr<IDevice> device, std::string deviceName);
	std::string deviceName()
	{
		return deviceName_;
	}
	std::shared_ptr<IDevice> device()
	{
		return device_;
	}
};

std::shared_ptr<DeviceWrapper> getDevice(std::string deviceType, std::string deviceId, std::string deviceName);

} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
