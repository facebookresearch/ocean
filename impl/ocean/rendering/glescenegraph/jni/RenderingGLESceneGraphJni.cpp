/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/jni/RenderingGLESceneGraphJni.h"

#include "ocean/rendering/glescenegraph/android/Android.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_rendering_glescenegraph_RenderingGLESceneGraphJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GLESceneGraph::Android::registerGLESceneGraphEngine();
#endif

	return true;
}

jboolean Java_com_meta_ocean_rendering_glescenegraph_RenderingGLESceneGraphJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();
#endif

	return true;
}
