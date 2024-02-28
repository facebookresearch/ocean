// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceView.h"
	#include <memory>
	#include <string>
	#include <unordered_map>
	#include <vector>


namespace Ocean::XRPlayground
{

class DeviceViewRegister
{
	private:
	size_t deviceViewId_;
	std::unordered_map<size_t, std::shared_ptr<DeviceView>> register_;

	size_t nextDeviceViewId();

	public:
	DeviceViewRegister();
	size_t addDeviceView(std::shared_ptr<DeviceView> deviceView);
	size_t numDeviceViews();
	std::shared_ptr<DeviceView> getDeviceView(size_t id);
};


} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
