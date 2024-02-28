// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdl/obj/jni/SceneDescriptionSDLOBJJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_scenedescription_sdl_obj_SceneDescriptionSDLOBJJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	SceneDescription::SDL::OBJ::registerOBJLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_scenedescription_sdl_obj_SceneDescriptionSDLOBJJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();
	return true;
}
