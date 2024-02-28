// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/interaction/experiences/Plugin.h"
#include "ocean/interaction/experiences/ExperiencesLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Interaction::Experiences::ExperiencesLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Interaction::Experiences::ExperiencesLibrary::unregisterLibrary();
}

#endif // #if defined(OCEAN_RUNTIME_SHARED)
