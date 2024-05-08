/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MEDIA_H
#define META_OCEAN_MEDIA_MEDIA_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Media
{

/**
 * @defgroup media Ocean Media Abstraction Library
 * @{
 * The Ocean Media Library provides access to individual media objects.
 * Media object can provide visual or/and audio content or can capture visual or/and audio content.<br>
 *
 * The base class for all media objects providing content is the Medium class.<br>
 * All Medium classes are designed to provide the content in real-time.<br>
 * There can be finite Medium objects like movie or audio files or there can be infinite Medium objects like live video streams or live camera streams.<br>
 * @see Medium, Image, Movie, Audio, FiniteMedium.
 *
 * The base class for all media objects capturing content is the Recorder class.<br>
 * Whether a Recorder is able to capture content in real-time depends on the underlying hardware and the underlying encoder technology.<br>
 * @see Recorder, MovieRecorder.
 *
 * Further, this library provides an abstract frame provider interface allowing random access for visual content of finite media objects.<br>
 * @see MediaFrameProviderInterface.
 *
 * This library is mainly an abstraction layer for the media objects.<br>
 * The actual implementation of the media objects is lokated in separate libraries allowing to realize individual media libraries on individual hardware platforms.<br>
 * However, the actual implementation is hided from the user so that any kind of media object from any kind of media library can be applied by the same functions and interfaces.<br>
 * Any derived media library implements a Library class which registers all supported media objects and media types at the media Manager of this abstraction library.<br>
 * This manager provides the unique access to all media objects.<br>
 * @see Manager, Library.
 *
 * The Utilities class provides simple helper functions to receive e.g., the visual content from an image file.
 *
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Media Namespace of the Media library.<p>
 * The Namespace Ocean::Media is used in the entire Ocean Media Library.
 */

// Defines OCEAN_MEDIA_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_EXPORT
		#define OCEAN_MEDIA_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_EXPORT
#endif

}

}

#endif // META_OCEAN_MEDIA_MEDIA_H
