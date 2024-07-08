/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/jni/SceneDescriptionSDLAssimpJni.h"

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_scenedescription_sdl_assimp_SceneDescriptionSDLAssimpJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_scenedescription_sdl_assimp_SceneDescriptionSDLAssimpJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
#endif

	return true;
}
