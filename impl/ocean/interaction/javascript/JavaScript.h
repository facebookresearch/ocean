/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_JAVA_SCRIPT_H
#define META_OCEAN_INTERACTION_JS_JAVA_SCRIPT_H

#include "ocean/base/Base.h"

#include "ocean/interaction/Interaction.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * @ingroup interaction
 * @defgroup interactionjs Ocean Interaction JavaScript Library
 * @{
 * The Ocean Interaction JavaScript Library implements a java script interaction library.<br>
 * @}
 */

/**
 * @namespace Ocean::Interaction::JavaScript Namespace of the Interaction JavaScript library.<p>
 * The Namespace Ocean::Interaction::JavaScript is used in the entire Ocean Interaction JavaScript Library.
 */

// Defines OCEAN_INTERACTION_JS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_INTERACTION_JS_EXPORT
		#define OCEAN_INTERACTION_JS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_INTERACTION_JS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_INTERACTION_JS_EXPORT
#endif

// Defines OCEAN_V8_VERSION according to the given V8 version, 5.3 equals to 50300
#define OCEAN_V8_VERSION (V8_MAJOR_VERSION * 10000 + V8_MINOR_VERSION * 100)

/**
 * Returns the name of this interaction library.
 * @ingroup interactionjs
 */
OCEAN_INTERACTION_JS_EXPORT std::string nameJavaScriptLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this JavaScript interaction library at the global interaction manager.
 * This function calls JSLibrary::registerLibrary() only.
 * @ingroup interactionjs
 */
void registerJavaScriptLibrary();

/**
 * Unregisters this JavaScript interaction library at the global interaction manager.
 * This function calls JSLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup interactionjs
 */
bool unregisterJavaScriptLibrary();

#endif // OCEAN_RUNTIME_STATIC

} // namespace JavaScript

} // namespace Interaction

} // namespace Ocean

#endif // META_OCEAN_INTERACTION_JS_JAVA_SCRIPT_H
