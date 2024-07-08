/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IMAGEIO_H
#define META_OCEAN_MEDIA_IMAGEIO_IMAGEIO_H

#include "ocean/media/Media.h"

#include "ocean/base/ScopedObject.h"

#ifndef OCEAN_PLATFORM_BUILD_APPLE
	#error This library is available on Apple platforms only!
#endif

#include <ImageIO/ImageIO.h>

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * @ingroup media
 * @defgroup mediaiio Ocean Media ImageIO Library
 * @{
 * The Ocean Media ImageIO Library provides media functionalities based on Apple's ImageIO Component.
 * The library is available on Apple platforms.
 * @}
 */

/**
 * @namespace Ocean::Media::ImageIO Namespace of the Media ImageIO library.<p>
 * The Namespace Ocean::Media::ImageIO is used in the entire Ocean Media ImageIO Library.
 */

// Defines OCEAN_MEDIA_IIO_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_IIO_EXPORT
		#define OCEAN_MEDIA_IIO_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_IIO_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_IIO_EXPORT
#endif

/**
 * Returns the name of this media ImageIO library.
 * @ingroup mediaiio
 */
OCEAN_MEDIA_IIO_EXPORT std::string nameImageIOLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this media library at the global library manager.
 * This function calls IIOLibrary::registerLibrary() only.
 * @ingroup mediaiio
 */
void registerImageIOLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls IIOLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediaiio
 */
bool unregisterImageIOLibrary();

#endif // OCEAN_RUNTIME_STATIC

/**
 * Definition of a scoped object holding a CGImageSourceRef object.
 * The wrapped CGImageSourceRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediaiio
 */
using ScopedCGImageSourceRef = ScopedObjectCompileTimeT<CGImageSourceRef, CFTypeRef, void, CFRelease>;

/**
 * Definition of a scoped object holding a CGImageDestinationRef object.
 * The wrapped CGImageDestinationRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediaiio
 */
using ScopedCGImageDestinationRef = ScopedObjectCompileTimeT<CGImageDestinationRef, CFTypeRef, void, CFRelease>;

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IMAGEIO_H
