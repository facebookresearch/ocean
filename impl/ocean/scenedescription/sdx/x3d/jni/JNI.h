/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_JNI_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_JNI_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include <jni.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

namespace JNI
{

/**
 * @ingroup scenedescriptionsdxx3d
 * @defgroup scenedescriptionsdxx3djni Ocean Scene Description SDX X3D JNI Library
 * @{
 * The Ocean Scene Description SDX X3D JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::SceneDescription::SDX::X3D::JNI Namespace of the Scene Description SDX X3D JNI library.<p>
 * The Namespace Ocean::SceneDescription::SDX::X3D::JNI is used in the entire Ocean Scene Description SDX X3D JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_JNI_JNI_H
