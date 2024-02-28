// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudFactory.h"
#include "ocean/devices/cloud/CloudPerFrameRelocalizerTracker6DOF.h"
#include "ocean/devices/cloud/CloudPlacementsTracker6DOF.h"
#include "ocean/devices/cloud/CloudRelocalizerECEFTracker6DOF.h"
#include "ocean/devices/cloud/CloudRelocalizerFusedGeoTracker6DOF.h"
#include "ocean/devices/cloud/CloudRelocalizerTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudFactory::CloudFactory() :
	Factory(nameCloudLibrary())
{
	registerDevices();
}

bool CloudFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new CloudFactory()));
}

bool CloudFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameCloudLibrary());
}

void CloudFactory::registerDevices()
{
	registerDevice(CloudPerFrameRelocalizerTracker6DOF::deviceNameCloudPerFrameRelocalizerTracker6DOF(), CloudPerFrameRelocalizerTracker6DOF::deviceTypeCloudPerFrameRelocalizerTracker6DOF(), InstanceFunction::createStatic(createCloudPerFrameRelocalizerTracker6DOF));

	registerDevice(CloudRelocalizerTracker6DOF::deviceNameCloudRelocalizerTracker6DOF(true), CloudRelocalizerTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF(), InstanceFunction::createStatic(createCloudRelocalizerTracker6DOF));
	registerDevice(CloudRelocalizerTracker6DOF::deviceNameCloudRelocalizerTracker6DOF(false), CloudRelocalizerTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF(), InstanceFunction::createStatic(createCloudRelocalizerTracker6DOF));

	registerDevice(CloudPlacementsTracker6DOF::deviceNameCloudPlacementsTracker6DOF(), CloudPlacementsTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF(), InstanceFunction::createStatic(createCloudPlacementsTracker6DOF));

	registerDevice(CloudRelocalizerECEFTracker6DOF::deviceNameCloudRelocalizerECEFTracker6DOF(), CloudRelocalizerECEFTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF(), InstanceFunction::createStatic(createCloudRelocalizerECEFTracker6DOF));

	registerDevice(CloudRelocalizerFusedGeoTracker6DOF::deviceNameCloudRelocalizerFusedGeoTracker6DOF(), CloudRelocalizerFusedGeoTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF(), InstanceFunction::createStatic(createCloudRelocalizerFusedGeoTracker6DOF));
}

Device* CloudFactory::createCloudPerFrameRelocalizerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == CloudPerFrameRelocalizerTracker6DOF::deviceNameCloudPerFrameRelocalizerTracker6DOF());
	ocean_assert(deviceType == CloudPerFrameRelocalizerTracker6DOF::deviceTypeCloudPerFrameRelocalizerTracker6DOF());

	return new CloudPerFrameRelocalizerTracker6DOF();
}

Device* CloudFactory::createCloudRelocalizerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(deviceType == CloudRelocalizerTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF());

	if (name == CloudRelocalizerTracker6DOF::deviceNameCloudRelocalizerTracker6DOF(true))
	{
		return new CloudRelocalizerTracker6DOF(true);
	}
	else
	{
		ocean_assert(name == CloudRelocalizerTracker6DOF::deviceNameCloudRelocalizerTracker6DOF(false));
		return new CloudRelocalizerTracker6DOF(false);
	}
}

Device* CloudFactory::createCloudPlacementsTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == CloudPlacementsTracker6DOF::deviceNameCloudPlacementsTracker6DOF());
	ocean_assert(deviceType == CloudPlacementsTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF());

	return new CloudPlacementsTracker6DOF();
}

Device* CloudFactory::createCloudRelocalizerECEFTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == CloudRelocalizerECEFTracker6DOF::deviceNameCloudRelocalizerECEFTracker6DOF());
	ocean_assert(deviceType == CloudRelocalizerECEFTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF());

	return new CloudRelocalizerECEFTracker6DOF();
}

Device* CloudFactory::createCloudRelocalizerFusedGeoTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == CloudRelocalizerFusedGeoTracker6DOF::deviceNameCloudRelocalizerFusedGeoTracker6DOF());
	ocean_assert(deviceType == CloudRelocalizerFusedGeoTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF());

	return new CloudRelocalizerFusedGeoTracker6DOF();
}

} // namespace Cloud

} // namespace Devices

} // namespace Ocean
