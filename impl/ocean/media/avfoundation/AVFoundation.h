/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_AVFOUNDATION_H
#define META_OCEAN_MEDIA_AVF_AVFOUNDATION_H

#include "ocean/base/ScopedObject.h"

#include "ocean/media/Media.h"

#ifndef OCEAN_PLATFORM_BUILD_APPLE
	#error This library is available on Mac OSX and iOS platforms only!
#endif

#include <CoreMedia/CoreMedia.h>
#include <CoreVideo/CoreVideo.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * @ingroup media
 * @defgroup mediaavf Ocean Media AVFoundation Library
 * @{
 * The Ocean Media AVFoundation Library provides media functionalities based on Apple's AV Foundation framework.
 * The library is available on Apple platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::AVFoundation Namespace of the Media AVFoundation library.<p>
 * The Namespace Ocean::Media::AVFoundation is used in the entire Ocean Media AVFoundation Library.
 */

/**
 * Returns the name of this media AVFoundation library.
 * @ingroup mediaavf
 */
std::string nameAVFLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this media library at the global library manager.
 * This function calls AVFLibrary::registerLibrary() only.
 * @ingroup mediaavf
 */
void registerAVFLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls AVFLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediaavf
 */
bool unregisterAVFLibrary();

#endif // OCEAN_RUNTIME_STATIC

/**
 * Definition of a scoped object holding a CVBufferRef object.
 * The wrapped CVBufferRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediaavf
 */
using ScopedCVBufferRef = ScopedObjectCompileTimeVoidT<CVBufferRef, CVBufferRelease>;

/**
 * Definition of a scoped object holding a CVPixelBufferRef object.
 * The wrapped CVPixelBufferRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediaavf
 */
using ScopedCVPixelBufferRef = ScopedObjectCompileTimeT<CVPixelBufferRef, CVBufferRef, void, CVBufferRelease>;

/**
 * Definition of a scoped object holding a CMSampleBufferRef object.
 * The wrapped CMSampleBufferRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediaavf
 */
using ScopedCMSampleBufferRef = ScopedObjectCompileTimeT<CMSampleBufferRef, CFTypeRef, void, CFRelease>;

}

}

}

#endif // META_OCEAN_MEDIA_AVF_AVFOUNDATION_H
