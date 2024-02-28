// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
