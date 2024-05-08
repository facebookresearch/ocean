/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_SCENEDESCRIPTIONSDXX3DJNI_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_SCENEDESCRIPTIONSDXX3DJNI_H

#include "ocean/scenedescription/sdx/x3d/jni/JNI.h"

/**
 * Java native interface function to register the SceneDescription SDXX3D component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup scenedescriptionsdxx3djni
 */
extern "C" jboolean Java_com_meta_ocean_scenedescription_sdx_x3d_SceneDescriptionSDXX3DJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the SceneDescription SDXX3D adapter component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup scenedescriptionsdxx3djni
 */
extern "C" jboolean Java_com_meta_ocean_scenedescription_sdx_x3d_SceneDescriptionSDXX3DJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_SCENEDESCRIPTIONSDXX3DJNI_H
