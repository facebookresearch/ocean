/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
