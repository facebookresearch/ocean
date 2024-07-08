/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/Plugin.h"
#include "ocean/devices/arcore/ACFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::ARCore::ACFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::ARCore::ACFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
