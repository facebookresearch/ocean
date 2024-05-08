// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/media/directshow/DSSoundMedium.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

EXTERN_C const CLSID CLSID_NullRenderer;

DSSoundMedium::DSSortableSoundType::DSSortableSoundType(DSMediaType&& dsMediaType, const SoundType& soundType) :
	SortableSoundType(soundType),
	dsMediaType_(std::move(dsMediaType))
{
	DSSoundMedium::extractSoundFormat(dsMediaType_.type(), actualSoundType);
}

const AM_MEDIA_TYPE& DSSoundMedium::DSSortableSoundType::type() const
{
	return dsMediaType_.type();
}

DSSoundMedium::DSSoundMedium(const std::string& url) :
	Medium(url),
	DSMedium(url),
	SoundMedium(url)
{
	// nothing to do here
}

DSSoundMedium::~DSSoundMedium()
{
	ocean_assert(soundBasicAudioInterface_.object() == nullptr);
	ocean_assert(soundVideoSuppressionNullRendererFilter_.object() == nullptr);
	ocean_assert(soundVideoSuppressionNullRendererFilter2_.object() == nullptr);
}

float DSSoundMedium::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	if (soundBasicAudioInterface_.object() == nullptr)
	{
		return -100.0;
	}

	// In the mute state the previous sound volume is returned
	if (soundNonMuteVolume_ != NumericF::minValue())
	{
		return soundNonMuteVolume_;
	}

	long volume = 0;
	if (S_OK != soundBasicAudioInterface_.object()->get_Volume(&volume))
	{
		return -100.0;
	}

	return volume * 0.01f;
}

bool DSSoundMedium::soundMute() const
{
	const ScopedLock scopedLock(lock_);

	return soundNonMuteVolume_ != NumericF::minValue();
}

bool DSSoundMedium::setSoundVolume(const float volume)
{
	const ScopedLock scopedLock(lock_);

	if (soundBasicAudioInterface_.object() == nullptr)
	{
		return false;
	}

	soundNonMuteVolume_ = NumericF::minValue();
	return S_OK == soundBasicAudioInterface_.object()->put_Volume(long(volume * 100.0f));
}

bool DSSoundMedium::setSoundMute(const bool mute)
{
	const ScopedLock scopedLock(lock_);

	if (soundBasicAudioInterface_.object() == nullptr)
	{
		return false;
	}

	if (mute)
	{
		// check whether already muted
		if (soundNonMuteVolume_ != NumericF::minValue())
		{
			return true;
		}

		soundNonMuteVolume_ = soundVolume();
		return S_OK == soundBasicAudioInterface_.object()->put_Volume(-10000l);
	}
	else
	{
		// check whether not muted
		if (soundNonMuteVolume_ == NumericF::minValue())
		{
			return true;
		}

		const bool result = S_OK == soundBasicAudioInterface_.object()->put_Volume(long(soundNonMuteVolume_ * 100.0f));
		soundNonMuteVolume_ = NumericF::minValue();

		return result;
	}
}

bool DSSoundMedium::extractSoundFormat(const AM_MEDIA_TYPE& mediaType, SoundType& soundType)
{
	if (mediaType.formattype == FORMAT_WaveFormatEx)
	{
		ocean_assert(sizeof(WAVEFORMATEX) <= mediaType.cbFormat);
		const WAVEFORMATEX& waveInfo = *(WAVEFORMATEX*)mediaType.pbFormat;

		soundType.setChannels(waveInfo.nChannels);
		soundType.setFrequency(SoundFrequency(waveInfo.nSamplesPerSec));
		soundType.setBitsPerSoundSample(waveInfo.wBitsPerSample);
	}
	else
	{
		return false;
	}

	return true;
}

bool DSSoundMedium::createSoundInterface()
{
	ocean_assert(filterGraph_.object()!= nullptr);
	ocean_assert(graphIsRunning_ == false);

	ScopedIBaseFilter soundDeviceFilter;
	if (S_OK != filterGraph_.object()->FindFilterByName(L"Default DirectSound Device", &soundDeviceFilter.resetObject()))
	{
		Log::info() << "\"" << url_ << "\" holds no sound.";

		return false;
	}

	if (!initializeSoundInterface(soundDeviceFilter.object()))
	{
		Log::error() << "Could not initialize sound interface.";

		return false;
	}

	const ScopedIPin soundDeviceFilterInputPin = firstPin(soundDeviceFilter.object(), PTYPE_INPUT, CTYPE_CONNECTED);
	if (soundDeviceFilterInputPin.object() != nullptr)
	{
		DSMediaType mediaType;
		soundDeviceFilterInputPin.object()->ConnectionMediaType(&mediaType.reset());

		if (extractSoundFormat(mediaType.type(), mediumSoundType))
		{
			mediumSoundTimestamp.toNow();
		}
	}

	return true;
}

bool DSSoundMedium::initializeSoundInterface(IBaseFilter* filter)
{
	ocean_assert(filter != nullptr);

	if (soundBasicAudioInterface_.object() != nullptr)
	{
		return true;
	}

	const bool result = S_OK == filter->QueryInterface(IID_IBasicAudio, (void**)(&soundBasicAudioInterface_.resetObject()));

	if (result == false)
	{
		releaseSoundInterface();
	}

	return result;
}

void DSSoundMedium::releaseSoundInterface()
{
	if (soundVideoSuppressionNullRendererFilter_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(soundVideoSuppressionNullRendererFilter_.object());
	}

	if (soundVideoSuppressionNullRendererFilter2_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(soundVideoSuppressionNullRendererFilter2_.object());
	}

	soundBasicAudioInterface_.release();
	soundVideoSuppressionNullRendererFilter_.release();
	soundVideoSuppressionNullRendererFilter2_.release();
}

bool DSSoundMedium::removeVideoBranch()
{
	ocean_assert(filterGraph_.object() != nullptr);

	ScopedIBaseFilter videoRendererFilter;
	if (S_OK != filterGraph_.object()->FindFilterByName(L"Video Renderer", &videoRendererFilter.resetObject()))
	{
		if (S_OK != filterGraph_.object()->FindFilterByName(L"Frame sample sink filter", &videoRendererFilter.resetObject()))
		{
			return true;
		}
	}

	ScopedIBaseFilter soundRendererFilter;
	if (S_OK != filterGraph_.object()->FindFilterByName(L"Default DirectSound Device", &soundRendererFilter.resetObject()))
	{
		return false;
	}

	Filters soundBranch;
	soundBranch.emplace_back(std::move(soundRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(soundBranch.back().object(), PTYPE_INPUT, CTYPE_CONNECTED);
		const ScopedIPin connected = connectedPin(pin.object());

		if (connected.object() == nullptr)
		{
			break;
		}

		soundBranch.emplace_back(pinOwner(connected.object()));
		ocean_assert(soundBranch.back().object() != nullptr);
	}

	ScopedIPin videoOutputPin;
	ScopedIBaseFilter videoFilter(std::move(videoRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(videoFilter.object(), PTYPE_INPUT, CTYPE_CONNECTED);
		if (pin.object() == nullptr)
		{
			break;
		}

		ScopedIPin connected = connectedPin(pin.object());

		filterGraph_.object()->RemoveFilter(videoFilter.object());

		videoFilter = pinOwner(connected.object());

		if (videoFilter.object() == nullptr)
		{
			break;
		}

		bool found = false;
		for (const ScopedIBaseFilter& filter : soundBranch)
		{
			if (videoFilter.object() == filter.object())
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			videoOutputPin = std::move(connected);
			break;
		}
	}

	if (videoOutputPin.object() == nullptr)
	{
		Log::error() << "Could not find a video output pin";

		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)(&soundVideoSuppressionNullRendererFilter_.resetObject())))
	{
		Log::error() << "Could not create a sound null renderer filter.";

		return false;
	}

	if (S_OK != filterGraph_.object()->AddFilter(soundVideoSuppressionNullRendererFilter_.object(), L"Video Null Renderer Filter"))
	{
		Log::error() << "Could not insert the sound null renderer filter.";

		return false;
	}

	if (!connectFilter(videoOutputPin.object(), soundVideoSuppressionNullRendererFilter_.object()))
	{
		Log::error() << "Could not connect the sound null renderer filter for video suppression.";

		return false;
	}

	return true;
}

bool DSSoundMedium::removeSoundBranch()
{
	ocean_assert(filterGraph_.object() != nullptr);

	ScopedIBaseFilter soundRendererFilter;
	if (S_OK != filterGraph_.object()->FindFilterByName(L"Default DirectSound Device", &soundRendererFilter.resetObject()))
	{
		return true;
	}

	ScopedIBaseFilter videoRendererFilter;
	if (S_OK != filterGraph_.object()->FindFilterByName(L"Video Renderer", &videoRendererFilter.resetObject()))
	{
		if (S_OK != filterGraph_.object()->FindFilterByName(L"Frame sample sink filter", &videoRendererFilter.resetObject()))
		{
			return true;
		}
	}

	ScopedIPin soundOutputPin;

	Filters videoBranch;
	videoBranch.emplace_back(std::move(videoRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(videoBranch.back().object(), PTYPE_INPUT, CTYPE_CONNECTED);
		const ScopedIPin connected = connectedPin(pin.object());

		if (connected.object() == nullptr)
		{
			break;
		}

		videoBranch.emplace_back(pinOwner(connected.object()));
		ocean_assert(videoBranch.back().object() != nullptr);
	}

	ScopedIBaseFilter soundFilter(std::move(soundRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(soundFilter.object(), PTYPE_INPUT, CTYPE_CONNECTED);
		if (pin.object() == nullptr)
		{
			break;
		}

		ScopedIPin connected = connectedPin(pin.object());

		filterGraph_.object()->RemoveFilter(soundFilter.object());

		soundFilter = pinOwner(connected.object());

		if (soundFilter.object() == nullptr)
		{
			break;
		}

		bool found = false;
		for (const ScopedIBaseFilter& filter : videoBranch)
		{
			if (soundFilter.object() == filter.object())
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			soundOutputPin = std::move(connected);
			break;
		}
	}

	soundBasicAudioInterface_.release();

	if (soundOutputPin.object() != nullptr)
	{
		Log::error() << "Failed to determine sound output pin";

		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)(&soundVideoSuppressionNullRendererFilter2_.resetObject())))
	{
		Log::error() << "Could not create a sound null renderer filter.";

		return false;
	}

	if (S_OK != filterGraph_.object()->AddFilter(soundVideoSuppressionNullRendererFilter2_.object(), L"Sound Null Renderer Filter"))
	{
		Log::error() << "Could not insert the sound null renderer filter.";

		return false;
	}

	if (!connectFilter(soundOutputPin.object(), soundVideoSuppressionNullRendererFilter2_.object()))
	{
		Log::error() << "Could not connect the sound null renderer filter for video suppression.";

		return false;
	}

	return true;
}

}

}

}
