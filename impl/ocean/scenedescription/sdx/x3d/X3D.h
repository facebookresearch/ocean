/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_H

#include "ocean/scenedescription/SceneDescription.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * @ingroup scenedescription
 * @defgroup scenedescriptionsdxx3d Ocean SceneDescription SDX X3D Library
 * @{
 * The Ocean SceneDescription SDX X3D Library implements a scene description interface for X3D files.<br>
 * The library is platform independent.
 * Do not use any specific function of this library directly, use the abstract interface of the Manager object instead.
 * @see Manager.
 * @}
 */

/**
 * @namespace Ocean::SceneDescription::SDX::X3D Namespace of the S(scene)D(escription)X X3D SceneDescription library.<p>
 * The Namespace Ocean::SceneDescription::SDX::X3D is used in the entire Ocean SceneDescription SDX X3D Library.
 */

// Defines OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT
		#define	OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT
#endif

/**
 * Returns the name of this OBJ scenedescription library.
 * @ingroup renderinggles
 */
OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT std::string nameX3DLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this scenedescription library at the global scenedescription manager.
 * This function calls X3DLoader::registerLoader() only.
 * @ingroup scenedescriptionlsgx3d
 */
void registerX3DLibrary();

/**
 * Unregisters this scenedescription library at the global scenedescription manager.
 * This function calls X3DLoader::unregisterLoader() only.
 * @return True, if succeeded
 * @ingroup scenedescriptionlsgx3d
 */
bool unregisterX3DLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_H
