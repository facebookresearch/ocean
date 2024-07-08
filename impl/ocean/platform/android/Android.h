/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_ANDROID_H
#define META_OCEAN_PLATFORM_ANDROID_ANDROID_H

#include "ocean/base/Exception.h"

#include "ocean/platform/Platform.h"

#include <jni.h>

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * @ingroup platform
 * @defgroup platformandroid Ocean Platform Android Library
 * @{
 * The Ocean Android Platform Library provides specific functionalities for Android platforms.
 * The library is available on android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Android Namespace of the Platform Android library.<p>
 * The Namespace Ocean::Platform::Android is used in the entire Ocean Platform Android Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_PLATFORM_ANDROID_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_PLATFORM_ANDROID_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_ANDROID_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_ANDROID_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_ANDROID_H
