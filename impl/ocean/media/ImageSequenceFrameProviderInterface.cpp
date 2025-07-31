/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageSequenceFrameProviderInterface.h"

#include "ocean/base/TaskQueue.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

ImageSequenceFrameProviderInterface::ImageSequenceFrameProviderInterface(const ImageSequenceRef& imageSequence) :
	imageSequence_(imageSequence)
{
	if (!imageSequence_.isNull())
	{
		ocean_assert(imageSequence_->isExclusive() && "The provided image sequence should be exclusive!");

		imageSequence_->setPreferredFrameFrequency(FrameMedium::FrameFrequency(0));
		imageSequence_->setMode(ImageSequence::SM_EXPLICIT);
	}
}

ImageSequenceFrameProviderInterface::~ImageSequenceFrameProviderInterface()
{
	// we simply want to ensure that no asynchronous request is left

	TemporaryScopedLock temporaryScopedLock(lock_);
		if (pendingAsynchronousRequests_ == 0u)
		{
			return;
		}
	temporaryScopedLock.release();

	const Timestamp startTimestamp(true);

	while (startTimestamp + 10.0 < Timestamp(true))
	{
		Thread::sleep(1u);

		const ScopedLock scopedLock(lock_);

		if (pendingAsynchronousRequests_ == 0u)
		{
			return;
		}
	}
	while (pendingAsynchronousRequests_ != 0u);

	ocean_assert(false && "Still pending asynchronous requests");
}

bool ImageSequenceFrameProviderInterface::isInitialized()
{
	const ScopedLock scopedLock(lock_);

	return !imageSequence_.isNull() && imageSequence_->isValid();
}

bool ImageSequenceFrameProviderInterface::setPreferredFrameType(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isInitialized());

	preferredPixelFormat_ = pixelFormat;
	preferredPixelOrigin_ = pixelOrigin;

	return true;
}

void ImageSequenceFrameProviderInterface::asynchronFrameRequest(const unsigned int index, const bool /*priority*/)
{
	TemporaryScopedLock temporaryScopedLock(lock_);
		++pendingAsynchronousRequests_;
	temporaryScopedLock.release();

	TaskQueue::get().pushTask(TaskQueue::Task::create(*this, &ImageSequenceFrameProviderInterface::handleAsynchronFrameRequest, index));
}

FrameRef ImageSequenceFrameProviderInterface::synchronFrameRequest(const unsigned int index, const double /*timeout*/, bool* /*abort*/)
{
	const ScopedLock scopedLock(lock_);

	if (imageSequence_.isNull() || !imageSequence_->isValid())
	{
		return FrameRef();
	}

	if (imageSequence_->setPosition(double(index)) && imageSequence_->index() == index)
	{
		imageSequence_->start();
		const FrameRef frameRef(imageSequence_->frame());
		imageSequence_->stop();

		if (frameRef.isNull())
		{
			return FrameRef();
		}

		const FrameType preferredFrameType(*frameRef, preferredPixelFormat_, preferredPixelOrigin_);

		if (preferredFrameType.isValid() && preferredFrameType != frameRef->frameType())
		{
			Frame preferredFrame;
			if (CV::FrameConverter::Comfort::convert(*frameRef, preferredFrameType.pixelFormat(), preferredFrameType.pixelOrigin(), preferredFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
			{
				return FrameRef(new Frame(std::move(preferredFrame)));
			}
		}

		return FrameRef(new Frame(*frameRef, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));
	}

	return FrameRef();
}

void ImageSequenceFrameProviderInterface::asynchronFrameNumberRequest()
{
	TemporaryScopedLock temporaryScopedLock(lock_);
		++pendingAsynchronousRequests_;
	temporaryScopedLock.release();

	TaskQueue::get().pushTask(TaskQueue::Task::create(*this, &ImageSequenceFrameProviderInterface::handleAsynchronFrameNumberRequest));
}

unsigned int ImageSequenceFrameProviderInterface::synchronFrameNumberRequest(const double /*timeout*/, bool* /*abort*/)
{
	const ScopedLock scopedLock(lock_);

	if (imageSequence_.isNull() || !imageSequence_->isValid())
	{
		return (unsigned int)(-1);
	}

	return imageSequence_->images();
}

void ImageSequenceFrameProviderInterface::asynchronFrameTypeRequest()
{
	TemporaryScopedLock temporaryScopedLock(lock_);
		++pendingAsynchronousRequests_;
	temporaryScopedLock.release();

	TaskQueue::get().pushTask(TaskQueue::Task::create(*this, &ImageSequenceFrameProviderInterface::handleAsynchronFrameTypeRequest));
}

FrameType ImageSequenceFrameProviderInterface::synchronFrameTypeRequest(const double timeout, bool* abort)
{
	const ScopedLock scopedLock(lock_);

	const FrameRef frameRef(synchronFrameRequest(0u, timeout, abort));

	if (!frameRef.isNull())
	{
		const FrameType preferredFrameType(*frameRef, preferredPixelFormat_, preferredPixelOrigin_);

		if (preferredFrameType.isValid() && CV::FrameConverter::Comfort::isSupported(frameRef->frameType(), preferredFrameType.pixelFormat()))
		{
			return preferredFrameType;
		}

		return frameRef->frameType();
	}

	return FrameType();
}

void ImageSequenceFrameProviderInterface::handleAsynchronFrameRequest(const unsigned int index)
{
	if (!frameCallbacks_.isEmpty())
	{
		const FrameRef frame = synchronFrameRequest(index);

		if (frame)
		{
			frameCallbacks_(frame, index);
		}
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(pendingAsynchronousRequests_ > 0u);
	--pendingAsynchronousRequests_;
}

void ImageSequenceFrameProviderInterface::handleAsynchronFrameNumberRequest()
{
	if (!frameNumberCallbacks_.isEmpty())
	{
		const unsigned int frameNumber = synchronFrameNumberRequest();

		if (frameNumber != (unsigned int)(-1))
		{
			frameNumberCallbacks_(frameNumber);
		}
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(pendingAsynchronousRequests_ > 0u);
	--pendingAsynchronousRequests_;
}

void ImageSequenceFrameProviderInterface::handleAsynchronFrameTypeRequest()
{
	if (!frameTypeCallbacks_.isEmpty())
	{
		const FrameType frameType = synchronFrameTypeRequest();

		if (frameType.isValid())
		{
			frameTypeCallbacks_(frameType);
		}
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(pendingAsynchronousRequests_ > 0u);
	--pendingAsynchronousRequests_;
}

}

}
