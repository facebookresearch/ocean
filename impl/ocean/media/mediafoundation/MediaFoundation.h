/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_MEDIA_FOUNDATION_H
#define META_OCEAN_MEDIA_MF_MEDIA_FOUNDATION_H

#include "ocean/media/Media.h"

#include "ocean/base/ScopedObject.h"

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS
	#error This library is available on windows platforms only!
#endif // OCEAN_PLATFORM_BUILD_WINDOWS

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

DISABLE_WARNINGS_BEGIN
	#include <mfapi.h>
	#include <mfidl.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * @ingroup media
 * @defgroup mediamf Ocean Media MediaFoundation Library
 * @{
 * The Ocean Media MediaFoundation Library provides MediaFoundation media functionalities.
 * The library is available on windows platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::MediaFoundation Namespace of the Media MediaFoundation library.<p>
 * The Namespace Ocean::Media::MediaFoundation is used in the entire Ocean Media MediaFoundation Library.
 */

// Defines OCEAN_MEDIA_MF_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_MF_EXPORT
		#define OCEAN_MEDIA_MF_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_MF_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_MF_EXPORT
#endif

/**
 * Returns the name of this media MediaFoundation library.
 * @ingroup mediamf
 */
OCEAN_MEDIA_MF_EXPORT std::string nameMediaFoundationLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this media library at the global library manager.
 * This function calls MFLibrary::registerLibrary() only.
 * @ingroup mediamf
 */
void registerMediaFoundationLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls MFLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediamf
 */
bool unregisterMediaFoundationLibrary();

#endif // OCEAN_RUNTIME_STATIC

/**
 * This functions allows to release a media foundation object if it does exist.
 * @param object Media Foundation object to be released
 * @ingroup mediamf
 */
template <class T> void release(T *object);

/**
 * Template specialization for ScopedObjectCompileTimeVoid using the release() function to release media foundation objects.
 * @tparam T The data type of the wrapped object
 * @ingroup mediamf
 */
template <typename T>
using ScopedMediaFoundationObject = ScopedObjectCompileTimeVoidT<T*, release>;

/**
 * Definition of a scoped object holding a IMFActivate object.
 * The wrapped IMFActivate object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFActivate = ScopedMediaFoundationObject<IMFActivate>;

/**
 * Definition of a scoped object holding a IMFAudioStreamVolume object.
 * The wrapped IMFAudioStreamVolume object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFAudioStreamVolume = ScopedMediaFoundationObject<IMFAudioStreamVolume>;

/**
 * Definition of a scoped object holding a IMFAttributes object.
 * The wrapped IMFAttributes object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFAttributes = ScopedMediaFoundationObject<IMFAttributes>;

/**
 * Definition of a scoped object holding a IMFMediaBuffer object.
 * The wrapped IMFMediaBuffer object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaBuffer = ScopedMediaFoundationObject<IMFMediaBuffer>;

/**
 * Definition of a scoped object holding a IMFMediaEvent object.
 * The wrapped IMFMediaEvent object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaEvent = ScopedMediaFoundationObject<IMFMediaEvent>;

/**
 * Definition of a scoped object holding a IMFMediaEventGenerator object.
 * The wrapped IMFMediaEventGenerator object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaEventGenerator = ScopedMediaFoundationObject<IMFMediaEventGenerator>;

/**
 * Definition of a scoped object holding a IMFMediaSession object.
 * The wrapped IMFMediaSession object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaSession = ScopedMediaFoundationObject<IMFMediaSession>;

/**
 * Definition of a scoped object holding a ScopedIMFMediaSource object.
 * The wrapped ScopedIMFMediaSource object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaSource = ScopedMediaFoundationObject<IMFMediaSource>;

/**
 * Definition of a scoped object holding a IMFMediaType object.
 * The wrapped IMFMediaType object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaType = ScopedMediaFoundationObject<IMFMediaType>;

/**
 * Definition of a scoped object holding a IMFMediaTypeHandler object.
 * The wrapped IMFMediaTypeHandler object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFMediaTypeHandler = ScopedMediaFoundationObject<IMFMediaTypeHandler>;

/**
 * Definition of a scoped object holding a IMFSample object.
 * The wrapped IMFSample object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFSample = ScopedMediaFoundationObject<IMFSample>;

/**
 * Definition of a scoped object holding a IMFStreamSink object.
 * The wrapped IMFStreamSink object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFStreamSink = ScopedMediaFoundationObject<IMFStreamSink>;

/**
 * Definition of a scoped object holding a IMFTopology object.
 * The wrapped IMFTopology object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFTopology = ScopedMediaFoundationObject<IMFTopology>;

/**
 * Definition of a scoped object holding a IMFTopologyNode object.
 * The wrapped IMFTopologyNode object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
using ScopedIMFTopologyNode = ScopedMediaFoundationObject<IMFTopologyNode>;

template <class T>
void release(T *object)
{
	if (object != nullptr)
	{
		object->Release();
	}
}

} // namespace MediaFoundation

} // namespace Media

} // namespace Ocean

#endif // META_OCEAN_MEDIA_MF_MEDIA_FOUNDATION_H
