/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_JNI_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_JNI_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"

#include <jni.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

namespace JNI
{

/**
 * @ingroup scenedescriptionsdlobj
 * @defgroup scenedescriptionsdlobjjni Ocean Scene Description SDL OBJ JNI Library
 * @{
 * The Ocean Scene Description SDL OBJ JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::SceneDescription::SDL::OBJ::JNI Namespace of the Scene Description SDL OBJ JNI library.<p>
 * The Namespace Ocean::SceneDescription::SDL::OBJ::JNI is used in the entire Ocean Scene Description SDL OBJ JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_JNI_H
