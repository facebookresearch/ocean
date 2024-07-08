/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/Plugin.h"
#include "ocean/media/wic/WICLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::WIC::WICLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::WIC::WICLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
