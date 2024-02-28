// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include "SmartThings/device/Device.hpp"
	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceWrapper.h"
	#include <memory>
	#include <string>
	#include <vector>

namespace Ocean::XRPlayground
{

class DeviceView
{
	private:
	std::shared_ptr<DeviceWrapper> device_;
	size_t deviceViewId_;

	public:
	DeviceView(std::shared_ptr<DeviceWrapper> device);
	size_t deviceViewId();
	void setDeviceViewId(size_t id);
	std::shared_ptr<DeviceWrapper> device()
	{
		return device_;
	}
};

} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
