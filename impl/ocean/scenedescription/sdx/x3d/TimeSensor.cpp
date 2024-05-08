/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/TimeSensor.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TimeSensor::TimeSensor(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DTimeDependentNode(environment),
	X3DSensorNode(environment),
	cycleInterval_(Timestamp(1.0)),
	lastFraction_(0)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

TimeSensor::NodeSpecification TimeSensor::specifyNode()
{
	NodeSpecification specification("TimeSensor");

	registerField(specification, "cycleInterval", cycleInterval_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "cycleTime", cycleTime_, ACCESS_GET);
	registerField(specification, "fraction_changed", fractionChanged_, ACCESS_GET);
	registerField(specification, "time", time_, ACCESS_GET);

	X3DTimeDependentNode::registerFields(specification);
	X3DSensorNode::registerFields(specification);

	return specification;
}

void TimeSensor::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTimeDependentNode::onInitialize(scene, timestamp);
	X3DSensorNode::onInitialize(scene, timestamp);

	if (double(cycleInterval_.value()) <= 0.0)
	{
		cycleInterval_.setValue(Timestamp(1.0));
	}

	if (enabled_.value())
	{
		if (loop_.value() && stopTime_.value() <= startTime_.value())
		{
			startNode(timestamp, timestamp);
		}
	}
}

bool TimeSensor::onFieldChanging(const std::string& fieldName, const Field& field)
{
	// Input events on the fields of the TimeSensor node (e.g., set_startTime) are processed and their corresponding outputOnly fields
	// (e.g., startTime_changed) are sent regardless of the state of the enabled field.
	if (fieldName == "startTime")
	{
		const SingleTime& timeValue = Field::cast<SingleTime>(field);
		startTime_.setValue(timeValue.value(), timeValue.timestamp());
		forwardThatFieldHasBeenChanged("startTime", startTime_);
		return true;
	}
	else if (fieldName == "pauseTime")
	{
		const SingleTime& timeValue = Field::cast<SingleTime>(field);
		pauseTime_.setValue(timeValue.value(), timeValue.timestamp());
		forwardThatFieldHasBeenChanged("pauseTime", pauseTime_);
		return true;
	}
	else if (fieldName == "resumeTime")
	{
		const SingleTime& timeValue = Field::cast<SingleTime>(field);
		resumeTime_.setValue(timeValue.value(), timeValue.timestamp());
		forwardThatFieldHasBeenChanged("resumeTime", resumeTime_);
		return true;
	}
	else if (fieldName == "stopTime")
	{
		const SingleTime& timeValue = Field::cast<SingleTime>(field);
		stopTime_.setValue(timeValue.value(), timeValue.timestamp());
		forwardThatFieldHasBeenChanged("stopTime", stopTime_);
		return true;
	}

	if (X3DTimeDependentNode::onFieldChanging(fieldName, field))
	{
		return true;
	}

	return X3DSensorNode::onFieldChanging(fieldName, field);
}

void TimeSensor::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "cycleInterval" && double(cycleInterval_.value()) <= 0.0)
	{
		cycleInterval_.setValue(Timestamp(1.0));
	}

	X3DChildNode::onFieldChanged(fieldName);
}

void TimeSensor::onUpdated(const Timestamp timestamp)
{
	if (isActive_.value() == false || isPaused_.value() == true)
	{
		return;
	}

	ocean_assert(double(cycleInterval_.value()) > 0.0);
	ocean_assert(pausedTime_ >= 0.0);

	const double quotient = (double(timestamp - startTime_.value()) - pausedTime_) / double(cycleInterval_.value()); // we subtract the time this sensor was paused to get a smooth animation when resuming

	ocean_assert(quotient >= 0.0); // startTime should never be later than timestamp

	Scalar fraction = Scalar(quotient) - Scalar(int(quotient));
	ocean_assert(Scalar(fraction) >= 0 && fraction < Scalar(1));

	// fraction needs to be in range (0, 1]
	if (fraction == Scalar(0) && timestamp > startTime_.value())
	{
		fraction = Scalar(1);
	}

	ocean_assert((Scalar(fraction) > 0 && fraction <= Scalar(1)) || (Scalar(fraction) == 0 && timestamp == startTime_.value()));

	// ensuring that we send a fraction == 1 after one interval has finished

	if (quotient > 1.0 && lastFraction_ > fraction)
	{
		fractionChanged_.setValue(1, timestamp);
		forwardThatFieldHasBeenChanged("fraction_changed", fractionChanged_);

		if (loop_.value() == false)
		{
			stopNode(timestamp, timestamp);
		}
		else
		{
			fractionChanged_.setValue(fraction, timestamp);
			forwardThatFieldHasBeenChanged("fraction_changed", fractionChanged_);
		}
	}
	else
	{
		fractionChanged_.setValue(fraction, timestamp);
		forwardThatFieldHasBeenChanged("fraction_changed", fractionChanged_);
	}

	time_.setValue(timestamp, timestamp);
	forwardThatFieldHasBeenChanged("time", time_);

	lastFraction_ = fraction;
}

void TimeSensor::onStarted(const Timestamp eventTimestamp)
{
	lastFraction_ = 0;
	pausedTime_ = 0.0;

	fractionChanged_.setValue(0, eventTimestamp);
	time_.setValue(eventTimestamp, eventTimestamp);

	forwardThatFieldHasBeenChanged("fraction_changed", fractionChanged_);
	forwardThatFieldHasBeenChanged("time", time_);
}

void TimeSensor::onPaused(const Timestamp /*eventTimestamp*/)
{
	// nothing to do here
}

void TimeSensor::onResumed(const Timestamp eventTimestamp)
{
	ocean_assert(pauseTime_.value() <= eventTimestamp);

	// the sensor may have been paused before, so we accumulate the paused time
	pausedTime_ += NumericD::abs(double(eventTimestamp - pauseTime_.value()));
}

void TimeSensor::onStopped(const Timestamp eventTimestamp)
{
	lastFraction_ = 0;
	pausedTime_ = 0.0;

	fractionChanged_.setValue(1, eventTimestamp);
	time_.setValue(eventTimestamp, eventTimestamp);

	forwardThatFieldHasBeenChanged("fraction_changed", fractionChanged_);
	forwardThatFieldHasBeenChanged("time", time_);
}

size_t TimeSensor::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
