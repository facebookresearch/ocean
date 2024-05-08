/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/Plugin.h"
#include "ocean/media/android/ALibrary.h"

#include "ocean/media/Manager.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::Android::ALibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::Android::ALibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
