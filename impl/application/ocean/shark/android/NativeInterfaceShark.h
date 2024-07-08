/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_NATIVE_INTERFACE_SHARK_H
#define FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_NATIVE_INTERFACE_SHARK_H

#include "application/ocean/shark/android/SharkAndroid.h"

namespace Ocean
{

namespace Shark
{

namespace Android
{

/**
 * Java native interface function loading a new scene file.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param filename Filename of the scene to be loaded
 * @param replace State determining whether already existing scenes will be removed first, or whether the scene will be added instead
 * @return True, if succeeded
 * @ingroup sharkandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_shark_android_NativeInterfaceShark_loadScene(JNIEnv* env, jobject javaThis, jstring filename, jboolean replace);

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_NATIVE_INTERFACE_SHARK_H
