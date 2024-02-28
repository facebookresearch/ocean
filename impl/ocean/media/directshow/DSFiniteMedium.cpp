// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

	ocean_assert(mediaSeekingInterface_.object() == nullptr);
}

double DSFiniteMedium::duration() const
{
	float currentSpeed = speed();

	if (double(currentSpeed) == 0.0)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double DSFiniteMedium::normalDuration() const
{
	if (!mediaSeekingInterface_.object())
	{
		return 0.0;
	}

	long long units;
	if (S_OK == mediaSeekingInterface_.object()->GetDuration(&units))
	{
		return double(units) * 0.0000001;
	}

	return 0.0;
}

double DSFiniteMedium::position() const
{
	if (mediaSeekingInterface_.object() == nullptr)
	{
		return 0.0;
	}

	float currentSpeed = speed();
	if (currentSpeed == 0.0)
	{
		return 0.0;
	}

	long long pos = 0l;
	if (S_OK != mediaSeekingInterface_.object()->GetCurrentPosition(&pos))
	{
		return 0.0;
	}

	return double(pos) * 0.0000001 / double(currentSpeed);
}

bool DSFiniteMedium::setPosition(const double position)
{
	if (mediaSeekingInterface_.object() == nullptr)
	{
		return false;
	}

	double currentSpeed = double(speed());
	long long pos = NumericD::round64(position * currentSpeed * 10000000.0);

	if (pauseTimestamp_.isValid())
	{
		long long units = NumericT<long long>::maxValue();
		mediaSeekingInterface_.object()->GetDuration(&units);

		pos = max(0ll, pos - 10000000ll);
		long long stopPosition = min(pos + 20000000ll, units);

		stopTimestamp_.toInvalid();

		return S_OK == mediaSeekingInterface_.object()->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, &stopPosition, AM_SEEKING_AbsolutePositioning);
	}
	else
	{
		return S_OK == mediaSeekingInterface_.object()->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
	}
}

float DSFiniteMedium::speed() const
{
	return mediumSpeed_;
}

bool DSFiniteMedium::setSpeed(const float speed)
{
	if (mediaSeekingInterface_.object() == nullptr)
	{
		mediumSpeed_ = speed;
		return true;
	}

	if (S_OK == mediaSeekingInterface_.object()->SetRate(double(speed)))
	{
		mediumSpeed_ = speed;
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
	mediumHasStopped_ = false;

	if (mediaSeekingInterface_.object() != nullptr)
	{
		mediaSeekingInterface_.object()->SetRate(double(mediumSpeed_));
	}

	return DSGraphObject::startGraph();
}

bool DSFiniteMedium::createFiniteInterface()
{
	ocean_assert(filterGraph_.object() != nullptr);

	if (mediaSeekingInterface_.object() != nullptr)
	{
		return true;
	}

	bool noError = true;

	if (noError && S_OK != filterGraph_.object()->QueryInterface(IID_IMediaSeeking, (void**)(&mediaSeekingInterface_.resetObject())))
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
	mediaSeekingInterface_.release();
}

void DSFiniteMedium::mediumHasStopped()
{
	mediumHasStopped_ = true;
}

void DSFiniteMedium::onScheduler()
{
	if (graphIsRunning_ && (type_ & FRAME_MEDIUM) == 0)
	{
		long code = 0;
		LONG_PTR param1 = 0;
		LONG_PTR param2 = 0;

		if (mediaEventInterface_.object() != nullptr && S_OK == mediaEventInterface_.object()->GetEvent(&code, &param1, &param2, 0))
		{
			mediaEventInterface_.object()->FreeEventParams(code, param1, param2);

			if (code == EC_COMPLETE)
			{
				mediumHasStopped_ = true;
			}
		}
	}

	if (mediumHasStopped_)
	{
		mediumHasStopped_ = false;

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
