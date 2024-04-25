// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRSFiniteMedium.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

VRSFiniteMedium::VRSFiniteMedium(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	VRSMedium(url)
{
	if (isValid_)
	{
		ocean_assert(recordFileReader_);
		ocean_assert(streamId_.getInstanceId() != uint16_t(0));

		firstFrameTimestamp_ = determineFirstFrameTimestamp(*recordFileReader_, streamId_);
		lastFrameTimestamp_ = determineLastFrameTimestamp(*recordFileReader_, streamId_);

		numberFrames_ = determineNumberFrames(*recordFileReader_, streamId_);

		if (numberFrames_ == 1)
		{
			normalDuration_ = 1.0 / 30.0;
		}
		else if (numberFrames_ >= 2)
		{
			// last - first / numberFrames_ - 1  ==  duration / numberFrames
			normalDuration_ = double(lastFrameTimestamp_ - firstFrameTimestamp_) * double(numberFrames_) / double(numberFrames_ - 1);
		}
		else if (numberFrames_ == 0)
		{
			isValid_ = false;
		}
	}
}

VRSFiniteMedium::~VRSFiniteMedium()
{
	// nothing to do here
}

double VRSFiniteMedium::duration() const
{
	const ScopedLock scopedLock(lock_);

	const float currentSpeed = speed();

	if (currentSpeed == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double VRSFiniteMedium::normalDuration() const
{
	const ScopedLock scopedLock(lock_);

	return normalDuration_;
}

double VRSFiniteMedium::position() const
{
	const ScopedLock scopedLock(lock_);

	return position_;
}

bool VRSFiniteMedium::setPosition(const double position)
{
	const ScopedLock scopedLock(lock_);

	if (position < 0.0 || position > normalDuration_)
	{
		return false;
	}

	newDesiredPosition_ = position;

	return true;
}

float VRSFiniteMedium::speed() const
{
	const ScopedLock scopedLock(lock_);

	return speed_;
}

bool VRSFiniteMedium::setSpeed(const float speed)
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

	if (speed == 0.0f || (speed_ == 0.0f && speed > 0.0f))
	{
		const bool respectPlaybackTime = speed > 0.0f;

		if (!setRespectPlaybackTime(respectPlaybackTime))
		{
			return false;
		}
	}

	speed_ = speed;

	return true;
}

}

}

}
