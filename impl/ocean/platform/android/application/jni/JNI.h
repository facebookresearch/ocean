/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_JNI_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_JNI_H

#include "ocean/base/Exception.h"

#include "ocean/platform/android/application/Application.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

namespace JNI
{

/**
 * @ingroup platformandroidapplication
 * @defgroup platformandroidapplicationjni Ocean Platform Android Application JNI Library
 * @{
 * The Ocean Android Platform Application Library provides JNI functionalities for applications on Android platforms.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Android::Application::JNI Namespace of the Platform Android Application JNI library.<p>
 * The Namespace Ocean::Platform::Android::Application::JNI is used in the entire Ocean Platform Android Application JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_JNI_H
