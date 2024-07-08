/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GLES_GLES_H
#define META_OCEAN_PLATFORM_GLES_GLES_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

namespace GLES
{

/**
 * @ingroup platform
 * @defgroup platformgles Ocean Platform GLES Library
 * @{
 * The Ocean GLES Library provides specific functionalities for OpenGLES.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Platform::GLES Namespace of the Platform GLES library.<p>
 * The Namespace Ocean::Platform::GLES is used in the entire Ocean Platform GLES Library.
 */

// Defines OCEAN_PLATFORM_GLES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_GLES_EXPORT
		#define OCEAN_PLATFORM_GLES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_GLES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_GLES_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_GLES_GLES_H
