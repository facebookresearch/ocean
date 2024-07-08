/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFFiniteMedium.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFFiniteMedium::MFFiniteMedium(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	MFMedium(url)
{
	Scheduler::get().registerFunction(Scheduler::Callback(*this, &MFFiniteMedium::onScheduler));
}

MFFiniteMedium::~MFFiniteMedium()
{
	Scheduler::get().unregisterFunction(Scheduler::Callback(*this, &MFFiniteMedium::onScheduler));
}

double MFFiniteMedium::duration() const
{
	const ScopedLock scopedLock(lock_);

	const float currentSpeed = speed();

	if (currentSpeed == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double MFFiniteMedium::normalDuration() const
{
	const ScopedLock scopedLock(lock_);

	if (!mediaSource_.isValid())
	{
		return 0.0;
	}

	IMFPresentationDescriptor* presentationDescriptor;
	if (S_OK != mediaSource_->CreatePresentationDescriptor(&presentationDescriptor))
	{
		return 0.0;
	}

	double result = 0.0;

	unsigned long long duration = 0ull;
	if (S_OK == presentationDescriptor->GetUINT64(MF_PD_DURATION, &duration))
	{
		result = duration / 1.0e+7;
	}

	release(presentationDescriptor);
	return result;
}

double MFFiniteMedium::position() const
{
	const ScopedLock scopedLock(lock_);

	if (!mediaSource_.isValid())
	{
		return -1.0;
	}

	ocean_assert(mediaSession_.isValid());

	ScopedMediaFoundationObject<IMFClock> clock;
	if (S_OK != mediaSession_->GetClock(&clock.resetObject()))
	{
		return -1.0;
	}

	LONGLONG clockTime = -1ll;
	MFTIME systemTime = -1ll;

	if (S_OK != clock->GetCorrelatedTime(0, &clockTime, &systemTime))
	{
		return -1.0;
	}

	return double(clockTime) / 1.0e+7;
}

bool MFFiniteMedium::setPosition(const double position)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(mediaSession_.isValid());
	if (!mediaSession_.isValid())
	{
		return false;
	}

	if (position > duration())
	{
		return false;
	}

	startPosition_ = (long long)(position * 1.0e+7);

	if (startTimestamp_.isValid())
	{
		return startMediaSession();
	}

	return true;
}

float MFFiniteMedium::speed() const
{
	const ScopedLock scopedLock(lock_);

	return speed_;
}

bool MFFiniteMedium::setSpeed(const float speed)
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

	ocean_assert(mediaSession_.isValid());

	if (!mediaSession_.isValid())
	{
		return false;
	}

	if (speed == 0.0f || (speed_ == 0.0f && speed > 0.0f))
	{
		// the caller wants to change respect-playback-time behavior, either active it or deactivate it

		if (startTimestamp_.isValid())
		{
			return false;
		}

		releasePipeline();

		const bool respectPlaybackTime = speed > 0.0f;

		if (!createPipeline(respectPlaybackTime))
		{
			return false;
		}

		respectPlaybackTime_ = respectPlaybackTime;

		if (speed == 0.0f)
		{
			speed_ = 0.0f;

			return true;
		}

		// we need to respect the playback time, and we have to set the correct speed
	}

	ocean_assert(speed_ > 0.0f && speed > 0.0f);

	bool success = false;

	ScopedMediaFoundationObject<IMFRateControl> rateControl;
	if (S_OK == MFGetService(*mediaSession_, MF_RATE_CONTROL_SERVICE, IID_IMFRateControl, (void**)(&rateControl.resetObject())))
	{
		if (S_OK == rateControl->SetRate(false, speed))
		{
			success = true;
			speed_ = speed;
		}
	}

	return success;
}

bool MFFiniteMedium::taskFinished() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isInvalid();
}

void MFFiniteMedium::mediumHasStopped()
{
	const ScopedLock scopedLock(lock_);

	hasStopped_ = true;
}

bool MFFiniteMedium::startMediaSession()
{
	ocean_assert(mediaSession_.isValid());
	if (!mediaSession_.isValid())
	{
		return false;
	}

	PROPVARIANT startPosition;
	PropVariantInit(&startPosition);
	startPosition.vt = VT_I8;

	if (startPosition_ == LONGLONG(-1))
	{
		startPosition.hVal.QuadPart = LONGLONG(0);

		if (pauseTimestamp_.isValid())
		{
			LONGLONG recentTimestamp = LONGLONG(-1);
			LONGLONG nextTimestamp = LONGLONG(-1);

			if (recentMediaSampleTimestamp(recentTimestamp, &nextTimestamp))
			{
				startPosition.hVal.QuadPart = nextTimestamp;
			}
		}
	}
	else
	{
		startPosition.hVal.QuadPart = startPosition_;
		startPosition_ = LONGLONG(-1);
	}

	if (S_OK != mediaSession_->Start(&GUID_NULL, &startPosition))
	{
		return false;
	}

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	return true;
}

void MFFiniteMedium::onSessionEnded()
{
	const ScopedLock scopedLock(lock_);

	eventSessionEnded_ = true;
}

void MFFiniteMedium::onScheduler()
{
	const ScopedLock scopedLock(lock_);

	if (eventSessionEnded_ || hasStopped_)
	{
		// reset the event
		eventSessionEnded_ = false;
		hasStopped_ = false;

		// if the medium is paused nothing should happen, expect that the stop time will be set to identify when the pause state has stopped
		if (pauseTimestamp_.isValid())
		{
			stopTimestamp_.toNow();
			return;
		}

		ocean_assert(startTimestamp_.isValid());

		if (loop_)
		{
			if (start())
			{
				return;
			}

			Log::error() << "Failed to restart the medium.";
			stop();
		}

		startTimestamp_.toInvalid();
		pauseTimestamp_.toInvalid();
		stopTimestamp_.toNow();
	}
}

}

}

}
