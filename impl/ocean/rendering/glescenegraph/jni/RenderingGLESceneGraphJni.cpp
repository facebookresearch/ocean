// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/jni/RenderingGLESceneGraphJni.h"

#include "ocean/rendering/glescenegraph/android/Android.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_rendering_glescenegraph_RenderingGLESceneGraphJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Rendering::GLESceneGraph::Android::registerGLESceneGraphEngine();
	return true;
}

jboolean Java_com_facebook_ocean_rendering_glescenegraph_RenderingGLESceneGraphJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();
	return true;
}
