/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_ANDROID_H
#define META_OCEAN_MEDIA_ANDROID_ANDROID_H

#include "ocean/media/Media.h"

#ifndef _ANDROID
	#error This library is available on Android platforms only!
#endif

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * @ingroup media
 * @defgroup mediaandroid Ocean Media Android Library
 * @{
 * The Ocean Media Android Library provides camera access functionalities on android platforms.
 * The library is available on android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::Android Namespace of the Media Android library.<p>
 * The Namespace Ocean::Media::Android is used in the entire Ocean Media Android Library.
 */

// Defines OCEAN_MEDIA_A_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_A_EXPORT
		#define OCEAN_MEDIA_A_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_A_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_A_EXPORT
#endif

/**
 * Returns the name of this media library.
 * @ingroup mediaandroid
 */
OCEAN_MEDIA_A_EXPORT std::string nameAndroidLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this media library at the global library manager.
 * This function calls ALibrary::registerLibrary() only.
 * @ingroup mediaandroid
 */
void registerAndroidLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls ALibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediaandroid
 */
bool unregisterAndroidLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_ANDROID_H
