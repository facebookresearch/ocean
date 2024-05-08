/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/Plugin.h"
#include "ocean/devices/android/AndroidFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Android::AndroidFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Android::AndroidFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
