// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/media/LiveVideo.h"

namespace Ocean
{

namespace Media
{

LiveVideo::LiveVideo(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url)
{
	type_ = Type(type_ | LIVE_VIDEO);
}

double LiveVideo::exposureDuration(double* minDuration, double* maxDuration) const
{
	if (minDuration)
	{
		*minDuration = -1.0;
	}

	if (maxDuration)
	{
		*maxDuration = -1.0;
	}

	return -1.0;
}

float LiveVideo::iso(float* minISO, float* maxISO) const
{
	if (minISO)
	{
		*minISO = -1.0f;
	}

	if (maxISO)
	{
		*maxISO = -1.0f;
	}

	return -1.0f;
}

float LiveVideo::focus() const
{
	return -1.0f;
}

bool LiveVideo::setExposureDuration(const double /*duration*/)
{
	return false;
}

bool LiveVideo::setISO(const float /*iso*/)
{
	return false;
}

bool LiveVideo::setFocus(const float /*position*/)
{
	return false;
}

}

}
