/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/FrameTracker.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/ScopedValue.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

FrameTracker::TrackerComponent::TrackerComponent(FrameTracker& parent) :
	parent_(parent)
{
	// nothing to do here
}

FrameTracker::TrackerComponent::~TrackerComponent()
{
	// nothing to do here
}

bool FrameTracker::TrackerComponent::invoke(const TrackerEvent& startedEvent, TrackerEvent& breakEvent, TrackerEvent& finishedEvent, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	const ScopedEvent scopedEvent(startedEvent, breakEvent, finishedEvent, parent_.eventCallbacks_);

	if (!onStart(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	if (!onStartIncreasing())
	{
		return false;
	}

	const unsigned int totalIterations = upperFrameIndex - lowerFrameIndex + 1u + (lowerFrameIndex == initialFrameIndex ? 0u : 1u);
	ocean_assert(totalIterations >= 1u);

	unsigned int iteration = 0u;
	unsigned int previousIndex = (unsigned int)(-1);

	// increasing frame index
	for (unsigned int currentIndex = initialFrameIndex; currentIndex <= upperFrameIndex && !parent_.shouldThreadStop(); ++currentIndex)
	{
		if (!onSetupIteration(currentIndex))
		{
			return false;
		}

		// preload the next frames so that is can be accessed faster
		if (currentIndex + 1u <= upperFrameIndex)
		{
			parent_.frameProviderInterface_->frameCacheRequest(currentIndex + 1u, 9);
		}

		const IterationResult result = onFrame(previousIndex, currentIndex, iteration++, totalIterations);
		previousIndex = currentIndex;

		ocean_assert(totalIterations != 0u);
		parent_.updateTrackerProgress(Scalar(iteration) / Scalar(totalIterations));

		if (result == IR_FINISHED)
		{
			break;
		}

		if (result == IR_FAILED)
		{
			return false;
		}

		ocean_assert(result == IR_SUCCEEDED);
	}

	if (!parent_.shouldThreadStop() && lowerFrameIndex != initialFrameIndex)
	{
		ocean_assert(lowerFrameIndex < initialFrameIndex);

		if (!onStartDecreasing())
		{
			return false;
		}

		previousIndex = (unsigned int)(-1);

		// decreasing frame index
		for (int currentIndex = int(initialFrameIndex); currentIndex >= int(lowerFrameIndex) && !parent_.shouldThreadStop(); --currentIndex)
		{
			ocean_assert(currentIndex >= 0);

			if (!onSetupIteration((unsigned int)currentIndex))
			{
				return false;
			}

			// preload the next frames so that is can be accessed faster
			if (currentIndex > int(lowerFrameIndex))
			{
				parent_.frameProviderInterface_->frameCacheRequest(currentIndex - 1u, -9);
			}

			const IterationResult result = onFrame(previousIndex, (unsigned int)currentIndex, iteration++, totalIterations);
			previousIndex = (unsigned int)currentIndex;

			ocean_assert(totalIterations != 0u);
			parent_.updateTrackerProgress(Scalar(iteration) / Scalar(totalIterations));

			if (result == IR_FINISHED)
			{
				break;
			}

			if (result == IR_FAILED)
			{
				return false;
			}

			ocean_assert(result == IR_SUCCEEDED);
		}
	}

	if (!parent_.shouldThreadStop() && !onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	scopedEvent.leave();
	return !parent_.shouldThreadStop();
}

bool FrameTracker::TrackerComponent::onStart(const unsigned int /*lowerFrameIndex*/, const unsigned int /*initialFrameIndex*/, const unsigned int /*upperFrameIndex*/)
{
	return true;
}

bool FrameTracker::TrackerComponent::onStartIncreasing()
{
	return true;
}

bool FrameTracker::TrackerComponent::onStartDecreasing()
{
	return true;
}

bool FrameTracker::TrackerComponent::onSetupIteration(const unsigned int /*index*/)
{
	return true;
}

bool FrameTracker::TrackerComponent::onStop(const unsigned int /*lowerFrameIndex*/, const unsigned int /*initialFrameIndex*/, const unsigned int /*upperFrameIndex*/)
{
	return true;
}

FrameTracker::FrameTrackerComponent::FrameTrackerComponent(FrameTracker& tracker) :
	TrackerComponent(tracker)
{
	// nothing to do here
}

bool FrameTracker::FrameTrackerComponent::onStartDecreasing()
{
	previousFrame_.release();
	currentFrame_.release();

	return true;
}

bool FrameTracker::FrameTrackerComponent::onSetupIteration(const unsigned int index)
{
	// swap the image points and the frame pyramid from the previous iteration
	std::swap(previousFrame_, currentFrame_);

	const FrameRef frame(parent_.frameProviderInterface_->synchronFrameRequest(index, 10.0, &parent_.shouldStop_));

	if (frame.isNull() || frame->width() != parent_.camera_.width() || frame->height() != parent_.camera_.height())
	{
		ocean_assert(parent_.shouldThreadStop() && "This should never happen!");
		return false;
	}

	const FrameType::PixelFormat targetPixelFormat = FrameType::formatRemoveAlphaChannel(FrameType::genericSinglePlanePixelFormat(frame->pixelFormat()));
	if (!CV::FrameConverter::Comfort::convert(*frame, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT, currentFrame_, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	return true;
}

FrameTracker::FramePyramidTrackerComponent::FramePyramidTrackerComponent(FrameTracker& tracker) :
	TrackerComponent(tracker)
{
	// nothing to do here
}

bool FrameTracker::FramePyramidTrackerComponent::onStartIncreasing()
{
	ocean_assert(parent_.frameProviderInterface_);
	ocean_assert(parent_.camera_.width() > 40u && parent_.camera_.height() > 40u);
	if (!parent_.frameProviderInterface_ || parent_.camera_.width() <= 40u || parent_.camera_.height() <= 40u)
	{
		return false;
	}

	framePyramidLayers_ = CV::FramePyramid::idealLayers(parent_.camera_.width(), parent_.camera_.height(), 20u, 20u, 2u, parent_.camera_.width() / 10u);
	ocean_assert(framePyramidLayers_ >= 1u);

	return true;
}

bool FrameTracker::FramePyramidTrackerComponent::onStartDecreasing()
{
	previousFramePyramid_.clear();
	currentFramePyramid_.clear();

	return true;
}

bool FrameTracker::FramePyramidTrackerComponent::onSetupIteration(const unsigned int index)
{
	// swap the frame pyramid from the previous iteration
	std::swap(previousFramePyramid_, currentFramePyramid_);

	const FrameRef frame(parent_.frameProviderInterface_->synchronFrameRequest(index, 10.0, &parent_.shouldStop_));

	if (frame.isNull() || frame->width() != parent_.camera_.width() || frame->height() != parent_.camera_.height())
	{
		ocean_assert(parent_.shouldThreadStop() && "This should never happen!");
		return false;
	}

	const FrameType::PixelFormat targetPixelFormat = FrameType::formatRemoveAlphaChannel(FrameType::genericSinglePlanePixelFormat(frame->pixelFormat()));

	Frame currentFrame;
	if (!CV::FrameConverter::Comfort::convert(*frame, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT, currentFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	// we create the frame pyramid without explicit Gaussian filtering
	return currentFramePyramid_.replace8BitPerChannel11(currentFrame, framePyramidLayers_, true /*copyFirstLayer*/, WorkerPool::get().scopedWorker()());
}

bool FrameTracker::succeeded() const
{
	return succeeded_;
}

bool FrameTracker::start()
{
	const ScopedLock scopedLock(lock_);

	if (frameProviderInterface_.isNull())
	{
		return false;
	}

	return OfflineTracker::start();
}

bool FrameTracker::setFrameProviderInterface(const CV::FrameProviderInterfaceRef& frameProviderInterface)
{
	const ScopedLock scopedLock(lock_);

	if (running())
	{
		return false;
	}

	if (frameProviderInterface_)
	{
		return false;
	}

	frameProviderInterface_ = frameProviderInterface;
	return true;
}

bool FrameTracker::setTrackingFrameRange(const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, const unsigned int startFrameIndex)
{
	ocean_assert(lowerFrameIndex <= upperFrameIndex);
	ocean_assert(startFrameIndex == (unsigned int)(-1) || (lowerFrameIndex <= startFrameIndex && startFrameIndex <= upperFrameIndex));

	const ScopedLock scopedLock(lock_);

	if (running())
	{
		return false;
	}

	if (startFrameIndex != (unsigned int)(-1) && (startFrameIndex < lowerFrameIndex || startFrameIndex > upperFrameIndex))
	{
		return false;
	}

	lowerFrameIndex_ = lowerFrameIndex;
	startFrameIndex_ = startFrameIndex;
	upperFrameIndex_ = upperFrameIndex;

	offlinePoses_.setFirstIndex(lowerFrameIndex);
	offlinePoses_.resize(upperFrameIndex - lowerFrameIndex + 1u);

	return true;
}

bool FrameTracker::setCamera(const PinholeCamera& pinholeCamera, const Scalar cameraPrecision, const bool optimizeCamera)
{
	ocean_assert(pinholeCamera.isValid());

	camera_ = pinholeCamera;
	cameraPrecision_ = cameraPrecision;

	if (camera_.isValid())
	{
		optimizeCamera_ = optimizeCamera;
	}
	else
	{
		ocean_assert(optimizeCamera);
		optimizeCamera_ = true;
		cameraPrecision_ = -1;
	}

	return true;
}

void FrameTracker::threadRun()
{
	const TrackerProcessComponentEvent startEvent(id(), ComponentEvent::CS_STARTED);
	TrackerProcessComponentEvent failedEvent(id(), ComponentEvent::CS_FAILED);
	TrackerProcessComponentEvent finishedEvent(id(), ComponentEvent::CS_FINISHED);
	const ScopedEvent scopedEvent(startEvent, failedEvent, finishedEvent, eventCallbacks_);

	ocean_assert(finished_ == false);
	const ScopedValueT<bool> scopedFinishedValue(finished_, true, false);
	ScopedValueT<bool> scopedSucceededValue(succeeded_, false, false);

	ocean_assert(frameProviderInterface_);

	const FrameType frameType(applyFrameTrackingInitialization());

	if (frameType.isValid())
	{
		if (applyFrameTracking(frameType))
		{
			// now the tracker has succeeded
			scopedSucceededValue.setDelayed(true);
			scopedEvent.leave();
		}
		else if (shouldThreadStop())
		{
			Log::info() << "Tracker stopped due to user request.";

			// an external request has stopped the tracker
			failedEvent.setState(ComponentEvent::CS_BROKE);
		}
		else
		{
			Log::info() << "Tracker failed!";
		}
	}
}

FrameType FrameTracker::applyFrameTrackingInitialization()
{
	ocean_assert(frameProviderInterface_);
	if (frameProviderInterface_.isNull())
	{
		return FrameType();
	}

	// wait until the frame provider interface has been initialized and can be used, or stops if the thread is requested to stop
	while (!frameProviderInterface_->isInitialized() && !shouldThreadStop())
	{
		sleep(1u);
	}

	if (!frameProviderInterface_->isInitialized())
	{
		return FrameType();
	}

	const FrameType frameType(frameProviderInterface_->synchronFrameTypeRequest(1.0, &shouldStop_));
	if (!frameType.isValid())
	{
		return FrameType();
	}

	const unsigned int frameNumbers = frameProviderInterface_->synchronFrameNumberRequest(1.0, &shouldStop_);
	if (frameNumbers == (unsigned int)(-1) || frameNumbers == 0u)
	{
		return FrameType();
	}

	if (offlinePoses_.firstIndex() >= OfflinePoses::Index(frameNumbers))
	{
		return FrameType();
	}

	// check whether the limits fit with the number of frames
	if (offlinePoses_.isEmpty() || offlinePoses_.lastIndex() >= OfflinePoses::Index(frameNumbers))
	{
		offlinePoses_.resize(frameNumbers - offlinePoses_.firstIndex());
	}

	return frameType;
}

void FrameTracker::updateCamera(const PinholeCamera& pinholeCamera)
{
	const ScopedLock scopedLock(lock_);

	camera_ = pinholeCamera;
	eventCallbacks_(CameraCalibrationStateEvent(id_, camera_));
}

}

}

}
