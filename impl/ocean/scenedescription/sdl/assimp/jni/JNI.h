/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_JNI_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_JNI_H

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

#include <jni.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

namespace JNI
{

/**
 * @ingroup scenedescriptionsdlassimp
 * @defgroup scenedescriptionsdlassimpjni Ocean Scene Description SDL Assimp JNI Library
 * @{
 * The Ocean Scene Description SDL Assimp JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::SceneDescription::SDL::Assimp::JNI Namespace of the Scene Description SDL Assimp JNI library.<p>
 * The Namespace Ocean::SceneDescription::SDL::Assimp::JNI is used in the entire Ocean Scene Description SDL Assimp JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_JNI_H
