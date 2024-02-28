// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdl/assimp/jni/SceneDescriptionSDLAssimpJni.h"

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_scenedescription_sdl_assimp_SceneDescriptionSDLAssimpJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_scenedescription_sdl_assimp_SceneDescriptionSDLAssimpJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	return true;
}
