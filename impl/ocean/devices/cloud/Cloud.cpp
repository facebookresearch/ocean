// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

const std::string& nameCloudLibrary()
{
	static const std::string name("Cloud tracking library");
	return name;
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerCloudLibrary()
{
	Ocean::Devices::Cloud::CloudFactory::registerFactory();
}

bool unregisterCloudLibrary()
{
	return Ocean::Devices::Cloud::CloudFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
