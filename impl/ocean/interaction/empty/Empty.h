/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EPY_EMPTY_H
#define META_OCEAN_INTERACTION_EPY_EMPTY_H

#include "ocean/base/Base.h"

#include "ocean/interaction/Interaction.h"

namespace Ocean
{

namespace Interaction
{

namespace Empty
{

/**
 * @ingroup interaction
 * @defgroup interactionepy Ocean Interaction Empty Demo Library
 * @{
 * The Ocean Interaction Empty Demo Library gives an exampl how to implement an interaction library for the Ocean framework.<br>
 * @}
 */

/**
 * @namespace Ocean::Interaction::Empty Namespace of the Interaction Empty Demo library.<p>
 * The Namespace Ocean::Interaction::Empty is used in the entire Ocean Interaction Empty Demo Library.
 */

// Defines OCEAN_INTERACTION_EPY_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_INTERACTION_EPY_EXPORT
		#define OCEAN_INTERACTION_EPY_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_INTERACTION_EPY_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_INTERACTION_EPY_EXPORT
#endif

#ifndef _DLL

/**
 * Registers this empty demo interaction library at the global interaction manager.
 * This function calls EPYLibrary::registerLibrary() only.
 * @ingroup interactionepy
 */
void registerEmptyLibrary();

/**
 * Unregisters this empty demo interaction library at the global interaction manager.
 * This function calls EPYLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup interactionepy
 */
bool unregisterEmptyLibrary();

#endif // _DLL

} // namespace Empty

} // namespace Interaction

} // namespace Ocean

#endif // META_OCEAN_INTERACTION_EPY_EMPTY_H
