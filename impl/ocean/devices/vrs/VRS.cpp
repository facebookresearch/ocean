// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSFactory.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

std::string nameVRSLibrary()
{
	return std::string("VRS Replay Library");
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerVRSLibrary()
{
	Ocean::Devices::VRS::VRSFactory::registerFactory();
}

bool unregisterVRSLibrary()
{
	return Ocean::Devices::VRS::VRSFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
