/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OPEN_IMAGE_LIBRARIES_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OPEN_IMAGE_LIBRARIES_H

#include "ocean/media/Media.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * @ingroup media
 * @defgroup mediaoil Ocean Media OpenImageLibraries Library
 * @{
 * The Ocean Media OpenImageLibraries Library provides image media functionalities based on common open image libraries like jpglib, libpng, and zlib.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Media::OpenImageLibraries Namespace of the Media OpenImageLibraries library.<p>
 * The Namespace Ocean::Media::OpenImageLibraries is used in the entire Ocean Media OpenImageLibraries Library.
 */

// Defines OCEAN_MEDIA_OIL_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_OIL_EXPORT
		#define OCEAN_MEDIA_OIL_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_OIL_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_OIL_EXPORT
#endif

/**
 * Returns the name of this media OpenImageLibraries library.
 * @ingroup mediaoil
 */
OCEAN_MEDIA_OIL_EXPORT std::string nameOpenImageLibrariesLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this media library at the global library manager.
 * This function calls OILLibrary::registerLibrary() only.
 * @ingroup mediaoil
 */
void registerOpenImageLibrariesLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls OILLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediaoil
 */
bool unregisterOpenImageLibrariesLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OPEN_IMAGE_LIBRARIES_H
