/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/Plugin.h"
#include "ocean/media/mediafoundation/MFLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::MediaFoundation::MFLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::MediaFoundation::MFLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
