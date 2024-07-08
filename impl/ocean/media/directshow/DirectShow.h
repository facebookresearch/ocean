/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_DIRECTSHOW_H
#define META_OCEAN_MEDIA_DS_DIRECTSHOW_H

#include "ocean/media/Media.h"

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS
	#error This library is available on windows platforms only!
#endif // OCEAN_PLATFORM_BUILD_WINDOWS

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * @ingroup media
 * @defgroup mediads Ocean Media DirectShow Library
 * @{
 * The Ocean Media DirectShow Library provides DirectShow media functionalities.
 * The library is available on windows platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::DirectShow Namespace of the Media DirectShow library.<p>
 * The Namespace Ocean::Media::DirectShow is used in the entire Ocean Media DirectShow Library.
 */

// Defines OCEAN_MEDIA_DS_EXPORT for dll export and import.
#if defined(OCEAN_PLATFORM_BUILD_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_DS_EXPORT
		#define OCEAN_MEDIA_DS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_DS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_DS_EXPORT
#endif

/**
 * Returns the name of this media DirectShow library.
 * @ingroup mediads
 */
OCEAN_MEDIA_DS_EXPORT std::string nameDirectShowLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this media library at the global library manager.
 * This function calls DSLibrary::registerLibrary() only.
 * @ingroup mediads
 */
void registerDirectShowLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls DSLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediads
 */
bool unregisterDirectShowLibrary();

#endif // OCEAN_RUNTIME_STATIC

// The DISABLE_WARNINGS_BEGIN macro allows to disable any warnings, needs to be balanced with DISABLE_WARNINGS_END
#ifndef DISABLE_WARNINGS_BEGIN
	#if defined(_MSC_VER)
		#define DISABLE_WARNINGS_BEGIN _Pragma("warning(push, 0)")
		#define DISABLE_WARNINGS_END _Pragma("warning(pop)")
	#else
		#define DISABLE_WARNINGS_BEGIN
		#define DISABLE_WARNINGS_END
	#endif
#endif

} // namespace DirectShow

} // namespace Media

} // namespace Ocean

#endif // META_OCEAN_MEDIA_DS_DIRECTSHOW_H
