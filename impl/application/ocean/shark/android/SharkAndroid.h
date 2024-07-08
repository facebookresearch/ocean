/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_SHARK_H
#define FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_SHARK_H

#include "application/ocean/shark/Shark.h"

#include <cstdlib>
#include <iostream>

#include <jni.h>

namespace Ocean
{

namespace Shark
{

namespace Android
{

/**
 * @ingroup shark
 * @defgroup sharkandroid Ocean Shark VR/AR viewer for Android mobile platforms.
 * @{
 * Shark is the VR/AR viewer of the Ocean framework for mobile platforms.<br>
 * This implementation is especially for Android platforms.<br>
 * It used the entire framework functionality and like e.g. rendering and tracking support.<br>
 * For desktop platforms with higher computational power Ocean provides the Orca viewer.<br>
 * The application is platform dependent and up to this moment available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Shark::Android Namespace of the Ocean Shark VR/AR viewer application for Android applications.
 */

#ifndef _ANDROID
	#error This application is available on Android platforms only!
#endif // _ANDROID

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_SHARK_H
