/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/Plugin.h"
#include "ocean/devices/arkit/AKFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::ARKit::AKFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::ARKit::AKFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
