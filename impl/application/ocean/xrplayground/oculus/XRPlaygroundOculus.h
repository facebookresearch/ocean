// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_XR_PLAYGROUND_OCULUS_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_XR_PLAYGROUND_OCULUS_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include <cstdlib>
#include <iostream>

#include <jni.h>

namespace Ocean
{

namespace XRPlayground
{

namespace Oculus
{

/**
 * @ingroup xrplayground
 * @defgroup xrplaygroundoculus The Ocean powered XRPlayground app for Oculus platforms.
 * @{
 * The XRPlayground app is a lightweight app for rapid prototyping XR-related features.
 * In this namespace, we keep the Oculus-specific code.
 * @}
 */

/**
 * @namespace Ocean::XRPlayground::Oculus Namespace of the Ocean powered XRPlayground app for Oculus platforms.
 */

#ifndef _ANDROID
	#error This application is available on Oculus/Android platforms only!
#endif // _ANDROID

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_XR_PLAYGROUND_OCULUS_H
