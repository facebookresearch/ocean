/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSAudio.h"
#include "ocean/media/directshow/DSLibrary.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSAudio::DSAudio(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	DSMedium(url),
	DSFiniteMedium(url),
	SoundMedium(url),
	DSSoundMedium(url),
	Audio(url)
{
	isValid_ = buildGraph();
}

DSAudio::~DSAudio()
{
	releaseGraph();
}

MediumRef DSAudio::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return DSLibrary::newAudio(url_, true);
	}

	return MediumRef();
}

bool DSAudio::buildGraph()
{
	if (filterGraph_.isValid())
	{
		return true;
	}

	if (!createGraphBuilder())
	{
		return false;
	}

	ScopedFunctionVoid scopedReleaseGraphFunction(std::bind(&DSAudio::releaseGraph, this)); // scoped function which will be invoked in case we don't reach the both of this function

	HRESULT renderResult = filterGraph_->RenderFile(String::toWString(url_).c_str(), nullptr);

	if (renderResult != S_OK)
	{
		if (renderResult == VFW_E_NOT_FOUND)
		{
			Log::error() << "Could not find the sound file \"" << url_ << "\".";
		}

		return false;
	}

	if (!createSoundInterface())
	{
		Log::error() << "The sound \"" << url_ << "\" holds no audio.";

		return false;
	}

	if (!createFiniteInterface())
	{
		Log::error() << "The finite interfaces could not be created.";

		return false;
	}

	if (!removeVideoBranch())
	{
		Log::error() << "Could not remove the video filter branch.";

		return false;
	}

	scopedReleaseGraphFunction.revoke(); // we don't want the release function to be called

	return true;
}

void DSAudio::releaseGraph()
{
	stopGraph();

	releaseFiniteInterface();
	releaseSoundInterface();
	releaseGraphBuilder();
}

}

}

}
