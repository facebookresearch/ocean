// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/VrApi.h"
#include "ocean/devices/quest/vrapi/VrApiFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

std::string nameQuestVrApiLibrary()
{
	return std::string("Quest VrApi Devices");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerQuestVrApiLibrary()
{
	VrApiFactory::registerFactory();
}

bool unregisterQuestVrApiLibrary()
{
	return VrApiFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
