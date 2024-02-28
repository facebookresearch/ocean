// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSFactory.h"
#include "ocean/devices/vrs/VRSGPSTracker.h"
#include "ocean/devices/vrs/VRSSceneTracker6DOF.h"
#include "ocean/devices/vrs/VRSVisualObjectSceneTracker6DOF.h"
#include "ocean/devices/vrs/VRSVisualObjectTracker6DOF.h"
#include "ocean/devices/vrs/VRSVisualTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSFactory::VRSFactory() :
	Factory(nameVRSLibrary())
{
	// nothing to do here
}

bool VRSFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new VRSFactory()));
}

bool VRSFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameVRSLibrary());
}

Device* VRSFactory::createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	if (deviceType == VRSVisualObjectTracker6DOF::deviceTypeVRSVisualObjectTracker6DOF())
	{
		return new VRSVisualObjectTracker6DOF(name);
	}

	if (deviceType == VRSVisualTracker6DOF::deviceTypeVRSVisualTracker6DOF())
	{
		return new VRSVisualTracker6DOF(name);
	}

	if (deviceType == VRSTracker6DOF::deviceTypeVRSTracker6DOF())
	{
		return new VRSTracker6DOF(name, VRSTracker6DOF::deviceTypeVRSTracker6DOF());
	}

	if (deviceType == VRSVisualObjectSceneTracker6DOF::deviceTypeVRSVisualObjectSceneTracker6DOF())
	{
		return new VRSVisualObjectSceneTracker6DOF(name);
	}

	if (deviceType == VRSVisualSceneTracker6DOF::deviceTypeVRSVisualSceneTracker6DOF())
	{
		return new VRSVisualSceneTracker6DOF(name, VRSVisualSceneTracker6DOF::deviceTypeVRSVisualSceneTracker6DOF());
	}

	if (deviceType == VRSSceneTracker6DOF::deviceTypeVRSSceneTracker6DOF())
	{
		return new VRSSceneTracker6DOF(name, VRSSceneTracker6DOF::deviceTypeVRSSceneTracker6DOF());
	}

	ocean_assert(false && "Device type is not supported!");
	return nullptr;
}

Device* VRSFactory::createGPSTracker(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(deviceType == VRSGPSTracker::deviceTypeVRSGPSTracker());

	return new VRSGPSTracker(name);
}

}

}

}
