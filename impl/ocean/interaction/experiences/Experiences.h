/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCES_H
#define META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCES_H

#include "ocean/interaction/Interaction.h"

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

/**
 * @ingroup interaction
 * @defgroup interactionexperiences Ocean Interaction Experiences Library
 * @{
 * The Ocean Interaction Experiences Library allows to implement experiences via the interface plugin mechanism.
 * @}
 */

/**
 * @namespace Ocean::Interaction::Experiences Namespace of the Interaction Experiences library.<p>
 * The Namespace Ocean::Interaction::Experiences is used in the entire Ocean Interaction Experiences Library.
 */

// Defines OCEAN_INTERACTION_EX_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_INTERACTION_EXPERIENCES_EXPORT
		#define OCEAN_INTERACTION_EXPERIENCES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_INTERACTION_EXPERIENCES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_INTERACTION_EXPERIENCES_EXPORT
#endif

/**
 * Returns the name of this interaction library.
 * @ingroup interactionexperiences
 */
OCEAN_INTERACTION_EXPERIENCES_EXPORT std::string nameExperiencesLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this Content interaction library at the global interaction manager.
 * This function calls ContentLibrary::registerLibrary() only.
 * @ingroup interactionexperiences
 */
void registerExperiencesLibrary();

/**
 * Unregisters this Content interaction library at the global interaction manager.
 * This function calls ContentLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup interactionexperiences
 */
bool unregisterExperiencesLibrary();

#endif // OCEAN_RUNTIME_STATIC

} // namespace Experiences

} // namespace Interaction

} // namespace Ocean

#endif // META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCES_H
