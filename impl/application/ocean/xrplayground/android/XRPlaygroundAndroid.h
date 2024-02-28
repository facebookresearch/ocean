// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_XR_PLAYGROUND_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_XR_PLAYGROUND_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include <cstdlib>
#include <iostream>

#include <jni.h>

namespace Ocean
{

namespace XRPlayground
{

namespace Android
{

/**
 * @ingroup xrplayground
 * @defgroup xrplaygroundandroid The Ocean powered XRPlayground app for Android platforms.
 * @{
 * The XRPlayground app is a lightweight app for rapid prototyping XR-related features.
 * In this namespace, we keep the Android-specific code.
 * @}
 */

/**
 * @namespace Ocean::XRPlayground::Android Namespace of the Ocean powered XRPlayground app for Android platforms.
 */

#ifndef _ANDROID
	#error This application is available on Android platforms only!
#endif // _ANDROID

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_XR_PLAYGROUND_H
