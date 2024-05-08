/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/Plugin.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::OpenImageLibraries::OILLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::OpenImageLibraries::OILLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
