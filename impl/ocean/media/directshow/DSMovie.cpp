/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSMovie.h"
#include "ocean/media/directshow/DSLibrary.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSMovie::DSMovie(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	DSMedium(url),
	DSFiniteMedium(url),
	FrameMedium(url),
	DSFrameMedium(url),
	SoundMedium(url),
	DSSoundMedium(url),
	Movie(url)
{
	isValid_ = buildGraph();
}

DSMovie::~DSMovie()
{
	releaseGraph();
}

bool DSMovie::setUseSound(const bool state)
{
	if (isStarted())
	{
		return false;
	}

	if (state)
	{
		ocean_assert(movieUseSound_);
		return true;
	}

	if (!state && removeSoundBranch())
	{
		movieUseSound_ = false;
		return true;
	}

	return false;
}

bool DSMovie::useSound() const
{
	return movieUseSound_;
}

MediumRef DSMovie::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return DSLibrary::newMovie(url_, true);
	}

	return MediumRef();
}

bool DSMovie::buildGraph()
{
	if (filterGraph_.isValid())
	{
		return true;
	}

	if (!createGraphBuilder())
	{
		return false;
	}

	ScopedFunctionVoid scopedReleaseGraphFunction(std::bind(&DSMovie::releaseGraph, this)); // scoped function which will be invoked in case we don't reach the both of this function

	const HRESULT renderResult = filterGraph_->RenderFile(String::toWString(url_).c_str(), nullptr);

	switch (renderResult)
	{
		case S_OK:
		case VFW_S_DUPLICATE_NAME:
			break;

		case VFW_S_AUDIO_NOT_RENDERED:
			Log::warning() << "Partial success; the audio was not rendered.";
			break;

		case VFW_S_PARTIAL_RENDER:
			Log::warning() << "Some of the streams in this movie are in an unsupported format.";
			break;

		case VFW_S_VIDEO_NOT_RENDERED:
			Log::warning() << "Partial success; some of the streams in this movie are in an unsupported format.";
			break;

		case E_ABORT:
		case E_FAIL:
			return false;

		case E_INVALIDARG:
		case E_POINTER:
			ocean_assert(false && "Argument is invalid.");
			break;

		case E_OUTOFMEMORY:
			Log::error() << "Insufficient memory.";
			return false;

		case VFW_E_CANNOT_CONNECT:
			Log::warning() << "No combination of intermediate filters could be found to make the connection.";
			break;

		case VFW_E_CANNOT_LOAD_SOURCE_FILTER:
			Log::warning() << "The source filter for this file could not be loaded.";
			return false;

		case VFW_E_CANNOT_RENDER:
			Log::warning() << "No combination of filters could be found to render the stream.";
			return false;

		case VFW_E_INVALID_FILE_FORMAT:
			Log::warning() << "The file format is invalid.";
			return false;

		case VFW_E_NOT_FOUND:
			Log::warning() << "An object or name was not found.";
			return false;

		case VFW_E_UNKNOWN_FILE_TYPE:
			Log::warning() << "The media type of this file is not recognized.";
			return false;

		case VFW_E_UNSUPPORTED_STREAM:
			Log::warning() << "Cannot play back the file: the format is not supported.";
			return false;
	}

	createSoundInterface();

	if (!createFrameSampleSinkFilter())
	{
		Log::error() << "Could not create the sample sink filter branch.";

		return false;
	}

	if (!createFiniteInterface())
	{
		Log::error() << "The finite interfaces could not be created.";

		return false;
	}

	scopedReleaseGraphFunction.revoke(); // we don't want the release function to be called

	return true;
}

void DSMovie::releaseGraph()
{
	stopGraph();

	releaseFiniteInterface();
	releaseSoundInterface();
	releaseFrameSampleSinkFilter();
	releaseGraphBuilder();
}

}

}

}
