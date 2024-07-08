/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_SPECIAL_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_SPECIAL_H

#include "ocean/media/Media.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

/**
 * @ingroup media
 * @defgroup mediaspecial Ocean Media Special Library
 * @{
 * The Ocean Media Special Library provides image media functionalities based on special image formats like e.g., Npy, Pfm, or Bmp.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Media::Special Namespace of the Media Special library.<p>
 * The Namespace Ocean::Media::Special is used in the entire Ocean Media Special Library.
 */

// Defines OCEAN_MEDIA_SPECIAL_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_SPECIAL_EXPORT
		#define OCEAN_MEDIA_SPECIAL_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_SPECIAL_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_SPECIAL_EXPORT
#endif

/**
 * Returns the name of this media Special library.
 * @ingroup mediaspecial
 */
OCEAN_MEDIA_SPECIAL_EXPORT std::string nameSpecialLibrary();

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_SPECIAL_H
