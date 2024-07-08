/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
