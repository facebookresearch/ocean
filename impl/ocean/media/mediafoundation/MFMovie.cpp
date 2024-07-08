/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFMovie.h"
#include "ocean/media/mediafoundation/MFLibrary.h"
#include "ocean/media/mediafoundation/Utilities.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFMovie::MFMovie(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	MFMedium(url),
	MFFiniteMedium(url),
	FrameMedium(url),
	MFFrameMedium(url),
	SoundMedium(url),
	MFSoundMedium(url),
	Movie(url)
{
	isValid_ = createPipeline(respectPlaybackTime_);
}

MFMovie::~MFMovie()
{
	const bool stopResult = stopMediaSession();
	ocean_assert_and_suppress_unused(stopResult, stopResult);

	releasePipeline();
}

bool MFMovie::setUseSound(const bool state)
{
	const ScopedLock scopedLock(lock_);

	if (useSound_ == state)
	{
		return true;
	}

	if (startTimestamp_.isValid())
	{
		return false;
	}

	releasePipeline();

	useSound_ = state;

	return createPipeline(respectPlaybackTime_);
}

bool MFMovie::useSound() const
{
	const ScopedLock scopedLock(lock_);

	return useSound_;
}

MediumRef MFMovie::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return MFLibrary::newMovie(url_, true);
	}

	return MediumRef();
}

bool MFMovie::createTopology(const bool respectPlaybackTime)
{
	if (topology_.isValid())
	{
		return true;
	}

	if (S_OK != MFCreateTopology(&topology_.resetObject()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (buildFrameTopology(respectPlaybackTime))
	{
		if (useSound_)
		{
			buildSoundTopology();
		}

		ocean_assert(mediaSession_.isValid());

		if (S_OK == mediaSession_->SetTopology(0, *topology_))
		{
			return true;
		}
	}

	releaseTopology();
	return false;
}

void MFMovie::releaseTopology()
{
	releaseFrameTopology();
	releaseSoundTopology();

	MFMedium::releaseTopology();
}

}

}

}
