/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/jni/SceneDescriptionSDXX3DJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_scenedescription_sdx_x3d_SceneDescriptionSDXX3DJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDX::X3D::registerX3DLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_scenedescription_sdx_x3d_SceneDescriptionSDXX3DJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDX::X3D::unregisterX3DLibrary();
#endif

	return true;
}
