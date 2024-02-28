// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/VrApiFactory.h"
#include "ocean/devices/quest/vrapi/ControllerTracker6DOF.h"
#include "ocean/devices/quest/vrapi/FloorTracker6DOF.h"
#include "ocean/devices/quest/vrapi/HeadsetTracker6DOF.h"
#include "ocean/devices/quest/vrapi/OnDeviceRelocalizerTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

VrApiFactory::VrApiFactory() :
	Factory(nameQuestVrApiLibrary())
{
	registerDevice(ControllerTracker6DOF::deviceNameControllerTracker6DOF(), ControllerTracker6DOF::deviceTypeControllerTracker6DOF(), InstanceFunction::createStatic(&VrApiFactory::createControllerTracker6DOF));
	registerDevice(FloorTracker6DOF::deviceNameFloorTracker6DOF(), FloorTracker6DOF::deviceTypeFloorTracker6DOF(), InstanceFunction::createStatic(&VrApiFactory::createFloorTracker6DOF));
	registerDevice(HeadsetTracker6DOF::deviceNameHeadsetTracker6DOF(), HeadsetTracker6DOF::deviceTypeHeadsetTracker6DOF(), InstanceFunction::createStatic(&VrApiFactory::createHeadsetTracker6DOF));
	registerDevice(OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF(), OnDeviceRelocalizerTracker6DOF::deviceTypeOnDeviceRelocalizerTracker6DOF(), InstanceFunction::createStatic(&VrApiFactory::createOnDeviceRelocalizerTracker6DOF));
}

bool VrApiFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new VrApiFactory()));
}

bool VrApiFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameQuestVrApiLibrary());
}

void VrApiFactory::update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	for (const std::string& deviceName : {ControllerTracker6DOF::deviceNameControllerTracker6DOF(), FloorTracker6DOF::deviceNameFloorTracker6DOF(), HeadsetTracker6DOF::deviceNameHeadsetTracker6DOF(), OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF()})
	{
		const DeviceRef device = DeviceRefManager::get().device(deviceName);

		if (device)
		{
			device.force<VrApiDevice>().update(ovr, deviceType, trackedRemoteDevice, timestamp);
		}
	}
}

Device* VrApiFactory::createControllerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == ControllerTracker6DOF::deviceNameControllerTracker6DOF());
	ocean_assert(deviceType == ControllerTracker6DOF::deviceTypeControllerTracker6DOF());

	return new ControllerTracker6DOF();
}

Device* VrApiFactory::createFloorTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == FloorTracker6DOF::deviceNameFloorTracker6DOF());
	ocean_assert(deviceType == FloorTracker6DOF::deviceTypeFloorTracker6DOF());

	return new FloorTracker6DOF();
}

Device* VrApiFactory::createHeadsetTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == HeadsetTracker6DOF::deviceNameHeadsetTracker6DOF());
	ocean_assert(deviceType == HeadsetTracker6DOF::deviceTypeHeadsetTracker6DOF());

	return new HeadsetTracker6DOF();
}

Device* VrApiFactory::createOnDeviceRelocalizerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF());
	ocean_assert(deviceType == OnDeviceRelocalizerTracker6DOF::deviceTypeOnDeviceRelocalizerTracker6DOF());

	return new OnDeviceRelocalizerTracker6DOF();
}

}

}

}

}
