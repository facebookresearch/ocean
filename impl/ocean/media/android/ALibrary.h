/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_LIBRARY_H
#define META_OCEAN_MEDIA_ANDROID_A_LIBRARY_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/NativeCameraLibrary.h"
#include "ocean/media/android/NativeMediaLibrary.h"

#include "ocean/media/Library.h"
#include "ocean/media/Manager.h"

#include <jni.h>

#include <SLES/OpenSLES.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements the android library.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT ALibrary : public Library
{
	friend class Media::Manager;
	friend class AAudio;
	friend class ALiveAudio;
	friend class ALiveVideo;
	friend class AMovie;

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
		 * Creates a new ALibrary object.
		 */
		ALibrary();

		/**
		 * Destructs an ALibrary object.
		 */
		~ALibrary() override;

		/**
		 * Creates a new audio medium.
		 * @param url The URL of the audio medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		MediumRef newAudio(const std::string& url, bool useExclusive);

		/**
		 * Creates a new live audio medium.
		 * @param url The URL of the audio medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		MediumRef newLiveAudio(const std::string& url, bool useExclusive);

		/**
		 * Creates a new microphone medium.
		 * @param url The URL of the audio medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		MediumRef newMicrophone(const std::string& url, bool useExclusive);

		/**
		 * Initializes the SL audio engine.
		 * @return True, if succeeded
		 */
		bool initializerAudioEngine();

		/**
		 * Releases the SL audio engine.
		 */
		void releaseAudioEngine();

		/**
		 * Creates a new live video medium.
		 * @param url The URL of the live video medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		static MediumRef newLiveVideo(const std::string& url, bool useExclusive);

		/**
		 * Creates a new movie medium.
		 * @param url The URL of the movie medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		static MediumRef newMovie(const std::string& url, bool useExclusive);

		/**
		 * Creates this library and returns it an object reference.
		 * @return The new library object
		 */
		static LibraryRef create();

	protected:

		/// The SL engine for all audio objects.
		SLObjectItf slEngine_ = nullptr;

		/// The SL engine interface for all audio objects.
		SLEngineItf slEngineInterface_ = nullptr;

#ifdef OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE
		/// The subscription for the native camera library
		NativeCameraLibrary::ScopedSubscription nativeCameraLibrarySubscription_;
#endif

#ifdef OCEAN_MEDIA_ANDROID_NATIVEMEDIALIBRARY_AVAILABLE
		/// The subscription for the native media library
		NativeMediaLibrary::ScopedSubscription nativeMediaLibrarySubscription_;
#endif
};

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_LIBRARY_H
