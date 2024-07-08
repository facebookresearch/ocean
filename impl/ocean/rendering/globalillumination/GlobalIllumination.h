/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_GLOBAL_ILLUMINATION_H
#define META_OCEAN_RENDERING_GI_GLOBAL_ILLUMINATION_H

#include "ocean/rendering/Rendering.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * @ingroup rendering
 * @defgroup renderinggi Ocean Rendering Global Illumination Library
 * @{
 * The Ocean Rendering Global Illumination Library is a rendering library using Global Illumination.
 * Is this library is available on windows and linux platforms.
 * @}
 */

/**
 * @namespace Ocean::Rendering::GlobalIllumination Namespace of the Rendering Global Illumination library.<p>
 * The Namespace Ocean::Rendering::GlobalIllumination is used in the entire Ocean Rendering Global Illumination Library.
 */

// Defines OCEAN_RENDERING_GI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_RENDERING_GI_EXPORT
		#define OCEAN_RENDERING_GI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_RENDERING_GI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_RENDERING_GI_EXPORT
#endif

/**
 * Returns the name of this rendering engine.
 * @ingroup renderinggi
 */
const std::string& globalIlluminationEngineName();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this rendering engine at the global engine manager.
 * This function calls GIEngine::registerEngine() only.
 * @ingroup renderinggi
 */
void registerGlobalIlluminationEngine();

/**
 * Unregisters this rendering engine at the global engine manager.
 * This function calls GIEngine::unregisterEngine() only.
 * @return True, if succeeded
 * @ingroup renderinggi
 */
bool unregisterGlobalIlluminationEngine();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_RENDERING_GI_GLOBAL_ILLUMINATION_H
