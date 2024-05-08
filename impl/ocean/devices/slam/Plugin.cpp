/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/slam/Plugin.h"
#include "ocean/devices/slam/SLAMFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::SLAM::SLAMFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::SLAM::SLAMFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
