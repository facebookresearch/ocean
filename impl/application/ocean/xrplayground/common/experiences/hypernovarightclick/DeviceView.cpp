// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/DeviceView.h"
	#include "SmartThings/device/KasaSmartPlugMini.hpp"
	#include "SmartThings/device/SonosOneGen2.hpp"

using facebook::cvmm::smart_things::KasaSmartPlugMini;
using facebook::cvmm::smart_things::SonosOneGen2;


namespace Ocean::XRPlayground
{

DeviceView::DeviceView(std::shared_ptr<DeviceWrapper> device) :
	device_(device), deviceViewId_(0)
{
}

size_t DeviceView::deviceViewId()
{
	return deviceViewId_;
}

void DeviceView::setDeviceViewId(size_t id)
{
	deviceViewId_ = id;
}


} // namespace Ocean::XRPlayground


#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
