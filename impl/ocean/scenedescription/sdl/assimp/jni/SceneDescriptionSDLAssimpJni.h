// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_SCENEDESCRIPTIONSDLASSIMPJNI_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_SCENEDESCRIPTIONSDLASSIMPJNI_H

#include "ocean/platform/android/Android.h"

/**
 * Java native interface function to register the SceneDescription SDLAssimp component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup scenedescriptionsdlassimpjni
 */
extern "C" jboolean Java_com_facebook_ocean_scenedescription_sdl_assimp_SceneDescriptionSDLAssimpJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the SceneDescription SDLAssimp adapter component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup scenedescriptionsdlassimpjni
 */
extern "C" jboolean Java_com_facebook_ocean_scenedescription_sdl_assimp_SceneDescriptionSDLAssimpJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_JNI_SCENEDESCRIPTIONSDLASSIMPJNI_H
