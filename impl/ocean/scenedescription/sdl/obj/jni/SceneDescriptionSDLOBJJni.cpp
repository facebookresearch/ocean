/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/jni/SceneDescriptionSDLOBJJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_scenedescription_sdl_obj_SceneDescriptionSDLOBJJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDL::OBJ::registerOBJLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_scenedescription_sdl_obj_SceneDescriptionSDLOBJJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();
#endif

	return true;
}
