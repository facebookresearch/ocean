/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
