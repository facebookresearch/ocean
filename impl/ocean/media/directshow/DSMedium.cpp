/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSMedium.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSMedium::DSMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameDirectShowLibrary();
}

bool DSMedium::isStarted() const
{
	return graphIsRunning_;
}

Timestamp DSMedium::startTimestamp() const
{
	return startTimestamp_;
}

Timestamp DSMedium::pauseTimestamp() const
{
	return pauseTimestamp_;
}

Timestamp DSMedium::stopTimestamp() const
{
	return stopTimestamp_;
}

bool DSMedium::start()
{
	if (buildGraph() == false)
		return false;

	bool result = startGraph();

	if (!result)
	{
		releaseGraph();

		result = buildGraph() && startGraph();
	}

	if (result)
	{
		startTimestamp_.toNow();
		pauseTimestamp_ = Timestamp();
		stopTimestamp_ = Timestamp();
	}

	return result;
}

bool DSMedium::pause()
{
	const bool result = pauseGraph();

	if (result)
	{
		startTimestamp_ = Timestamp();
		pauseTimestamp_.toNow();
		stopTimestamp_ = Timestamp();
	}

	return result;
}

bool DSMedium::stop()
{
	if (stopGraph())
	{
		startTimestamp_ = Timestamp();
		pauseTimestamp_ = Timestamp();
		stopTimestamp_.toNow();
		return true;
	}

	return false;
}

bool DSMedium::setRespectPlaybackTime(const bool respectPlaybackTime)
{
	if (!respectPlaybackTime)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

}

}

}
