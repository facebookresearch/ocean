/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_H

#include "ocean/scenedescription/SceneDescription.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

/**
 * @ingroup scenedescription
 * @defgroup scenedescriptionsdlobj Ocean SceneDescription SDL OBJ Library
 * @{
 * The Ocean SceneDescription SDL OBJ Library implements a scene description interface for obj files.<br>
 * The library is platform independent.<br>
 * Do not use any specific function of this library directly, use the abstract interface of the Manager object instead.
 * @see Manager.
 * @}
 */

/**
 * @namespace Ocean::SceneDescription::SDL::OBJ Namespace of the SDL OBJ SceneDescription library.<p>
 * The Namespace Ocean::SceneDescription::SDL::OBJ is used in the entire Ocean SceneDescription SDL OBJ Library.
 */

// Defines OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT
		#define OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT
#endif

/**
 * Returns the name of this OBJ scenedescription library.
 * @ingroup scenedescriptionsdlobj
 */
OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT std::string nameOBJLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this scenedescription library at the global scenedescription manager.
 * This function calls OBJLibrary::registerLibrary() only.
 * @ingroup scenedescriptionsdlobj
 */
void registerOBJLibrary();

/**
 * Unregisters this scenedescription library at the global scenedescription manager.
 * This function calls OBJLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup scenedescriptionsdlobj
 */
bool unregisterOBJLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_H
