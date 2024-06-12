/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/ALibrary.h"
#include "ocean/media/android/AAudio.h"
#include "ocean/media/android/ALiveAudio.h"
#include "ocean/media/android/ALiveVideo.h"
#include "ocean/media/android/AMicrophone.h"
#include "ocean/media/android/AMovie.h"
#include "ocean/media/android/AMovieRecorder.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

ALibrary::ALibrary() :
	Library(nameAndroidLibrary(), 70u)
{
	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("gif");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("tiff");

#if defined(__NDK_MAJOR__) && defined(__ANDROID_API__)
	Log::debug() << "Media::Android uses Android SDK version " << __NDK_MAJOR__ << " and API level " << __ANDROID_API__;
#endif

#ifdef OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE

	nativeCameraLibrarySubscription_ = NativeCameraLibrary::get().initialize();

	if (!nativeCameraLibrarySubscription_)
	{
		Log::error() << "Failed to load native camera library";
	}

#endif // OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE

#ifdef OCEAN_MEDIA_ANDROID_NATIVEMEDIALIBRARY_AVAILABLE

	nativeMediaLibrarySubscription_ = NativeMediaLibrary::get().initialize();

	if (!nativeMediaLibrarySubscription_)
	{
		Log::error() << "Failed to load native media library";
	}

#endif // OCEAN_MEDIA_ANDROID_NATIVEMEDIALIBRARY_AVAILABLE
}

ALibrary::~ALibrary()
{
#ifdef OCEAN_MEDIA_ANDROID_NATIVEMEDIALIBRARY_AVAILABLE
	nativeMediaLibrarySubscription_.release();
#endif

#ifdef OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE
	nativeCameraLibrarySubscription_.release();
#endif

	releaseAudioEngine();
}

bool ALibrary::registerLibrary()
{
	return Manager::get().registerLibrary<ALibrary>(nameAndroidLibrary());
}

bool ALibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameAndroidLibrary());
}

MediumRef ALibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (String::toLower(url).find("microphone") != std::string::npos)
	{
		MediumRef medium = newMicrophone(url, useExclusive);

		if (medium)
		{
			return medium;
		}
	}

	MediumRef medium = newLiveVideo(url, useExclusive);

	if (medium.isNull())
	{
		medium = newMovie(url, useExclusive);
	}

	if (medium.isNull())
	{
		medium = newAudio(url, useExclusive);
	}

	return medium;
}

MediumRef ALibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (type == Medium::LIVE_VIDEO || type == Medium::FRAME_MEDIUM)
	{
		MediumRef medium = newLiveVideo(url, useExclusive);

		if (medium)
		{
			return medium;
		}
	}

	if (type == Medium::MOVIE || type == Medium::FRAME_MEDIUM)
	{
		MediumRef medium = newMovie(url, useExclusive);

		if (medium)
		{
			return medium;
		}
	}

	if (type == Medium::AUDIO)
	{
		return newAudio(url, useExclusive);
	}

	if (type == Medium::LIVE_AUDIO)
	{
		return newLiveAudio(url, useExclusive);
	}

	if (type == Medium::MICROPHONE)
	{
		return newMicrophone(url, useExclusive);
	}

	return MediumRef();
}

RecorderRef ALibrary::newRecorder(const Recorder::Type type)
{
#if defined(__ANDROID_API__) && __ANDROID_API__ >= 21
	if (type == Recorder::MOVIE_RECORDER)
	{
		return RecorderRef(new AMovieRecorder());
	}
#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 21

	return RecorderRef();
}

Medium::Type ALibrary::supportedTypes() const
{
	return Medium::Type(Medium::LIVE_VIDEO | Medium::MOVIE | Medium::AUDIO | Medium::LIVE_AUDIO | Medium::MICROPHONE);
}

MediumRef ALibrary::newAudio(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, name(), Medium::AUDIO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	if (slEngine_ == nullptr)
	{
		if (!initializerAudioEngine())
		{
			return MediumRef();
		}
	}

	ocean_assert(slEngineInterface_ != nullptr);

	AAudio* medium = new AAudio(slEngineInterface_, url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

MediumRef ALibrary::newLiveAudio(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, name(), Medium::LIVE_AUDIO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	if (slEngine_ == nullptr)
	{
		if (!initializerAudioEngine())
		{
			return MediumRef();
		}
	}

	ocean_assert(slEngineInterface_ != nullptr);

	ALiveAudio* medium = new ALiveAudio(slEngineInterface_, url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

MediumRef ALibrary::newMicrophone(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, name(), Medium::MICROPHONE));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	if (slEngine_ == nullptr)
	{
		if (!initializerAudioEngine())
		{
			return MediumRef();
		}
	}

	ocean_assert(slEngineInterface_ != nullptr);

	AMicrophone* medium = new AMicrophone(slEngineInterface_, url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

bool ALibrary::initializerAudioEngine()
{
	ocean_assert(slEngine_ == nullptr);

	bool noError = true;

	SLInterfaceID interfaceIds[1] = {SL_IID_ANDROIDEFFECTCAPABILITIES};
	SLboolean interfaceRequired[1] = {SL_BOOLEAN_FALSE};

	if (noError && slCreateEngine(&slEngine_, 0u, nullptr, 1u, interfaceIds, interfaceRequired) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL engine";
		noError = false;
	}

	if (noError && (*slEngine_)->Realize(slEngine_, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to realize SL engine";
		noError = false;
	}

	ocean_assert(slEngineInterface_ == nullptr);
	if (noError && (*slEngine_)->GetInterface(slEngine_, SL_IID_ENGINE, &slEngineInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL engine interface";
		noError = false;
	}

	AMedium::EffectManager::get().initialize(slEngine_);

	if (noError == false)
	{
		releaseAudioEngine();
	}

	return noError;
}

void ALibrary::releaseAudioEngine()
{
	slEngineInterface_ = nullptr;

	if (slEngine_)
	{
		(*slEngine_)->Destroy(slEngine_);
		slEngine_ = nullptr;
	}
}

MediumRef ALibrary::newLiveVideo(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameAndroidLibrary(), Medium::LIVE_VIDEO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	ALiveVideo* medium = new ALiveVideo(url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

MediumRef ALibrary::newMovie(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameAndroidLibrary(), Medium::MOVIE));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

	AMovie* medium = new AMovie(url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);

#else

	Log::warning() << "__ANDROID_API__ needs to be 24+ to load movies on Android";

	return MediumRef();

#endif // __ANDROID_API__
}

LibraryRef ALibrary::create()
{
	return LibraryRef(new ALibrary());
}

}

}

}
