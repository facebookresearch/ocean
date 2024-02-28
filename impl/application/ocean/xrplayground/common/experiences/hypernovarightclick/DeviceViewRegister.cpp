// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceViewRegister.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

namespace Ocean::XRPlayground
{

size_t DeviceViewRegister::nextDeviceViewId()
{
	return deviceViewId_++;
}

DeviceViewRegister::DeviceViewRegister() :
	deviceViewId_(0) {}

size_t DeviceViewRegister::addDeviceView(std::shared_ptr<DeviceView> deviceView)
{
	size_t deviceViewId = this->nextDeviceViewId();
	deviceView->setDeviceViewId(deviceViewId);
	register_[deviceViewId] = deviceView;
	return deviceViewId;
}


size_t DeviceViewRegister::numDeviceViews()
{
	return register_.size();
}

std::shared_ptr<DeviceView> DeviceViewRegister::getDeviceView(size_t id)
{
	return register_[id];
}

} // namespace Ocean::XRPlayground


#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
