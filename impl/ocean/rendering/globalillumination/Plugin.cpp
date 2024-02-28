// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/globalillumination/Plugin.h"
#include "ocean/rendering/globalillumination/GIEngine.h"

#include <string>

bool pluginLoad()
{
	Ocean::Rendering::GlobalIllumination::GIEngine::registerEngine();

	return true;
}

bool pluginUnload()
{
	return Ocean::Rendering::GlobalIllumination::GIEngine::unregisterEngine();
}
