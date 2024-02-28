// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

}

}

}
