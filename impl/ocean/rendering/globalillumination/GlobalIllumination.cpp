// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIEngine.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

const std::string& globalIlluminationEngineName()
{
	static const std::string name = "Global Illumination";

	return name;
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerGlobalIlluminationEngine()
{
	GIEngine::registerEngine();
}

bool unregisterGlobalIlluminationEngine()
{
	return GIEngine::unregisterEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
