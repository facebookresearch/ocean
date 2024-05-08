// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/devices/blob/Plugin.h"
#include "ocean/devices/blob/BlobFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Blob::BlobFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Blob::BlobFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
