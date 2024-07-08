/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSMovieRecorder.h"

#include "ocean/base/ScopedFunction.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSMovieRecorder::DSMovieRecorder() :
	DSFileRecorder(),
	DSFrameRecorder(),
	MovieRecorder()
{
	// nothing to do here
}

DSMovieRecorder::~DSMovieRecorder()
{
	releaseGraph();
}

bool DSMovieRecorder::buildGraph()
{
	if (filterGraph_.isValid())
	{
		return true;
	}

	if (!createGraphBuilder())
	{
		return false;
	}

	ScopedFunctionVoid scopedReleaseGraphFunction(std::bind(&DSMovieRecorder::releaseGraph, this)); // scoped function which will be invoked in case we don't reach the both of this function

	ScopedIPin sourceFilterOutputPin;
	if (!insertSourceFilter(sourceFilterOutputPin))
	{
		return false;
	}

	ScopedIPin encoderFilterOutputPin;
	if (!insertFrameEncoderFilter(*sourceFilterOutputPin, encoderFilterOutputPin))
	{
		return false;
	}

	if (!insertFileWriterFilter(*encoderFilterOutputPin))
	{
		return false;
	}

	scopedReleaseGraphFunction.revoke(); // we don't call the releaseGraph() function

	return true;
}

void DSMovieRecorder::releaseGraph()
{
	stopGraph();

	releaseFileWriterFilter();
	releaseFrameEncoderFilter();
	releaseFrameSourceFilter();
	releaseGraphBuilder();
}

}

}

}
