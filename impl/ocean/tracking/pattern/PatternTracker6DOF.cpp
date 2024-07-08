/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/pattern/PatternTracker6DOF.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Tracking
{

namespace Pattern
{

PatternTracker6DOF::PatternTracker6DOF(const Options& options) :
	PatternTrackerCore6DOF(options)
{
	// nothing to do here
}

PatternTracker6DOF::~PatternTracker6DOF()
{
	// nothing to do here
}

unsigned int PatternTracker6DOF::addPattern(const Frame& frame, const Vector2& dimension, Worker* worker)
{
	if (!frame.isValid() || dimension.x() <= 0)
	{
		return (unsigned int)(-1);
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, false, worker))
	{
		return (unsigned int)(-1);
	}

	return PatternTrackerCore6DOF::addPattern(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), dimension, worker);
}

unsigned int PatternTracker6DOF::addPattern(const std::string& filename, const Vector2& dimension, Worker* worker)
{
	return PatternTrackerCore6DOF::addPattern(filename, dimension, worker);
}

bool PatternTracker6DOF::removePattern(const unsigned int patternId)
{
	return PatternTrackerCore6DOF::removePattern(patternId);
}

bool PatternTracker6DOF::removePatterns()
{
	return PatternTrackerCore6DOF::removePatterns();
}

bool PatternTracker6DOF::determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(frame.isValid() && pinholeCamera.isValid());
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	ocean_assert(yFrame_.width() == pinholeCamera.width() && yFrame_.height() == pinholeCamera.height());

	return PatternTrackerCore6DOF::determinePoses(yFrame_.constdata<uint8_t>(), pinholeCamera, yFrame_.paddingElements(), frameIsUndistorted, frame.timestamp(), transformations, previousCamera_R_camera, worker);
}

}

}

}
