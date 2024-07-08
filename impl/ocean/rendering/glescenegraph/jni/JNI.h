/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLESCENEGRAPH_JNI_JNI_H
#define META_OCEAN_RENDERING_GLESCENEGRAPH_JNI_JNI_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"

#include <jni.h>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace JNI
{

/**
 * @ingroup renderingglescenegraph
 * @defgroup renderingglescenegraphjni Ocean Rendering GLESceneGraph JNI Library
 * @{
 * The Ocean Rendering GLESceneGraph JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Rendering::GLESceneGraph::JNI Namespace of the Rendering GLESceneGraph JNI library.<p>
 * The Namespace Ocean::Rendering::GLESceneGraph::JNI is used in the entire Ocean Rendering GLESceneGraph JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_RENDERING_GLESCENEGRAPH_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_RENDERING_GLESCENEGRAPH_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_RENDERING_GLESCENEGRAPH_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_RENDERING_GLESCENEGRAPH_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_RENDERING_GLESCENEGRAPH_JNI_JNI_H
