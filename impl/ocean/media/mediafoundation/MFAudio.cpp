// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
	isValid_ = createPipeline();
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

bool MFAudio::createTopology()
{
	if (topology_.object() != nullptr)
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
		ocean_assert(mediaSession_.object() != nullptr);

		if (S_OK == mediaSession_.object()->SetTopology(0, topology_.object()))
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
