// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/UserAccount.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include <json/json.h>
	#include <cassert>
	#include <fstream>
	#include <iostream>

namespace Ocean::XRPlayground
{

UserAccount::UserAccount(std::string configPath) :
	deviceViewIds_(std::make_shared<std::vector<size_t>>())
{
	Json::Value root;
	std::ifstream ifs;
	ifs.open(configPath.c_str());

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;

	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs))
	{
		std::cerr << errs << std::endl;
	}
	else
	{
		std::cout << root << std::endl;
		name_ = root["userName"].asString();
		accessToken_ = root["accessToken"].asString();

		localizationRoiFeaturesPath = root["localizationRoiFeatures"].asString();
		classificationRoiFeaturesPath = root["classificationRoiFeatures"].asString();

		const Json::Value devices = root["devices"];
		for (int i = 0; i < devices.size(); i++)
		{
			auto deviceId = devices[i]["deviceId"].asString();
			assert(devices_.find(deviceId) == devices_.end());

			devices_[deviceId] = getDevice(
				devices[i]["deviceType"].asString(),
				deviceId,
				devices[i]["deviceName"].asString());
		}

		const Json::Value deviceViews = root["deviceViews"];
		for (int index = 0; index < deviceViews.size(); index++)
		{
			auto deviceId = deviceViews[index]["deviceId"].asString();

			assert(devices_.find(deviceId) != devices_.end());

			std::shared_ptr<DeviceView> deviceView = std::make_shared<DeviceView>(
				devices_[deviceId]);

			deviceViewIds_->push_back(deviceViewRegister_.addDeviceView(deviceView));
		}
	}
}

std::shared_ptr<std::vector<size_t>> UserAccount::deviceViewIds()
{
	return deviceViewIds_;
}

std::shared_ptr<std::unordered_map<size_t, std::string>> UserAccount::deviceViewIdToDeviceId()
{
	std::shared_ptr<std::unordered_map<size_t, std::string>> deviceIds = std::make_shared<std::unordered_map<size_t, std::string>>();

	for (auto id : (*deviceViewIds_))
	{
		(*deviceIds)[id] = deviceViewRegister_.getDeviceView(id)->device()->device()->deviceId();
	}
	return deviceIds;
}

} // namespace Ocean::XRPlayground


#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
