/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/FrameMedium.h"

#include "ocean/base/Thread.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

FrameMedium::SortableFrameType::SortableFrameType(const MediaFrameType& frameType) :
	preferableFrameType_(frameType)
{
	// nothing to do here
}

FrameMedium::SortableFrameType::SortableFrameType(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const FrameFrequency frequency) :
	preferableFrameType_(width, height, pixelFormat, pixelOrigin, frequency)
{
	// nothing to do here
}

bool FrameMedium::SortableFrameType::operator<(const SortableFrameType& right) const
{
	ocean_assert(preferableFrameType_ == right.preferableFrameType_);

	if (preferableFrameType_.width() != 0u)
	{
		const int leftDifference = abs(int(preferableFrameType_.width()) - int(actualFrameType_.width()));
		const int rightDifference = abs(int(preferableFrameType_.width()) - int(right.actualFrameType_.width()));

		if (leftDifference < rightDifference)
		{
			return true;
		}

		if (rightDifference < leftDifference)
		{
			return false;
		}
	}

	if (preferableFrameType_.height() != 0u)
	{
		const int leftDifference = abs(int(preferableFrameType_.height()) - int(actualFrameType_.height()));
		const int rightDifference = abs(int(preferableFrameType_.height()) - int(right.actualFrameType_.height()));

		if (leftDifference < rightDifference)
		{
			return true;
		}

		if (rightDifference < leftDifference)
		{
			return false;
		}
	}

	if (preferableFrameType_.frequency() != 0u)
	{
		const FrameFrequency leftDifference = NumericT<FrameFrequency>::abs(preferableFrameType_.frequency() - actualFrameType_.frequency());
		const FrameFrequency rightDifference = NumericT<FrameFrequency>::abs(preferableFrameType_.frequency() - right.actualFrameType_.frequency());

		if (leftDifference < rightDifference)
		{
			return true;
		}

		if (rightDifference < leftDifference)
		{
			return false;
		}
	}

	if (preferableFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		if (preferableFrameType_.pixelFormat() == actualFrameType_.pixelFormat() && preferableFrameType_.pixelFormat() != right.actualFrameType_.pixelFormat())
		{
			return true;
		}

		if (preferableFrameType_.pixelFormat() == right.actualFrameType_.pixelFormat() && preferableFrameType_.pixelFormat() != actualFrameType_.pixelFormat())
		{
			return false;
		}
	}

	return actualFrameType_.width() < right.actualFrameType_.width();
}

FrameMedium::FrameReceiver::~FrameReceiver()
{
	reset();
}

void FrameMedium::FrameReceiver::onFrame(const Frame& frame, const SharedAnyCamera& camera)
{
	while (true)
	{
		TemporaryScopedLock lock(lock_);

		if (!frame_.isValid())
		{
			ocean_assert(!camera_);

			frame_ = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			camera_ = camera;

			return;
		}

		lock.release();

		Thread::sleep(1u);
	}
}

bool FrameMedium::FrameReceiver::latestFrame(Frame& frame, SharedAnyCamera* camera)
{
	const ScopedLock scopedLock(lock_);

	if (frame_.isValid())
	{
		frame = Frame(frame_, Frame::ACM_USE_KEEP_LAYOUT);

		if (camera != nullptr)
		{
			*camera = camera_;
		}

		return true;
	}

	return false;
}

bool FrameMedium::FrameReceiver::latestFrameAndReset(Frame& frame, SharedAnyCamera* camera)
{
	const ScopedLock scopedLock(lock_);

	if (frame_.isValid())
	{
		frame = Frame(frame_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		if (camera != nullptr)
		{
			*camera = camera_;
		}

		reset();

		return true;
	}

	return false;

}

void FrameMedium::FrameReceiver::reset()
{
	const ScopedLock scopedLock(lock_);

	frame_.release();
	camera_ = nullptr;
}

FrameMedium::FrameMedium(const std::string& url) :
	Medium(url)
{
	type_ = Type(type_ | FRAME_MEDIUM);
}

FrameMedium::~FrameMedium()
{
	// nothing to do here
}

FrameRef FrameMedium::frame(SharedAnyCamera* anyCamera) const
{
	return frameCollection_.recent(anyCamera);
}

FrameRef FrameMedium::frame(const Timestamp timestamp, SharedAnyCamera* anyCamera) const
{
	return frameCollection_.get(timestamp, anyCamera);
}

FrameRef FrameMedium::frameTimeout(const double timeout, SharedAnyCamera* anyCamera) const
{
	ocean_assert(timeout >= 0.0);

	FrameRef frameRef(frame(anyCamera));

	if (frameRef)
	{
		return frameRef;
	}

	const Timestamp startTimestamp(true);

	while (startTimestamp + timeout > Timestamp(true))
	{
		frameRef = frame(anyCamera);

		if (frameRef)
		{
			return frameRef;
		}

		Thread::sleep(1);
	}

	return FrameRef();
}

bool FrameMedium::hasFrame(const Timestamp timestamp) const
{
	return frameCollection_.has(timestamp);
}

HomogenousMatrixD4 FrameMedium::device_T_camera() const
{
	return HomogenousMatrixD4(true);
}

bool FrameMedium::setPreferredFrameDimension(const unsigned int width, const unsigned int height)
{
	preferredFrameType_ = MediaFrameType(preferredFrameType_, width, height);
	return true;
}

bool FrameMedium::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	preferredFrameType_ = MediaFrameType(preferredFrameType_, format);
	return true;
}

bool FrameMedium::setPreferredFrameFrequency(const FrameFrequency frequency)
{
	if (frequency < FrameFrequency(0) || frequency > FrameFrequency(10000))
	{
		return false;
	}

	preferredFrameType_.setFrequency(frequency);
	return true;
}

bool FrameMedium::setCapacity(const size_t capacity)
{
	return frameCollection_.setCapacity(capacity);
}

size_t FrameMedium::capacity() const
{
	return frameCollection_.capacity();
}

FrameMedium::FrameCallbackScopedSubscription FrameMedium::addFrameCallback(FrameCallbackFunction&& frameCallbackFunction)
{
	ocean_assert(frameCallbackFunction);

	assert(frameCallbackHandler_.isThreadSafe_ && "Handler must be thread-safe");

	return frameCallbackHandler_.addCallback(std::move(frameCallbackFunction));
}

bool FrameMedium::syncedFrames(const FrameMediumRefs& frameMediums, const Timestamp lastTimestamp, FrameRefs& frames, SharedAnyCameras& cameras, const unsigned int waitTime, bool* timedOut, HomogenousMatricesD4* device_T_cameras)
{
	if (timedOut != nullptr)
	{
		*timedOut = false;
	}

	if (frameMediums.empty())
	{
		return false;
	}

	frames.clear();
	cameras.clear();

	frames.reserve(frameMediums.size());
	cameras.reserve(frameMediums.size());

	ocean_assert(frameMediums[0]);

	SharedAnyCamera camera;
	FrameRef frame = frameMediums[0]->frame(&camera);

	if (frame.isNull())
	{
		return false;
	}

	const Timestamp timestamp = frame->timestamp();

	if (timestamp <= lastTimestamp)
	{
		// at least the first medium does not yet have a new frame
		return false;
	}

	if (device_T_cameras != nullptr)
	{
		device_T_cameras->clear();
		device_T_cameras->reserve(frameMediums.size());
	}

	frames.emplace_back(std::move(frame));
	cameras.emplace_back(std::move(camera));

	if (device_T_cameras != nullptr)
	{
		device_T_cameras->emplace_back(frameMediums[0]->device_T_camera());
	}

	const Timestamp startTimestamp(true);

	for (size_t n = 1; n < frameMediums.size(); /*noop*/)
	{
		ocean_assert(frameMediums[n]);

		frame = frameMediums[n]->frame(timestamp, &camera);

		if (frame.isNull() || frame->timestamp() != timestamp)
		{
			if (startTimestamp + double(waitTime) * 0.001 > Timestamp(true))
			{
				Thread::sleep(1u);
				continue;
			}
			else
			{
				frames.clear();
				cameras.clear();

				if (timedOut != nullptr)
				{
					*timedOut = true;
				}

				return false;
			}
		}

		frames.emplace_back(std::move(frame));
		cameras.emplace_back(std::move(camera));

		if (device_T_cameras != nullptr)
		{
			device_T_cameras->emplace_back(frameMediums[n]->device_T_camera());
		}

		++n;
	}

	ocean_assert(frames.size() == cameras.size());
	ocean_assert(device_T_cameras == nullptr || frames.size() == device_T_cameras->size());

	return true;
}

bool FrameMedium::deliverNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera)
{
	ocean_assert(frame.isValid());

	if (!frame.isValid())
	{
		return false;
	}

	assert(frameCallbackHandler_.isThreadSafe_ && "Handler must be thread-safe");

	if (frameCallbackHandler_.isEmpty())
	{
		if (frame.isOwner())
		{
			const FrameRef frameRef = frameCollection_.set(std::move(frame), std::move(anyCamera));

			return bool(frameRef);
		}
		else
		{
			const FrameRef frameRef = frameCollection_.set(frame, std::move(anyCamera));

			return bool(frameRef);
		}
	}

	frameCallbackHandler_.callCallbacks(frame, anyCamera);

	return true;
}

}

}
