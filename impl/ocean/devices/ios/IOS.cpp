// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSFactory.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

const std::string& nameIOSLibrary()
{
	static const std::string name("IOS sensor library");
	return name;
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerIOSLibrary()
{
	Ocean::Devices::IOS::IOSFactory::registerFactory();
}

bool unregisterIOSLibrary()
{
	return Ocean::Devices::IOS::IOSFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
