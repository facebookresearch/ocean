// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/jni/SceneDescriptionSDXX3DJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_scenedescription_sdx_x3d_SceneDescriptionSDXX3DJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDX::X3D::registerX3DLibrary();
#endif

	return true;
}

jboolean Java_com_facebook_ocean_scenedescription_sdx_x3d_SceneDescriptionSDXX3DJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDX::X3D::unregisterX3DLibrary();
#endif

	return true;
}
