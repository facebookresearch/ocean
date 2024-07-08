/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_ASSIMP_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_ASSIMP_H

#include "ocean/scenedescription/SceneDescription.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

/**
 * @ingroup scenedescription
 * @defgroup scenedescriptionsdlassimp Ocean SceneDescription SDL Assimp Library
 * @{
 * The Ocean SceneDescription SDL Assimp Library implements a scene description interface for assimp-supported files.<br>
 * The library is platform independent.<br>
 * Do not use any specific function of this library directly, use the abstract interface of the Manager object instead.
 * @see Manager.
 * @}
 */

/**
 * @namespace Ocean::SceneDescription::SDL::Assimp Namespace of the SDL Assimp SceneDescription library.<p>
 * The Namespace Ocean::SceneDescription::SDL::Assimp is used in the entire Ocean SceneDescription SDL Assimp Library.
 */

// Defines OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT
		#define OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT
#endif

/**
 * Returns the name of this Assimp scenedescription library.
 * @ingroup scenedescriptionsdlassimp
 */
OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT std::string nameAssimpLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this scenedescription library at the global scenedescription manager.
 * This function calls AssimpLibrary::registerLibrary() only.
 * @ingroup scenedescriptionsdlassimp
 */
void registerAssimpLibrary();

/**
 * Unregisters this scenedescription library at the global scenedescription manager.
 * This function calls AssimpLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup scenedescriptionsdlassimp
 */
bool unregisterAssimpLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_ASSIMP_H
