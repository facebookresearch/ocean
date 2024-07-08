/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/experiences/Experiences.h"
#include "ocean/interaction/experiences/ExperiencesLibrary.h"

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

std::string nameExperiencesLibrary()
{
	return std::string("Experiences interaction library");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerExperiencesLibrary()
{
	ExperiencesLibrary::registerLibrary();
}

bool unregisterExperiencesLibrary()
{
	return ExperiencesLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace Experiences

} // namespace Interaction

} // namespace Ocean
