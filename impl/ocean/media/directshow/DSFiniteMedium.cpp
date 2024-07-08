/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSFiniteMedium.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSFiniteMedium::DSFiniteMedium(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	DSMedium(url)
{
	Scheduler::get().registerFunction(Scheduler::Callback(*this, &DSFiniteMedium::onScheduler));
}

DSFiniteMedium::~DSFiniteMedium()
{
	Scheduler::get().unregisterFunction(Scheduler::Callback(*this, &DSFiniteMedium::onScheduler));

	ocean_assert(!seekingInterface_.isValid());
}

double DSFiniteMedium::duration() const
{
	const float currentSpeed = speed();

	if (currentSpeed == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double DSFiniteMedium::normalDuration() const
{
	if (!seekingInterface_.isValid())
	{
		return 0.0;
	}

	long long units;
	if (S_OK == seekingInterface_->GetDuration(&units))
	{
		return double(units) * 0.0000001;
	}

	return 0.0;
}

double DSFiniteMedium::position() const
{
	if (!seekingInterface_.isValid())
	{
		return 0.0;
	}

	float currentSpeed = speed();
	if (currentSpeed == 0.0)
	{
		return 0.0;
	}

	long long pos = 0l;
	if (S_OK != seekingInterface_->GetCurrentPosition(&pos))
	{
		return 0.0;
	}

	return double(pos) * 0.0000001 / double(currentSpeed);
}

bool DSFiniteMedium::setPosition(const double position)
{
	const ScopedLock scopedLock(lock_);

	if (!seekingInterface_.isValid())
	{
		return false;
	}

	double currentSpeed = double(speed());
	long long pos = NumericD::round64(position * currentSpeed * 10000000.0);

	if (pauseTimestamp_.isValid())
	{
		long long units = NumericT<long long>::maxValue();
		seekingInterface_->GetDuration(&units);

		pos = max(0ll, pos - 10000000ll);
		long long stopPosition = min(pos + 20000000ll, units);

		stopTimestamp_.toInvalid();

		return S_OK == seekingInterface_->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, &stopPosition, AM_SEEKING_AbsolutePositioning);
	}
	else
	{
		return S_OK == seekingInterface_->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
	}
}

float DSFiniteMedium::speed() const
{
	const ScopedLock scopedLock(lock_);

	return speed_;
}

bool DSFiniteMedium::setSpeed(const float speed)
{
	if (speed < 0.0f)
	{
		ocean_assert(false && "Invalid speed");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (speed_ == speed)
	{
		return true;
	}

	if (!seekingInterface_.isValid())
	{
		speed_ = speed;
		return true;
	}

	if (speed == 0.0f || (speed_ == 0.0f && speed > 0.0f))
	{
		// the caller wants to change respect-playback-time behavior, either active it or deactivate it

		if (startTimestamp_.isValid())
		{
			return false;
		}

		const bool respectPlaybackTime = speed > 0.0f;

		if (!setRespectPlaybackTime(respectPlaybackTime))
		{
			return false;
		}

		if (speed == 0.0f)
		{
			speed_ = 0.0f;

			return true;
		}

		// we need to respect the playback time, and we have to set the correct speed
	}

	if (S_OK == seekingInterface_->SetRate(double(speed)))
	{
		speed_ = speed;

		return true;
	}

	return false;
}

bool DSFiniteMedium::taskFinished() const
{
	return stopTimestamp_.isValid() || (startTimestamp_.isInvalid() && pauseTimestamp_.isInvalid() && stopTimestamp_.isInvalid());
}

bool DSFiniteMedium::startGraph()
{
	hasStopped_ = false;

	if (seekingInterface_.isValid())
	{
		seekingInterface_->SetRate(double(speed_));
	}

	return DSGraphObject::startGraph();
}

bool DSFiniteMedium::createFiniteInterface()
{
	ocean_assert(filterGraph_.isValid());

	if (seekingInterface_.isValid())
	{
		return true;
	}

	bool noError = true;

	if (noError && S_OK != filterGraph_->QueryInterface(IID_IMediaSeeking, (void**)(&seekingInterface_.resetObject())))
	{
		Log::error() << "Could not create the media seeking interface.";
		noError = false;
	}

	if (noError == false)
	{
		releaseFiniteInterface();
	}

	return noError;
}

void DSFiniteMedium::releaseFiniteInterface()
{
	seekingInterface_.release();
}

void DSFiniteMedium::hasStopped()
{
	hasStopped_ = true;
}

void DSFiniteMedium::onScheduler()
{
	if (graphIsRunning_ && (type_ & FRAME_MEDIUM) == 0)
	{
		long code = 0;
		LONG_PTR param1 = 0;
		LONG_PTR param2 = 0;

		if (mediaEventInterface_.isValid() && S_OK == mediaEventInterface_->GetEvent(&code, &param1, &param2, 0))
		{
			mediaEventInterface_->FreeEventParams(code, param1, param2);

			if (code == EC_COMPLETE)
			{
				hasStopped_ = true;
			}
		}
	}

	if (hasStopped_)
	{
		hasStopped_ = false;

		// if the medium is paused nothing should happen, expect that the stop time will be set to identify when the pause state has stopped
		if (pauseTimestamp_.isValid())
		{
			stopTimestamp_.toNow();
			return;
		}

		ocean_assert(startTimestamp_.isValid());

		if (!loop_)
		{
			stopGraph();
		}

		if (!setPosition(0))
		{
			Log::error() << "Failed to set the medium back to start position.";
		}

		if (loop_)
		{
			if (start())
			{
				return;
			}

			Log::error() << "Failed to restart the medium.";
		}

		startTimestamp_.toInvalid();
		pauseTimestamp_.toInvalid();
		stopTimestamp_.toNow();
	}
}

}

}

}
