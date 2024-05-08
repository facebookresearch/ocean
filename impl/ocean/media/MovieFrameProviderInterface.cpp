/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/MovieFrameProviderInterface.h"

#include "ocean/base/Scheduler.h"

namespace Ocean
{

namespace Media
{

MovieFrameProviderInterface::MovieFrameProviderInterface(const MovieFrameProviderRef& movieFrameProvider) :
	movieFrameProvider_(movieFrameProvider)
{
	Scheduler::get().registerFunction(Scheduler::Callback::create(*this, &MovieFrameProviderInterface::onScheduler));

	ocean_assert(movieFrameProvider_);

	if (movieFrameProvider_)
	{
		movieFrameProvider_->addFrameCallback(MovieFrameProvider::FrameCallback::create(*this, &MovieFrameProviderInterface::onFrame));
	}
}

MovieFrameProviderInterface::~MovieFrameProviderInterface()
{
	Scheduler::get().unregisterFunction(Scheduler::Callback::create(*this, &MovieFrameProviderInterface::onScheduler));

	release();
}

bool MovieFrameProviderInterface::isInitialized()
{
	const ScopedLock scopedLock(lock_);

	if (!movieFrameProvider_ || movieFrameProvider_->actualFrameNumber() == 0u)
	{
		return false;
	}

	return true;
}

bool MovieFrameProviderInterface::setPreferredFrameType(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isInitialized());

	return movieFrameProvider_ && movieFrameProvider_->setPreferredFrameType(pixelFormat, pixelOrigin);
}

void MovieFrameProviderInterface::asynchronFrameRequest(const unsigned int index, const bool priority)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(movieFrameProvider_);

	if (movieFrameProvider_)
	{
		movieFrameProvider_->asynchronFrameRequest(index, priority);
	}
}

FrameRef MovieFrameProviderInterface::synchronFrameRequest(const unsigned int index, const double timeout, bool* abort)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(movieFrameProvider_);

	if (movieFrameProvider_)
	{
		return movieFrameProvider_->synchronFrameRequest(index, timeout, abort);
	}

	return FrameRef();
}

void MovieFrameProviderInterface::frameCacheRequest(const unsigned int index, const int range)
{
	const ScopedLock scopedLock(lock_);

	const unsigned int actualFrames = movieFrameProvider_->actualFrameNumber();
	ocean_assert(index <= actualFrames);

	if (actualFrames != 0u && index < actualFrames)
	{
		const int endIndex = range < 0 ? max(-1, int(index) + range - 1) : min(int(index) + range + 1, int(actualFrames));
		const int offset = range < 0 ? -1 : 1;

		for (int i = int(index); i != endIndex; i += offset)
		{
			ocean_assert(i >= 0 && i < int(actualFrames));
			movieFrameProvider_->asynchronFrameRequest(i, false);
		}
	}
}

void MovieFrameProviderInterface::asynchronFrameNumberRequest()
{
	const ScopedLock scopedLock(lock_);

	asynchronousFrameNumber_ = true;
}

unsigned int MovieFrameProviderInterface::synchronFrameNumberRequest(const double timeout, bool* abort)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(timeout >= 0.0);
	ocean_assert(movieFrameProvider_);

	if (movieFrameProvider_.isNull())
	{
		return (unsigned int)(-1);
	}

	const unsigned int knownFrames = movieFrameProvider_->actualFrameNumber();
	if (knownFrames != 0u)
	{
		return knownFrames;
	}

	const Timestamp startTimestamp(true);
	while (startTimestamp + timeout > Timestamp(true) && (abort == nullptr || *abort == false))
	{
		Thread::sleep(1u);

		const unsigned int frames = movieFrameProvider_->actualFrameNumber();
		if (frames != 0u)
		{
			return frames;
		}
	}

	return (unsigned int)(-1);
}

void MovieFrameProviderInterface::asynchronFrameTypeRequest()
{
	const ScopedLock scopedLock(lock_);

	asynchronousFrameType_ = true;
}

FrameType MovieFrameProviderInterface::synchronFrameTypeRequest(const double timeout, bool* abort)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(timeout >= 0.0);
	ocean_assert(movieFrameProvider_);

	if (movieFrameProvider_.isNull())
	{
		return FrameType();
	}

	if (movieFrameProvider_->actualFrameNumber() != 0u)
	{
		return movieFrameProvider_->frameType();
	}

	const Timestamp startTimestamp(true);
	while (startTimestamp + timeout > Timestamp(true) && (abort == nullptr || *abort == false))
	{
		Thread::sleep(1u);

		if (movieFrameProvider_->actualFrameNumber() != 0u)
		{
			return movieFrameProvider_->frameType();
		}
	}

	return FrameType();
}

void MovieFrameProviderInterface::release()
{
	const ScopedLock scopedLock(lock_);

	if (movieFrameProvider_)
	{
		movieFrameProvider_->removeFrameCallback(MovieFrameProvider::FrameCallback::create(*this, &MovieFrameProviderInterface::onFrame));
		movieFrameProvider_->setEnabled(false);
		movieFrameProvider_.release();
	}
}

void MovieFrameProviderInterface::onScheduler()
{
	FramePair framePair;

	{
		const ScopedLock scopedLock(lock_);

		if (!frameQueue_.empty())
		{
			framePair = frameQueue_.front();
			frameQueue_.pop();
		}
	}

	// check whether we have to deliver a new frame
	if (framePair.second)
	{
		ocean_assert(framePair.first != (unsigned int)(-1));
		frameCallbacks_(framePair.second, framePair.first);
	}

	// check whether a frame number request exists
	if (asynchronousFrameNumber_)
	{
		unsigned int number = (unsigned int)(-1);

		TemporaryScopedLock temporaryScopedLock(lock_);
			if (movieFrameProvider_->frameNumber() != 0u)
			{
				number = movieFrameProvider_->frameNumber();
				asynchronousFrameNumber_ = false;
			}
		temporaryScopedLock.release();

		// check whether we have to deliver a frame number
		if (number != (unsigned int)(-1))
		{
			frameNumberCallbacks_(number);
		}
	}

	// check whether a frame type request exists
	if (asynchronousFrameType_)
	{
		FrameType frameType;

		TemporaryScopedLock temporaryScopedLock(lock_);
			if (movieFrameProvider_->frameNumber() != 0u)
			{
				frameType = movieFrameProvider_->frameType();
				ocean_assert(frameType.isValid());

				if (frameType.isValid())
				{
					asynchronousFrameType_ = false;
				}
			}
		temporaryScopedLock.release();

		// check whether we have to deliver a frame type
		if (frameType.isValid())
		{
			frameTypeCallbacks_(frameType);
		}
	}
}

void MovieFrameProviderInterface::onFrame(const unsigned int frameIndex, const bool frameRequested)
{
	ocean_assert(movieFrameProvider_);

	// check whether the delivered frame has been requested before and a corresponding callback function has been set
	if (frameRequested && !frameCallbacks_.isEmpty())
	{
		const FrameRef frame(movieFrameProvider_->frame(frameIndex));

		// if the frame still is available in the provider
		if (frame)
		{
			const ScopedLock scopedLock(lock_);

			frameQueue_.push(FramePair(frameIndex, frame));
		}
		else // if the frame has been replaced already we request the frame again
		{
			movieFrameProvider_->asynchronFrameRequest(frameIndex, true);
		}
	}
}

}

}
