// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_APPLICATION_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_APPLICATION_H

#include "ocean/base/Exception.h"

#include "ocean/platform/android/Android.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

/**
 * @ingroup platformandroid
 * @defgroup platformandroidapplication Ocean Platform Android Application Library
 * @{
 * The Ocean Android Platform Application Library provides specific functionalities for applications on Android platforms.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Android::Application Namespace of the Platform Android Application library.<p>
 * The Namespace Ocean::Platform::Android::Application is used in the entire Ocean Platform Android Application Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_APPLICATION_H
