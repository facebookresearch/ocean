/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/Plugin.h"
#include "ocean/devices/serialization/SerializationFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Serialization::SerializationFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Serialization::SerializationFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
