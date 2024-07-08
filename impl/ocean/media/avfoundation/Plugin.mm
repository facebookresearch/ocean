/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/Plugin.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::AVFoundation::AVFLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::AVFoundation::AVFLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
