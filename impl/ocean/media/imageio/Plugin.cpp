/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/Plugin.h"
#include "ocean/media/imageio/IIOLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::ImageIO::IIOLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::ImageIO::IIOLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
