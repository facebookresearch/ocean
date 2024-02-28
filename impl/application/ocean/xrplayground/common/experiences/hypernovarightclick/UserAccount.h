// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST


	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceViewRegister.h"
	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceWrapper.h"
	#include <memory>
	#include <string>
	#include <unordered_map>


namespace Ocean::XRPlayground
{

class UserAccount
{
	private:
	std::string name_;
	std::string accessToken_;
	std::unordered_map<std::string, std::shared_ptr<DeviceWrapper>> devices_;
	DeviceViewRegister deviceViewRegister_;
	std::shared_ptr<std::vector<size_t>> deviceViewIds_;

	public:
	std::string localizationRoiFeaturesPath;
	std::string classificationRoiFeaturesPath;

	UserAccount(std::string configPath);
	std::shared_ptr<std::vector<size_t>> deviceViewIds();
	std::shared_ptr<std::unordered_map<size_t, std::string>> deviceViewIdToDeviceId();
	std::unordered_map<std::string, std::shared_ptr<DeviceWrapper>>& devices()
	{
		return devices_;
	}
	std::string accessToken()
	{
		return accessToken_;
	}
};


} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
