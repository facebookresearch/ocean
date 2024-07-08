/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/TrackerEvent.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

TrackerEvent::TrackerEvent(const unsigned int type, const unsigned int subtype, const unsigned int trackerId) :
	Event(type, subtype),
	trackerId_(trackerId)
{
	// nothing to do here
}

std::string TrackerEvent::typeName() const
{
	switch (type())
	{
		case ET_COMPONENT:
			return std::string("Component");

		case ET_PROGRESS:
			return std::string("Progress");

		case ET_STATE:
			return std::string("State");
	}

	ocean_assert(false && "Invalid type!");
	return std::string("Invalid type");
}

std::string ComponentEvent::subtypeName() const
{
	switch (subtype())
	{
		case CEST_TRACKER_PROCESS:
			return std::string("Tracker Process");

		case CEST_ANALYSIS:
			return std::string("Analysis");

		case CEST_TRACKING:
			return std::string("Tracking");
	}

	ocean_assert(false && "Invalid sub-type!");
	return std::string("Invalid sub-type");
}

std::string ComponentEvent::stateName() const
{
	switch (eventState)
	{
		case CS_STARTED:
			return std::string("Started");

		case CS_FINISHED:
			return std::string("Finished");

		case CS_FAILED:
			return std::string("Failed");

		case CS_BROKE:
			return std::string("Broke");
	}

	ocean_assert(false && "Invalid state!");
	return std::string("Invalid state");
}

std::string ProgressEvent::subtypeName() const
{
	switch (subtype())
	{
		case PEST_TRACKER_PROCESS:
			return std::string("Tracker Process");

		case PEST_ANALYSIS:
			return std::string("Analysis");

		case PEST_TRACKING:
			return std::string("Tracking");
	}

	ocean_assert(false && "Invalid sub-type!");
	return std::string("Invalid sub-type");
}

std::string StateEvent::subtypeName() const
{
	switch (subtype())
	{
		case SEST_CAMERA_CALIBRATION:
			return std::string("Camera Calibration");

		case SEST_TRACKER_TRANSFORMATION:
			return std::string("Tracker Transformation");

		case SEST_TRACKER_POSE:
			return std::string("Tracker Pose");

		case SEST_TRACKER_PLANE:
			return std::string("Tracker Plane");

		case SEST_TRACKER_POSES:
			return std::string("Tracker Poses");
	}

	ocean_assert(false && "Invalid sub-type!");
	return std::string("Invalid sub-type");
}

}

}

}
