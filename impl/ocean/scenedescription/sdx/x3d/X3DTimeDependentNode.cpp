/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DTimeDependentNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DTimeDependentNode::X3DTimeDependentNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	loop_(false),
	pauseTime_(Timestamp(0.0)),
	resumeTime_(Timestamp(0.0)),
	startTime_(Timestamp(0.0)),
	stopTime_(Timestamp(0.0)),
	isActive_(false),
	isPaused_(false),
	pausedTime_(0.0)
{
	// nothing to do here
}

void X3DTimeDependentNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "loop", loop_, ACCESS_GET_SET);
	registerField(specification, "pauseTime", pauseTime_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "resumeTime", resumeTime_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "startTime", startTime_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "stopTime", stopTime_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "elapsedTime", elapsedTime_, ACCESS_GET_SET);
	registerField(specification, "isActive", isActive_, ACCESS_GET);
	registerField(specification, "isPaused", isPaused_, ACCESS_GET);

	X3DChildNode::registerFields(specification);
}

void X3DTimeDependentNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DChildNode::onInitialize(scene, timestamp);
}

bool X3DTimeDependentNode::onFieldChanging(const std::string& fieldName, const Field& field)
{
	const Timestamp& now = field.timestamp();

	if (fieldName == "startTime")
	{
		// Any set_startTime events to an active time-dependent node are ignored.
		if (isActive_.value() == false)
		{
			const SingleTime& time = Field::cast<SingleTime>(field);
			startTime_.setValue(time.value(), time.timestamp());
		}

		return true;
	}
	else if (fieldName == "stopTime")
	{
		const SingleTime& time = Field::cast<SingleTime>(field);

		if (isActive_.value())
		{
			// Any set_stopTime event where stopTime <= startTime sent to an active time-dependent node is also ignored.
			if (time.value() <= startTime_.value())
			{
				return true;
			}

			// A set_stopTime event where startTime < stopTime <= now sent to an active time-dependent node results in events being generated as if stopTime has just been reached.
			// That is, final events, including an isActive FALSE, are generated and the node becomes inactive.
			// The stopTime_changed event will have the set_stopTime value.
			// Other final events are node-dependent (see 8.4.1 TimeSensor).
			if (startTime_.value() < time.value() && time.value() <= now)
			{
				stopNode(time.value(), now);
			}
		}

		return true;
	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

void X3DTimeDependentNode::onFieldChanged(const std::string& fieldName)
{

	X3DChildNode::onFieldChanged(fieldName);
}

void X3DTimeDependentNode::onUpdate(const Rendering::ViewRef& /*view*/, const Timestamp timestamp)
{
	if (!initialized_)
	{
		return;
	}

	const ScopedLock scopedLock(lock_);

	if (isActive_.value())
	{
		if (isPaused_.value())
		{
			// An active but paused time-dependent node shall resume at the first simulation tick when now >= resumeTime > pauseTime.
			// The time-dependent node then resumes generating its output events from the paused state at the simulation tick.
			// A resumeTime_changed event is also generated reporting the simulation time when the node was resumed.
			if (timestamp >= resumeTime_.value() && resumeTime_.value() > pauseTime_.value())
			{
				resumeNode(timestamp, timestamp);
				return;
			}
		}
		else // isPaused.value() == false
		{
			// An active time-dependent node will become inactive when stopTime is reached if stopTime > startTime.
			// The value of stopTime is ignored if stopTime <= startTime.
			if (stopTime_.value() > startTime_.value() && timestamp >= stopTime_.value())
			{
				stopNode(stopTime_.value(), timestamp);
				return;
			}

			// An active time-dependent node may be paused when its SFTime fields are such that now >= pauseTime > resumeTime.
			// When a time-dependent node is paused, the time-dependent node shall send out a TRUE  event on isPaused and a pauseTime_changed event reporting the simulation time when the node was paused.
			if (timestamp >= pauseTime_.value() && pauseTime_.value() > resumeTime_.value())
			{
				pauseNode(timestamp, timestamp);
				return;
			}
		}
	}
	else // isActive.value() == false
	{
		// A time-dependent node is inactive until its startTime is reached.
		// When time now becomes greater than or equal to startTime, an isActive TRUE event is generated and the time-dependent node becomes active
		if (timestamp >= startTime_.value())
		{
			if (timestamp < stopTime_.value() || stopTime_.value() <= startTime_.value())
			{
				// stop time is either not yet reached, or start time is newer

				startNode(timestamp, timestamp);
				return;
			}
		}
	}

	onUpdated(timestamp);

	if (isActive_.value())
	{
		ocean_assert(startTime_.value() <= timestamp);

		elapsedTime_.setValue(Timestamp(timestamp - startTime_.value() - pausedTime_), timestamp);
	}
}

void X3DTimeDependentNode::startNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp)
{
	ocean_assert(isActive_.value() == false);

	isActive_.setValue(true, eventTimestamp);
	startTime_.setValue(valueTimestamp, eventTimestamp);
	elapsedTime_.setValue(Timestamp(0.0), eventTimestamp);

	onStarted(eventTimestamp);

	forwardThatFieldHasBeenChanged("isActive", isActive_);
	forwardThatFieldHasBeenChanged("startTime", startTime_);
	forwardThatFieldHasBeenChanged("elapsedTime", elapsedTime_);
}

void X3DTimeDependentNode::pauseNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp)
{
	// While an active time-dependent node is paused, it generates TRUE isPaused and pauseTime_changed events and ceases to generate all other output events,
	// while maintaining (or 'freezing') its state (holding the last output values and the clock's internal time when the pausing conditions are met).

	ocean_assert(isActive_.value() == true);
	ocean_assert(isPaused_.value() == false);

	isPaused_.setValue(true, eventTimestamp);
	pauseTime_.setValue(valueTimestamp, eventTimestamp);

	onPaused(eventTimestamp);

	forwardThatFieldHasBeenChanged("isPaused", isPaused_);
	forwardThatFieldHasBeenChanged("pauseTime", pauseTime_);
}

void X3DTimeDependentNode::resumeNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp)
{
	ocean_assert(isActive_.value());
	ocean_assert(isPaused_.value());

	isPaused_.setValue(false, eventTimestamp);
	resumeTime_.setValue(valueTimestamp, eventTimestamp);

	onResumed(eventTimestamp);

	forwardThatFieldHasBeenChanged("isPaused", isPaused_);
	forwardThatFieldHasBeenChanged("resumeTime", resumeTime_);
}

void X3DTimeDependentNode::stopNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp)
{
	ocean_assert(isActive_.value());

	isActive_.setValue(false, eventTimestamp);
	stopTime_.setValue(valueTimestamp, eventTimestamp);

	onStopped(eventTimestamp);

	forwardThatFieldHasBeenChanged("isActive", isActive_);
	forwardThatFieldHasBeenChanged("stopTime", stopTime_);
}

}

}

}

}
