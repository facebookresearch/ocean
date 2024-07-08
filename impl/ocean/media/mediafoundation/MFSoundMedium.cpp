/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFSoundMedium.h"
#include "ocean/media/mediafoundation/Utilities.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFSoundMedium::MFSoundMedium(const std::string& url) :
	Medium(url),
	MFMedium(url),
	SoundMedium(url)
{
	// nothing to do here
}

MFSoundMedium::~MFSoundMedium()
{
	// nothing to do here
}

float MFSoundMedium::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(mediaSession_.isValid());

	if (!mediaSession_.isValid())
	{
		return false;
	}

	float volume = -100.0f;

	ScopedIMFAudioStreamVolume audioStreamVolume;

	if (S_OK == MFGetService(*mediaSession_, MR_STREAM_VOLUME_SERVICE, IID_IMFAudioStreamVolume, (void**)(&audioStreamVolume.resetObject())))
	{
		float level = 0.0f;
		UINT32 channels = 0;

		if (S_OK == audioStreamVolume->GetChannelCount(&channels))
		{
			for (UINT32 i = 0; i < channels; ++i)
			{
				float cLevel = 0.0;

				if (S_OK == audioStreamVolume->GetChannelVolume(i, &cLevel))
				{
					level = max(level, cLevel);
				}
			}

			if (level > 0)
			{
				volume = 20 * NumericF::log10(level);
			}
		}
	}

	return volume;
}

bool MFSoundMedium::soundMute() const
{
	const ScopedLock scopedLock(lock_);

	return soundVolume() <= -100.0f;
}

bool MFSoundMedium::setSoundVolume(const float volume)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(mediaSession_.isValid());

	if (!mediaSession_.isValid())
	{
		return false;
	}

	bool success = false;

	ScopedIMFAudioStreamVolume audioStreamVolume;

	if (S_OK == MFGetService(*mediaSession_, MR_STREAM_VOLUME_SERVICE, IID_IMFAudioStreamVolume, (void**)(&audioStreamVolume.resetObject())))
	{
		float level = NumericF::pow(10.0f, volume * 0.05f);
		UINT32 channels = 0;

		if (S_OK == audioStreamVolume->GetChannelCount(&channels))
		{
			for (UINT32 i = 0; i < channels; ++i)
			{
				success = S_OK == audioStreamVolume->SetChannelVolume(i, level) && success;
			}
		}
	}

	return success;
}

bool MFSoundMedium::setSoundMute(const bool mute)
{
	return setSoundVolume(mute ? -100.0f : 0.0f);
}

bool MFSoundMedium::extractSoundFormat(IMFMediaType* mediaType, SoundType& soundType)
{
	GUID majorType = GUID_NULL;
	if (S_OK != mediaType->GetMajorType(&majorType) || majorType != MFMediaType_Audio)
	{
		return false;
	}

	GUID mediaSubType = GUID_NULL;
	if (S_OK != mediaType->GetGUID(MF_MT_SUBTYPE, &mediaSubType))
	{
		return false;
	}

	if (mediaSubType == MFAudioFormat_PCM)
	{
		UINT32 channels = 0u;
		mediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels);
		soundType.setChannels(channels);

		UINT32 bitsPerSample = 0u;
		mediaType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample);
		soundType.setBitsPerSoundSample(bitsPerSample);

		UINT32 samplesPerSecond = 0u;
		mediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &samplesPerSecond);
		soundType.setFrequency(SoundFrequency(samplesPerSecond));

		return true;
	}

	return false;
}

bool MFSoundMedium::buildSoundTopology()
{
	ScopedIMFActivate sinkActivate;

	if (S_OK != MFCreateAudioRendererActivate(&sinkActivate.resetObject()))
	{
		return false;
	}

	if (!Media::MediaFoundation::Utilities::connectSelectedStream(*topology_, *mediaSource_, *sinkActivate, MFMediaType_Audio))
	{
		return false;
	}

	return true;
}

void MFSoundMedium::releaseSoundTopology()
{
	// nothing to do here
}

}

}

}
