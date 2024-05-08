/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_SCENEDESCRIPTIONSDLOBJJNI_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_SCENEDESCRIPTIONSDLOBJJNI_H

#include "ocean/scenedescription/sdl/obj/jni/JNI.h"

/**
 * Java native interface function to register the SceneDescription SDLOBJ component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup scenedescriptionsdlobjjni
 */
extern "C" jboolean Java_com_meta_ocean_scenedescription_sdl_obj_SceneDescriptionSDLOBJJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the SceneDescription SDLOBJ adapter component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup scenedescriptionsdlobjjni
 */
extern "C" jboolean Java_com_meta_ocean_scenedescription_sdl_obj_SceneDescriptionSDLOBJJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_JNI_SCENEDESCRIPTIONSDLOBJJNI_H
