// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/quest/Plugin.h"
#include "ocean/media/quest/QuestLibrary.h"

#include "ocean/media/Manager.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::Quest::QuestLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Quest::Android::QuestLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
