// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/Plugin.h"
#include "ocean/devices/quest/vrapi/VrApiFactory.h"

#include "ocean/base/String.h"

#include <VrApi.h>

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Ouclus::VrApi::VrApiFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Ouclus::VrApi::VrApiFactory::unregisterFactory();
}

const char* pluginVersion()
{
	static const std::string ouclusVersion(std::string("Quest VrApi Version ") + Ocean::String::toAString(VRAPI_MAJOR_VERSION)
		+ std::string(".") + Ocean::String::toAString(VRAPI_MINOR_VERSION));

	return ouclusVersion.c_str();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
