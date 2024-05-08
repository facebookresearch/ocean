/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_WIC_WIC_H
#define META_OCEAN_MEDIA_WIC_WIC_H

#include "ocean/media/Media.h"

#ifndef _WINDOWS
	#error This library is available on windows platforms only!
#endif // _WINDOWS


namespace Ocean
{

namespace Media
{

namespace WIC
{

/**
 * @ingroup media
 * @defgroup mediawic Ocean Media Windows Imaging Component (WIC) Library
 * @{
 * The Ocean Media WIC Library provides media functionalities based on the Microsoft's Windows Imaging Component.
 * The library is available on Windows platforms.
 * @}
 */

/**
 * @namespace Ocean::Media::WIC Namespace of the Media WIC library.<p>
 * The Namespace Ocean::Media::WIC is used in the entire Ocean Media WIC Library.
 */

// Defines OCEAN_MEDIA_WIC_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_WIC_EXPORT
		#define OCEAN_MEDIA_WIC_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_WIC_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_WIC_EXPORT
#endif

/**
 * Returns the name of this media WIC library.
 * @ingroup mediawic
 */
OCEAN_MEDIA_WIC_EXPORT std::string nameWICLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this media library at the global library manager.
 * This function calls WICLibrary::registerLibrary() only.
 * @ingroup mediawic
 */
void registerWICLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls WICLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediawic
 */
bool unregisterWICLibrary();

#endif // OCEAN_RUNTIME_STATIC

} // namespace WIC

} // namespace Media

} // namespace Ocean

#endif // META_OCEAN_MEDIA_WIC_WIC_H
