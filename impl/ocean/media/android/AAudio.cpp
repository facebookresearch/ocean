/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/AAudio.h"

#include "ocean/base/String.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"

#include <SLES/OpenSLES_Android.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

AAudio::AAudio(const SLEngineItf& slEngineInterface, const std::string& url) :
	Medium(url),
	AMedium(url),
	FiniteMedium(url),
	SoundMedium(url),
	Audio(url)
{
	libraryName_ = nameAndroidLibrary();

	isValid_ = initialize(slEngineInterface);
}

AAudio::~AAudio()
{
	release();
}

MediumRef AAudio::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return Manager::get().newMedium(url_, libraryName_, AUDIO, true);
	}

	return MediumRef();
}

bool AAudio::start()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	if (slPlayInterface_ != nullptr)
	{
		if ((*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toNow();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toInvalid();

			return true;
		}
	}

	return false;
}

bool AAudio::pause()
{
	const ScopedLock scopedLock(lock_);

	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	if (slPlayInterface_ != nullptr)
	{
		if ((*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_PAUSED) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toNow();
			stopTimestamp_.toInvalid();

			return true;
		}
	}

	return false;
}

bool AAudio::stop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	if (slPlayInterface_ != nullptr)
	{
		if ((*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_STOPPED) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toNow();

			return true;
		}
	}

	return false;
}

bool AAudio::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

double AAudio::duration() const
{
	const ScopedLock scopedLock(lock_);

	if (slPlayInterface_ != nullptr)
	{
		SLmicrosecond durationMilliseconds = 0u;
		if ((*slPlayInterface_)->GetDuration(slPlayInterface_, &durationMilliseconds) == SL_RESULT_SUCCESS)
		{
			return double(durationMilliseconds) * 0.001;
		}
	}

	return 0.0;
}

double AAudio::normalDuration() const
{
	return duration();
}

double AAudio::position() const
{
	const ScopedLock scopedLock(lock_);

	if (slPlayInterface_ != nullptr)
	{
		SLmicrosecond durationMilliseconds = 0u;
		if ((*slPlayInterface_)->GetPosition(slPlayInterface_, &durationMilliseconds) == SL_RESULT_SUCCESS)
		{
			return double(durationMilliseconds) * 0.001;
		}
	}

	return -1.0;
}

bool AAudio::setPosition(const double position)
{
	const ScopedLock scopedLock(lock_);

	if (slSeekInterface_ != nullptr)
	{
		const SLmicrosecond durationMilliseconds = SLmicrosecond(position * 1000.0 + 0.5);

		if ((*slSeekInterface_)->SetPosition(slSeekInterface_, durationMilliseconds, SL_SEEKMODE_FAST) == SL_RESULT_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

float AAudio::speed() const
{
	return 1.0f;
}

bool AAudio::setSpeed(const float speed)
{
	if (speed == 1.0f)
	{
		return true;
	}

	return false;
}

bool AAudio::setLoop(bool value)
{
	const ScopedLock scopedLock(lock_);

	if (slSeekInterface_ != nullptr)
	{
		if ((*slSeekInterface_)->SetLoop(slSeekInterface_, value ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN) == SL_RESULT_SUCCESS)
		{
			return FiniteMedium::setLoop(value);
		}
	}

	return false;
}

float AAudio::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		SLmillibel millibel = 0;
		if ((*slVolumeInterface_)->GetVolumeLevel(slVolumeInterface_, &millibel) != SL_RESULT_SUCCESS)
		{
			return -1.0f;
		}

		return float(millibel) * 0.01f;
	}

	return -1.0f;
}

bool AAudio::soundMute() const
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		SLboolean mute = SL_BOOLEAN_FALSE;
		if ((*slVolumeInterface_)->GetMute(slVolumeInterface_, &mute) != SL_RESULT_SUCCESS)
		{
			return false;
		}

		return mute == SL_BOOLEAN_TRUE ? true : false;
	}

	return false;
}

bool AAudio::setSoundVolume(const float volume)
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		if ((*slVolumeInterface_)->SetVolumeLevel(slVolumeInterface_, SLmillibel(volume * 100.0f)) == SL_RESULT_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

bool AAudio::setSoundMute(const bool mute)
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		if ((*slVolumeInterface_)->SetMute(slVolumeInterface_, mute ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE) == SL_RESULT_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

Timestamp AAudio::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp AAudio::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp AAudio::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

bool AAudio::initialize(const SLEngineItf& slEngineInterface)
{
	bool noError = true;

	SLEngineItf slEngineInterface_(slEngineInterface);

	ocean_assert(slOutputMix_ == nullptr);
	if (noError && (*slEngineInterface_)->CreateOutputMix(slEngineInterface_, &slOutputMix_, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL output mix";
		noError = false;
	}

	if (noError && (*slOutputMix_)->Realize(slOutputMix_, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to realize SL output mix";
		noError = false;
	}

	const std::string localUrl(url());

	if (String::toLower(localUrl).find("http") == std::string::npos)
	{
		if (!IO::File(localUrl).exists())
		{
			Log::warning() << "The given audio url '" << localUrl << "' does not exist";
			return false;
		}
	}

	SLDataLocator_URI slDataLocatorUrl = {SL_DATALOCATOR_URI, (SLchar*)(localUrl.c_str())};
	SLDataFormat_MIME slDataFormatMime = {SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED};
	SLDataSource slAudioSource = {&slDataLocatorUrl, &slDataFormatMime};

	ocean_assert(slOutputMix_ != nullptr);
	SLDataLocator_OutputMix dataLocatorOutputMix = {SL_DATALOCATOR_OUTPUTMIX, slOutputMix_};
	SLDataSink slAudioSink = {&dataLocatorOutputMix, nullptr};

	const SLInterfaceID interfaceIds[2] = {SL_IID_SEEK, SL_IID_VOLUME};
	const SLboolean interfaceRequired[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

	ocean_assert(slPlayer_ == nullptr);
	if (noError && (*slEngineInterface_)->CreateAudioPlayer(slEngineInterface_, &slPlayer_, &slAudioSource, &slAudioSink, 2u, interfaceIds, interfaceRequired) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio player";
		noError = false;
	}

	if (noError && (*slPlayer_)->Realize(slPlayer_, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to realize SL audio player";
		noError = false;
	}

	ocean_assert(slPlayInterface_ == nullptr);
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_PLAY, &slPlayInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio player interface";
		noError = false;
	}

	if (noError && (*slPlayInterface_)->RegisterCallback(slPlayInterface_, staticCallbackFunction, this))
	{
		Log::error() << "Failed to register SL callback function";
		noError = false;
	}

	if (noError && (*slPlayInterface_)->SetCallbackEventsMask(slPlayInterface_, SL_PLAYEVENT_HEADATEND))
	{
		Log::error() << "Failed to register SL callback function";
		noError = false;
	}

	ocean_assert(slSeekInterface_ == nullptr);
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_SEEK, &slSeekInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL seek interface";
		noError = false;
	}

	ocean_assert(slVolumeInterface_ == nullptr);
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_VOLUME, &slVolumeInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL volume interface";
		noError = false;
	}

#ifdef OCEAN_DEBUG

	SLmillibel maxVolumeLevel = 0;
	if (noError && (*slVolumeInterface_)->GetMaxVolumeLevel(slVolumeInterface_, &maxVolumeLevel) == SL_RESULT_SUCCESS)
	{
		Log::debug() << "Volume level range [" << float(SL_MILLIBEL_MIN) * 0.1f << ", " << float(maxVolumeLevel) * 0.1f << "] db";
	}

#endif

	if (noError == false)
	{
		release();
	}

	return noError;
}

bool AAudio::release()
{
	slVolumeInterface_ = nullptr;
	slSeekInterface_ = nullptr;
	slPlayInterface_ = nullptr;

	if (slPlayer_)
	{
		(*slPlayer_)->Destroy(slPlayer_);
		slPlayer_ = nullptr;
	}

	if (slOutputMix_)
	{
		(*slOutputMix_)->Destroy(slOutputMix_);
		slOutputMix_ = nullptr;
	}

	return true;
}

void AAudio::callbackFunction(SLPlayItf slCaller, SLuint32 slEvent)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(slPlayInterface_ == slCaller);

	switch (slEvent)
	{
		case SL_PLAYEVENT_HEADATEND:
		{
			if (slPlayInterface_ == nullptr || (*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_STOPPED) != SL_RESULT_SUCCESS)
			{
				Log::error() << "Failed to stop player";
			}

			startTimestamp_.toInvalid();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toNow();
			break;
		}

		default:
			// not of interest
			break;
	}
}

void AAudio::staticCallbackFunction(SLPlayItf slCaller, void* context, SLuint32 slEvent)
{
	ocean_assert(context != nullptr);

	AAudio* audio = static_cast<AAudio*>(context);

	if (audio != nullptr)
	{
		audio->callbackFunction(slCaller, slEvent);
	}
}

}

}

}
