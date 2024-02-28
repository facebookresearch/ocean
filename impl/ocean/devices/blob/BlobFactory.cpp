// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/blob/BlobFactory.h"
#include "ocean/devices/blob/BlobTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

BlobFactory::BlobFactory() :
	Factory(nameBlobLibrary())
{
	registerDevice(BlobTracker6DOF::deviceNameBlobTracker6DOF(), BlobTracker6DOF::deviceTypeBlobTracker6DOF(), InstanceFunction::createStatic(&BlobFactory::createBlobTracker6DOF));
}

bool BlobFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new BlobFactory()));
}

bool BlobFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameBlobLibrary());
}

Device* BlobFactory::createBlobTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(name == BlobTracker6DOF::deviceNameBlobTracker6DOF(), name);
	ocean_assert_and_suppress_unused(deviceType == BlobTracker6DOF::deviceTypeBlobTracker6DOF(), deviceType);

	return new BlobTracker6DOF();
}

}

}

}
