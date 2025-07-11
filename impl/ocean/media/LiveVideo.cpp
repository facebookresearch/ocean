/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/LiveVideo.h"

namespace Ocean
{

namespace Media
{

LiveVideo::StreamProperty::StreamProperty(const StreamType streamType, const unsigned int width, unsigned int height, const FrameType::PixelFormat framePixelFormat, const CodecType codecType) :
	streamType_(streamType),
	width_(width),
	height_(height),
	framePixelFormat_(framePixelFormat),
	codecType_(codecType)
{
	// nothing to do here
}

bool LiveVideo::StreamProperty::operator==(const StreamProperty& right) const
{
	return streamType_ == right.streamType_ && width_ == right.width_ && height_ == right.height_ && framePixelFormat_ == right.framePixelFormat_ && codecType_ == right.codecType_;
}

LiveVideo::StreamConfiguration::StreamConfiguration(const StreamProperty& streamProperty, std::vector<double>&& frameRates) :
	StreamProperty(streamProperty),
	frameRates_(std::move(frameRates))
{
	// nothing to do here
}

LiveVideo::StreamConfiguration::StreamConfiguration(const StreamType streamType, const unsigned int width, unsigned int height, std::vector<double>&& frameRates, const FrameType::PixelFormat framePixelFormat, const CodecType codecType) :
	StreamProperty(streamType, width, height, framePixelFormat, codecType),
	frameRates_(std::move(frameRates))
{
	// nothing to do here
}

std::string LiveVideo::StreamConfiguration::toString() const
{
	if (streamType_ == ST_INVALID)
	{
		return "Invalid";
	}

	std::string result = translateStreamType(streamType_);

	result += "\nResolution: " + String::toAString(width_) + "x" + String::toAString(height_);
	result += "\nFrame rates: ";

	for (const double frameRate : frameRates_)
	{
		result += String::toAString(frameRate, 1u) + " ";
	}

	if (frameRates_.empty())
	{
		result += "Unknown";
	}
	else
	{
		result += "fps";
	}

	switch (streamType_)
	{
		case ST_INVALID:
			ocean_assert(false && "This should never happen!");
			break;

		case ST_FRAME:
			result += "\nPixel format: " + FrameType::translatePixelFormat(framePixelFormat_);
			break;

		case ST_MJPEG:
			break;

		case ST_CODEC:
			result += "\nCodec: " + translateCodecType(codecType_);
			break;
	}

	return result;
}

LiveVideo::LiveVideo(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url)
{
	type_ = Type(type_ | LIVE_VIDEO);
}

LiveVideo::StreamTypes LiveVideo::supportedStreamTypes() const
{
	return StreamTypes();
}

LiveVideo::StreamConfigurations LiveVideo::supportedStreamConfigurations(const StreamType /*streamType*/) const
{
	return StreamConfigurations();
}

double LiveVideo::exposureDuration(double* minDuration, double* maxDuration, ControlMode* exposureMode) const
{
	if (minDuration != nullptr)
	{
		*minDuration = -1.0;
	}

	if (maxDuration != nullptr)
	{
		*maxDuration = -1.0;
	}

	if (exposureMode != nullptr)
	{
		*exposureMode = CM_INVALID;
	}

	return -1.0;
}

float LiveVideo::iso(float* minISO, float* maxISO, ControlMode* isoMode) const
{
	if (minISO != nullptr)
	{
		*minISO = -1.0f;
	}

	if (maxISO != nullptr)
	{
		*maxISO = -1.0f;
	}

	if (isoMode != nullptr)
	{
		*isoMode = CM_INVALID;
	}

	return -1.0f;
}

float LiveVideo::focus(ControlMode* focusMode) const
{
	if (focusMode != nullptr)
	{
		*focusMode = CM_INVALID;
	}

	return -1.0f;
}

bool LiveVideo::setPreferredStreamType(const StreamType /*streamType*/)
{
	return false;
}

bool LiveVideo::setPreferredStreamConfiguration(const StreamConfiguration& /*streamConfiguration*/)
{
	return false;
}

bool LiveVideo::setExposureDuration(const double /*duration*/, const bool /*allowShorterExposure*/)
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

std::string LiveVideo::translateControlMode(const ControlMode controlMode)
{
	switch (controlMode)
	{
		case CM_INVALID:
			return std::string("Invalid");

		case CM_FIXED:
			return std::string("Fixed");

		case CM_DYNAMIC:
			return std::string("Dynamic");
	}

	ocean_assert(false && "Invalid control mode!");
	return std::string("Invalid");
}

std::string LiveVideo::translateStreamType(const StreamType streamType)
{
	switch (streamType)
	{
		case ST_INVALID:
			return std::string("Invalid");

		case ST_FRAME:
			return std::string("Frame");

		case ST_MJPEG:
			return std::string("MJPEG");

		case ST_CODEC:
			return std::string("Codec");
	}

	ocean_assert(false && "Invalid stream type!");
	return std::string("Invalid");
}

std::string LiveVideo::translateCodecType(const CodecType codecType)
{
	switch (codecType)
	{
		case CT_INVALID:
			return std::string("Invalid");

		case CT_H264:
			return std::string("H264");

		case CT_H265:
			return std::string("H265");
	}

	ocean_assert(false && "Invalid stream type!");
	return std::string("Invalid");
}

}

}
