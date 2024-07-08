/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFAudio.h"
#include "ocean/media/mediafoundation/MFLibrary.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFAudio::MFAudio(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	MFMedium(url),
	MFFiniteMedium(url),
	SoundMedium(url),
	MFSoundMedium(url),
	Audio(url)
{
	isValid_ = createPipeline(respectPlaybackTime_);
}

MFAudio::~MFAudio()
{
	stopMediaSession();
	releaseTopology();
}

MediumRef MFAudio::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return MFLibrary::newAudio(url_, true);
	}

	return MediumRef();
}

bool MFAudio::createTopology(const bool respectPlaybackTime)
{
	ocean_assert(respectPlaybackTime);
	if (!respectPlaybackTime)
	{
		ocean_assert(false && "MFAudio does not support ignoring playback time");
		return false;
	}

	if (topology_.isValid())
	{
		return true;
	}

	if (S_OK != MFCreateTopology(&topology_.resetObject()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (buildSoundTopology())
	{
		ocean_assert(mediaSession_.isValid());

		if (S_OK == mediaSession_->SetTopology(0, *topology_))
		{
			return true;
		}
	}

	releaseTopology();
	return false;
}

void MFAudio::releaseTopology()
{
	releaseSoundTopology();
	MFMedium::releaseTopology();
}

}

}

}
