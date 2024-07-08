/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
	ocean_assert(!soundBasicAudioInterface_.isValid());
	ocean_assert(!soundVideoSuppressionNullRendererFilter_.isValid());
	ocean_assert(!soundVideoSuppressionNullRendererFilter2_.isValid());
}

float DSSoundMedium::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	if (!soundBasicAudioInterface_.isValid())
	{
		return -100.0;
	}

	// In the mute state the previous sound volume is returned
	if (soundNonMuteVolume_ != NumericF::minValue())
	{
		return soundNonMuteVolume_;
	}

	long volume = 0;
	if (S_OK != soundBasicAudioInterface_->get_Volume(&volume))
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

	if (!soundBasicAudioInterface_.isValid())
	{
		return false;
	}

	soundNonMuteVolume_ = NumericF::minValue();
	return S_OK == soundBasicAudioInterface_->put_Volume(long(volume * 100.0f));
}

bool DSSoundMedium::setSoundMute(const bool mute)
{
	const ScopedLock scopedLock(lock_);

	if (!soundBasicAudioInterface_.isValid())
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
		return S_OK == soundBasicAudioInterface_->put_Volume(-10000l);
	}
	else
	{
		// check whether not muted
		if (soundNonMuteVolume_ == NumericF::minValue())
		{
			return true;
		}

		const bool result = S_OK == soundBasicAudioInterface_->put_Volume(long(soundNonMuteVolume_ * 100.0f));
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
	ocean_assert(filterGraph_.isValid());
	ocean_assert(graphIsRunning_ == false);

	ScopedIBaseFilter soundDeviceFilter;
	if (S_OK != filterGraph_->FindFilterByName(L"Default DirectSound Device", &soundDeviceFilter.resetObject()))
	{
		Log::info() << "\"" << url_ << "\" holds no sound.";

		return false;
	}

	if (!initializeSoundInterface(*soundDeviceFilter))
	{
		Log::error() << "Could not initialize sound interface.";

		return false;
	}

	const ScopedIPin soundDeviceFilterInputPin = firstPin(*soundDeviceFilter, PTYPE_INPUT, CTYPE_CONNECTED);
	if (soundDeviceFilterInputPin.isValid())
	{
		DSMediaType mediaType;
		soundDeviceFilterInputPin->ConnectionMediaType(&mediaType.reset());

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

	if (soundBasicAudioInterface_.isValid())
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
	if (soundVideoSuppressionNullRendererFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*soundVideoSuppressionNullRendererFilter_);
	}

	if (soundVideoSuppressionNullRendererFilter2_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*soundVideoSuppressionNullRendererFilter2_);
	}

	soundBasicAudioInterface_.release();
	soundVideoSuppressionNullRendererFilter_.release();
	soundVideoSuppressionNullRendererFilter2_.release();
}

bool DSSoundMedium::removeVideoBranch()
{
	ocean_assert(filterGraph_.isValid());

	ScopedIBaseFilter videoRendererFilter;
	if (S_OK != filterGraph_->FindFilterByName(L"Video Renderer", &videoRendererFilter.resetObject()))
	{
		if (S_OK != filterGraph_->FindFilterByName(L"Frame sample sink filter", &videoRendererFilter.resetObject()))
		{
			return true;
		}
	}

	ScopedIBaseFilter soundRendererFilter;
	if (S_OK != filterGraph_->FindFilterByName(L"Default DirectSound Device", &soundRendererFilter.resetObject()))
	{
		return false;
	}

	Filters soundBranch;
	soundBranch.emplace_back(std::move(soundRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(*soundBranch.back(), PTYPE_INPUT, CTYPE_CONNECTED);
		const ScopedIPin connected = connectedPin(*pin);

		if (!connected.isValid())
		{
			break;
		}

		soundBranch.emplace_back(pinOwner(*connected));
		ocean_assert(soundBranch.back().isValid());
	}

	ScopedIPin videoOutputPin;
	ScopedIBaseFilter videoFilter(std::move(videoRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(*videoFilter, PTYPE_INPUT, CTYPE_CONNECTED);
		if (!pin.isValid())
		{
			break;
		}

		ScopedIPin connected = connectedPin(*pin);

		filterGraph_->RemoveFilter(*videoFilter);

		videoFilter = pinOwner(*connected);

		if (!videoFilter.isValid())
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

	if (!videoOutputPin.isValid())
	{
		Log::error() << "Could not find a video output pin";

		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)(&soundVideoSuppressionNullRendererFilter_.resetObject())))
	{
		Log::error() << "Could not create a sound null renderer filter.";

		return false;
	}

	if (S_OK != filterGraph_->AddFilter(*soundVideoSuppressionNullRendererFilter_, L"Video Null Renderer Filter"))
	{
		Log::error() << "Could not insert the sound null renderer filter.";

		return false;
	}

	if (!connectFilter(*videoOutputPin, *soundVideoSuppressionNullRendererFilter_))
	{
		Log::error() << "Could not connect the sound null renderer filter for video suppression.";

		return false;
	}

	return true;
}

bool DSSoundMedium::removeSoundBranch()
{
	ocean_assert(filterGraph_.isValid());

	ScopedIBaseFilter soundRendererFilter;
	if (S_OK != filterGraph_->FindFilterByName(L"Default DirectSound Device", &soundRendererFilter.resetObject()))
	{
		return true;
	}

	ScopedIBaseFilter videoRendererFilter;
	if (S_OK != filterGraph_->FindFilterByName(L"Video Renderer", &videoRendererFilter.resetObject()))
	{
		if (S_OK != filterGraph_->FindFilterByName(L"Frame sample sink filter", &videoRendererFilter.resetObject()))
		{
			return true;
		}
	}

	ScopedIPin soundOutputPin;

	Filters videoBranch;
	videoBranch.emplace_back(std::move(videoRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(*videoBranch.back(), PTYPE_INPUT, CTYPE_CONNECTED);
		const ScopedIPin connected = connectedPin(*pin);

		if (!connected.isValid())
		{
			break;
		}

		videoBranch.emplace_back(pinOwner(*connected));
		ocean_assert(videoBranch.back().isValid());
	}

	ScopedIBaseFilter soundFilter(std::move(soundRendererFilter));

	while (true)
	{
		const ScopedIPin pin = firstPin(*soundFilter, PTYPE_INPUT, CTYPE_CONNECTED);
		if (!pin.isValid())
		{
			break;
		}

		ScopedIPin connected = connectedPin(*pin);

		filterGraph_->RemoveFilter(*soundFilter);

		soundFilter = pinOwner(*connected);

		if (!soundFilter.isValid())
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

	if (soundOutputPin.isValid())
	{
		Log::error() << "Failed to determine sound output pin";

		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)(&soundVideoSuppressionNullRendererFilter2_.resetObject())))
	{
		Log::error() << "Could not create a sound null renderer filter.";

		return false;
	}

	if (S_OK != filterGraph_->AddFilter(*soundVideoSuppressionNullRendererFilter2_, L"Sound Null Renderer Filter"))
	{
		Log::error() << "Could not insert the sound null renderer filter.";

		return false;
	}

	if (!connectFilter(*soundOutputPin, *soundVideoSuppressionNullRendererFilter2_))
	{
		Log::error() << "Could not connect the sound null renderer filter for video suppression.";

		return false;
	}

	return true;
}

}

}

}
