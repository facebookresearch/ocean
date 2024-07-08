/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/Plugin.h"
#include "ocean/devices/ios/IOSFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::IOS::IOSFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::IOS::IOSFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
