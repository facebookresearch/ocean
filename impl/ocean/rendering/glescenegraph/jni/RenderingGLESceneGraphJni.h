/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLESCENEGRAPH_JNI_RENDERINGGLESCENEGRAPHJNI_H
#define META_OCEAN_RENDERING_GLESCENEGRAPH_JNI_RENDERINGGLESCENEGRAPHJNI_H

#include "ocean/rendering/glescenegraph/jni/JNI.h"

/**
 * Java native interface function to register the Rendering GLESceneGraph component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup renderingglescenegraphjni
 */
extern "C" jboolean Java_com_meta_ocean_rendering_glescenegraph_RenderingGLESceneGraphJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Rendering GLESceneGraph component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup renderingglescenegraphjni
 */
extern "C" jboolean Java_com_meta_ocean_rendering_glescenegraph_RenderingGLESceneGraphJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_RENDERING_GLESCENEGRAPH_JNI_RENDERINGGLESCENEGRAPHJNI_H
