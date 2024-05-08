/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/rmv/Plugin.h"
#include "ocean/devices/rmv/RMVFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::RMV::RMVFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::RMV::RMVFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
