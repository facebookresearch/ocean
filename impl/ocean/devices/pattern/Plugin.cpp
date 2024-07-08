/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/pattern/Plugin.h"
#include "ocean/devices/pattern/PatternFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Pattern::PatternFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Pattern::PatternFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
