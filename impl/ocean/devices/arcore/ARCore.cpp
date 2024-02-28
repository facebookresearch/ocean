// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/arcore/ARCore.h"
#include "ocean/devices/arcore/ACFactory.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

const std::string& nameARCoreLibrary()
{
	static const std::string name("ARCore tracking library");
	return name;
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerARCoreLibrary()
{
	Ocean::Devices::ARCore::ACFactory::registerFactory();
}

bool unregisterARCoreLibrary()
{
	return Ocean::Devices::ARCore::ACFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
