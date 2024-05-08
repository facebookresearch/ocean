/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FFM_FFM_LIBRARY_H
#define META_OCEAN_MEDIA_FFM_FFM_LIBRARY_H

#include "ocean/media/ffmpeg/FFmpeg.h"

#include "ocean/media/Library.h"
#include "ocean/media/Manager.h"

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

/**
 * This class implements the FFmpeg library.
 * @ingroup mediaffm
 */
class FFMLibrary : public Library
{
	friend class Media::Manager;
	friend class FFMMovie;

	public:

		/**
		 * Creates a new medium by a given url.
		 * @see Library::newMedium().
		 */
		MediumRef newMedium(const std::string& url, bool useExclusive = false) override;

		/**
		 * Creates a new medium by a given url and an expected type.
		 * @see Library::newMedium().
		 */
		MediumRef newMedium(const std::string& url, const Medium::Type type, bool useExclusive = false) override;

		/**
		 * Creates a new recorder specified by the recorder type.
		 * @see Library::newRecorder().
		 */
		RecorderRef newRecorder(const Recorder::Type type) override;

		/**
		 * Returns the supported medium types.
		 * @see Library::supportedTypes().
		 */
		Medium::Type supportedTypes() const override;

		/**
		 * Registers this library at the global media manager.
		 * With each register call, the reference counter for a specific library will be incremented.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library has not been registered before
		 * @see Manager, unregisterLibrary()
		 */
		static bool registerLibrary();

		/**
		 * Unregisters this library at the global media manager.
		 * With each unregister call, the reference counter for a specific library will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library was actually removed from the system (as the reference counter reached zero); False, if the library is still used by someone else
		 * @see registerLibrary().
		 */
		static bool unregisterLibrary();

	protected:

		/**
		 * Creates a new FFMLibrary object.
		 */
		FFMLibrary();

		/**
		 * Destructs an FFMLibrary object.
		 */
		~FFMLibrary() override;

		/**
		 * Creates this library and returns it an object reference.
		 * @return The new library object
		 */
		static LibraryRef create();

		/**
		 * Creates a new movie medium.
		 * @param url Url of the movie medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		static MediumRef newMovie(const std::string& url, bool useExclusive);

		/**
		 * Callback function for FFmpeg's logs.
		 * @param context The context of the log
		 * @param level The log's level
		 * @param message The log message
		 * @param arguments The log's arguments
		 */
		static void avLogCallback(void* context, int level, const char* message, va_list arguments);
};

}

}

}

#endif // META_OCEAN_MEDIA_FFM_FFM_LIBRARY_H
