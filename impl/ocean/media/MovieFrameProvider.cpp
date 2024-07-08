/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/MovieFrameProvider.h"

#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolator.h"

namespace Ocean
{

namespace Media
{

MovieFrameProvider::MovieFrameProvider(const bool enable, const unsigned int maximalFrameStorage, const unsigned int maximalQueueStorage) :
	Thread("FrameProvider thread"),
	enabled_(enable),
	maximalQueueSize_(maximalQueueStorage),
	frames_(maximalFrameStorage)
{
	if (enable)
	{
		const bool result = startThread();
		ocean_assert_and_suppress_unused(result, result);
	}
}

MovieFrameProvider::~MovieFrameProvider()
{
	release();

	stopThreadExplicitly();
}

bool MovieFrameProvider::setEnabled(const bool state)
{
	const ScopedLock scopedLock(lock_);

	if (enabled_ == state)
	{
		return true;
	}

	if (state)
	{
		ocean_assert(!isThreadActive());

		startThread();

		bool needStart = false;

		for (PreviewFrames::const_iterator i = previewFrames_.begin(); i != previewFrames_.end(); ++i)
		{
			if (i->first == false)
			{
				needStart = true;
				break;
			}
		}

		if (needStart && moviePreview_ && !moviePreview_->start())
		{
			return false;
		}
	}
	else
	{
		ocean_assert(isThreadActive());

		stopThread();

		if (moviePreview_ && !moviePreview_->stop())
		{
			return false;
		}
	}

	enabled_ = state;

	return true;
}

std::string MovieFrameProvider::url() const
{
	const ScopedLock scopedLock(lock_);

	if (movie_)
	{
		return movie_->url();
	}

	return std::string();
}

bool MovieFrameProvider::setMovie(const MovieRef& movie)
{
	if (movie.isNull())
	{
		return false;
	}

	const MovieRef secondMovie(movie->clone());

	if (secondMovie.isNull())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	release();

	moviePreview_ = movie;
	movie_ = secondMovie;

	eventCallbacks_(ET_NEW_MEDIA, 0ull);
	released_ = false;

	if (movie_)
	{
		ocean_assert(movie_->isExclusive() && "The provided movie should be exclusive!");

		if (!movie_->setPosition(0.0))
		{
			Log::error() << "MovieFrameProvider: Failed to set the position in the movie";
			return false;
		}

		if (!movie_->setLoop(false))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!movie_->setUseSound(false))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!movie_->setSpeed(Media::Movie::AS_FAST_AS_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		scopedSubscriptionFrames_ = movie_->addFrameCallback(std::bind(&MovieFrameProvider::onFrame, this, std::placeholders::_1, std::placeholders::_2));
	}

	if (moviePreview_)
	{
		ocean_assert(moviePreview_->isExclusive() && "The provided movie should be exclusive!");

		if (!moviePreview_->stop())
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		mediaDuration_ = moviePreview_->normalDuration();
		if (mediaDuration_ <= 0.0)
		{
			Log::warning() << "MovieFrameProvider: Failed to determine the movie's duration";
		}

		mediaFrameFrequency_ = double(moviePreview_->frameFrequency());

		if (mediaFrameFrequency_ <= 0.0)
		{
			mediaFrameFrequency_ = 30.0;
		}

		mediaFrameTime_ = 1.0 / mediaFrameFrequency_;

		const double dFrames = mediaDuration_ * mediaFrameFrequency_;
		estimatedFrameNumber_ = (unsigned int)(dFrames + 0.5);

		if (!moviePreview_->setPosition(0.0))
		{
			Log::error() << "MovieFrameProvider: Failed to set the position in the movie";
			return false;
		}

		if (!moviePreview_->setLoop(false))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!moviePreview_->setUseSound(false))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!moviePreview_->setSpeed(Media::Movie::AS_FAST_AS_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		// reserve 101% of the estimated frames
		previewFrames_.resize((estimatedFrameNumber_ * 101u) / 100u, PreviewFrame(false, FrameRef()));

		scopedSubscriptionPreviewFrames_ = moviePreview_->addFrameCallback(std::bind(&MovieFrameProvider::onPreviewFrame, this, std::placeholders::_1, std::placeholders::_2));

		if (enabled_)
		{
			if (!moviePreview_->start())
			{
				Log::error() << "MovieFrameProvider: Failed to start the movie";
				return false;
			}
		}
	}

	return true;
}

bool MovieFrameProvider::setPreferredFrameType(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	const ScopedLock scopedLock(lock_);

	if (frameType_.isValid() && !CV::FrameConverter::Comfort::isSupported(frameType_, pixelFormat))
	{
		return false;
	}

	preferredPixelFormat_ = pixelFormat;
	preferredPixelOrigin_ = pixelOrigin;

	return true;
}

bool MovieFrameProvider::setPreferredPreviewDimension(const unsigned int width, const unsigned int height)
{
	if ((width == 0u && height == 0u) || (width != 0u && height != 0u))
	{
		ocean_assert(false && "Invalid preview dimensions!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	const Dimension oldPreviewDimension(determinePreviewDimensions());

	preferredPreviewWidth_ = width;
	preferredPreviewHeight_ = height;

	const Dimension newPreviewDimension(determinePreviewDimensions());

	if (oldPreviewDimension != newPreviewDimension)
	{
		for (PreviewFrames::iterator i = previewFrames_.begin(); i != previewFrames_.end(); ++i)
		{
			i->first = false;
		}
	}

	return true;
}

MovieFrameProvider::Dimension MovieFrameProvider::preferredPreviewDimension() const
{
	const ScopedLock scopedLock(lock_);
	return Dimension(preferredPreviewWidth_, preferredPreviewHeight_);
}

bool MovieFrameProvider::setFrameCapacity(const size_t capacity)
{
	const ScopedLock scopedLock(frameLock_);
	frames_.setCapacity(capacity);

	return true;
}

unsigned int MovieFrameProvider::actualFrameNumber()
{
	const ScopedLock scopedLock(lock_);
	return actualFrameNumber_;
}

unsigned int MovieFrameProvider::estimatedFrameNumber()
{
	const ScopedLock scopedLock(lock_);
	return estimatedFrameNumber_;
}

unsigned int MovieFrameProvider::currentFrameNumber()
{
	const ScopedLock scopedLock(lock_);
	return currentFrameNumber_;
}

unsigned int MovieFrameProvider::frameNumber()
{
	const ScopedLock scopedLock(lock_);

	if (actualFrameNumber_ != 0u)
	{
		return actualFrameNumber_;
	}

	if (currentFrameNumber_ > estimatedFrameNumber_)
	{
		return currentFrameNumber_;
	}

	return estimatedFrameNumber_;
}

FrameRef MovieFrameProvider::synchronFrameRequest(const unsigned int index, const double timeout, bool* abort)
{
	FrameRef frame(frameRequest(index));

	if (frame)
	{
		return frame;
	}

	bool secondRequest = false;
	const Timestamp startTimestamp(true);

	while (startTimestamp + timeout > Timestamp(true) && (abort == nullptr || *abort == false))
	{
		TemporaryScopedLock scopedLock(frameLock_);

		if (frames_.element(index, frame))
		{
			return frame;
		}

		scopedLock.release();

		// here we check whether we should re-invoke the frame request as it might be that the first request failed and thus the frame would never arrive
		if (!secondRequest && startTimestamp + timeout * 0.5 > Timestamp(true))
		{
			secondRequest = true;
			frame = frameRequest(index);

			if (frame)
			{
				return frame;
			}
		}

		sleep(1u);
	}

	return FrameRef();
}

void MovieFrameProvider::asynchronFrameRequest(const unsigned int index, const bool priority)
{
	const ScopedLock scopedLock(frameLock_);

	if (priority)
	{
		requestList_.push_front(index);
	}
	else
	{
		requestList_.push_back(index);
	}
}

FrameRef MovieFrameProvider::frame(const unsigned index)
{
	const ScopedLock scopedLock(frameLock_);

	FrameRef frameRef;
	frames_.element(index, frameRef);

	return frameRef;
}

FrameRef MovieFrameProvider::frameRequest(const unsigned int index)
{
	{
		const ScopedLock scopedLock(frameLock_);

		FrameRef frameRef;
		if (frames_.element(index, frameRef))
		{
			return frameRef;
		}
	}

	asynchronFrameRequest(index);

	return FrameRef();
}

FrameRef MovieFrameProvider::previewFrame(const unsigned int index)
{
	const ScopedLock scopedLock(frameLock_);

	ocean_assert(index < previewFrames_.size());
	if (index < currentFrameNumber_)
	{
		return previewFrames_[index].second;
	}

	return FrameRef();
}

FrameRefs MovieFrameProvider::previewFrames(const unsigned int index, const unsigned int size, const unsigned int zoom)
{
	ocean_assert(zoom >= 1u);

	const unsigned int internalZoom = max(1u, zoom);

	FrameRefs frames;
	frames.reserve(size);

	const ScopedLock scopedLock(frameLock_);

	for (unsigned int n = 0u; n < size; ++n)
	{
		const unsigned int i = index + n * internalZoom;

		if (i < previewFrames_.size())
		{
			frames.push_back(previewFrames_[i].second);
		}
		else
		{
			frames.push_back(FrameRef());
		}
	}

	return frames;
}

MovieFrameProvider::Dimension MovieFrameProvider::determinePreviewDimensions() const
{
	const ScopedLock scopedLock(lock_);

	if (frameType_.width() == 0u || frameType_.height() == 0u || (preferredPreviewWidth_ == 0u && preferredPreviewHeight_ == 0u))
	{
		return Dimension(0u, 0u);
	}

	return Dimension(max(1u, preferredPreviewHeight_ ? ((frameType_.width() * preferredPreviewHeight_ + frameType_.height() / 2u) / frameType_.height()) : preferredPreviewWidth_),
						max(1u, preferredPreviewHeight_ ? preferredPreviewHeight_ : ((frameType_.height() * preferredPreviewWidth_ + frameType_.width() / 2u) / frameType_.width())));
}

unsigned int MovieFrameProvider::previewProgress() const
{
	const ScopedLock scopedLock(lock_);

	if (estimatedFrameNumber_ == 0u)
	{
		return 0u;
	}

	if (actualFrameNumber_ != 0u)
	{
		return 100u;
	}

	return min(currentFrameNumber_ * 100u / estimatedFrameNumber_, 100u);
}

void MovieFrameProvider::release()
{
	released_ = true;

	TemporaryScopedLock frameScopedLock(frameLock_);
		previewFrameQueue_ = FrameQueue();
		frameQueue_ = FrameQueue();
	frameScopedLock.release();

	{
		const ScopedLock scopedLock(lock_);

		actualFrameNumber_ = 0u;
		estimatedFrameNumber_ = 0u;
		currentFrameNumber_ = 0u;

		frameRequestIndex_ = (unsigned int)(-1);
		lastFrameEventTimestamp_.toInvalid();

		scopedSubscriptionPreviewFrames_.release();
		scopedSubscriptionFrames_.release();

		if (moviePreview_)
		{
			moviePreview_->stop();
			moviePreview_.release();
		}

		if (movie_)
		{
			movie_->stop();
			movie_.release();
		}
	}


	frameScopedLock.relock(frameLock_);

	frames_.clear();
	previewFrames_.clear();
	previewFrameQueue_ = FrameQueue();
	frameQueue_ = FrameQueue();
}

void MovieFrameProvider::threadRun()
{
	bool maySleep;

	while (!shouldThreadStop())
	{
		maySleep = true;

		if (actualFrameNumber_ == 0u)
		{
			frameLock_.lock();
			const bool isEmpty = previewFrameQueue_.empty();
			frameLock_.unlock();

			const ScopedLock scopedLock(lock_);
			if (isEmpty && moviePreview_ && moviePreview_->taskFinished())
			{
				actualFrameNumber_ = currentFrameNumber_;

				eventCallbacks_(ET_PREVIEW_COMPLETED, actualFrameNumber_);
			}
		}

		// check whether preview frames have to be updated
		{
			TemporaryScopedLock temporalScopedLock(frameLock_);
				FrameRef frameRef;

				if (!previewFrameQueue_.empty())
				{
					frameRef = previewFrameQueue_.front();
					previewFrameQueue_.pop();
				}
			temporalScopedLock.release();

			if (frameRef)
			{
				ocean_assert(frameRef && *frameRef);

				if (double(frameRef->relativeTimestamp()) >= 0.0)
				{
					const ScopedLock scopecdLock(lock_);

					if (frameType_ != frameRef->frameType())
					{
						frameType_ = frameRef->frameType();

						eventCallbacks_(ET_FRAME_SIZE_CHANGED, ((unsigned long long)(frameType_.width()) << 32ull) | (unsigned long long)(frameType_.height()));
					}

					ocean_assert(frameType_.width() != 0u && frameType_.height() != 0u);
					ocean_assert(preferredPreviewWidth_ != 0u || preferredPreviewHeight_ != 0u);
					ocean_assert(preferredPreviewWidth_ * preferredPreviewHeight_ == 0u);

					const Dimension previewDimension(determinePreviewDimensions());

					const unsigned int index = timestamp2index(double(frameRef->relativeTimestamp()));

					if (index != (unsigned int)(-1))
					{
						handlePreviewFrame(*frameRef, index, previewDimension.first, previewDimension.second);
					}
				}
				else
				{
					ocean_assert(false && "Invalid timestamp!");
				}
			}
		}

		// check for already existing requested frames and handle the pending frames in the frame queue
		{
			const ScopedLock scopedLock(frameLock_);

			// check if an requested frame already exists in the database
			for (RequestList::iterator i = requestList_.begin(); i != requestList_.end(); /* noop */)
			{
				if (frames_.refreshElement(*i))
				{
					frameCallbacks_(*i, true);
					i = requestList_.erase(i);

					maySleep = false;
				}
				else
				{
					++i;
				}
			}

			if (!frameQueue_.empty())
			{
				ocean_assert(frameQueue_.front());

				const unsigned int index = timestamp2index(double(frameQueue_.front()->relativeTimestamp()), true);

				if (index != (unsigned int)(-1))
				{
					const FrameRef frame = frameQueue_.front();
					ocean_assert(frame);

					if (preferredPixelFormat_ != FrameType::FORMAT_UNDEFINED && preferredPixelOrigin_ != FrameType::ORIGIN_INVALID
							&& (preferredPixelFormat_ != frame->pixelFormat() || preferredPixelOrigin_ != frame->pixelOrigin()))
					{
						CV::FrameConverter::Comfort::change(*frame, preferredPixelFormat_, preferredPixelOrigin_, false, WorkerPool::get().scopedWorker()());
					}

					frames_.insertElement(index, frame, true);
					frameCallbacks_(index, index == frameRequestIndex_);
				}

				lastFrameEventTimestamp_.toNow();
				if (index == frameRequestIndex_)
				{
					frameRequestIndex_ = (unsigned int)(-1);
					movieStopTimestamp_ = Timestamp(true) + 0.5;
				}

				frameQueue_.pop();

				maySleep = false;
			}
		}

		// checks whether the default movie can be stopped
		if (movieStopTimestamp_.isValid() && Timestamp(true) >= movieStopTimestamp_)
		{
			const ScopedLock scopedLock(lock_);

			if (movie_.isNull())
			{
				break;
			}

			movieStopTimestamp_.toInvalid();
			const bool result = movie_->stop();
			ocean_assert_and_suppress_unused(result, result);
		}

		// check whether the delivery of a new frame can be invoked
		{
			if (frameRequestIndex_ != (unsigned int)(-1))
			{
				ocean_assert(lastFrameEventTimestamp_.isValid());

				// check if the previously requested frame has not been delivered within the specified timeout duration
				if (Timestamp(true) > lastFrameEventTimestamp_ + frameRequestTimeout_)
				{
					eventCallbacks_(ET_REQUESTED_FRAME_FAILED, frameRequestIndex_);
					frameRequestIndex_ = (unsigned int)(-1);
				}
			}

			if (frameRequestIndex_ == (unsigned int)(-1))
			{
				unsigned int requestedIndex = (unsigned int)(-1);

				bool nextFrameRequest = false;

				{
					const ScopedLock scopedLock(lock_);

					if (movie_)
					{
						nextFrameRequest = movie_->taskFinished();
					}
				}

				if (nextFrameRequest)
				{
					{
						const ScopedLock scopedLock(frameLock_);

						if (!requestList_.empty())
						{
							requestedIndex = requestList_.front();
							requestList_.pop_front();
						}
					}

					{
						const ScopedLock scopedLock(lock_);

						const double timestamp = (requestedIndex == (unsigned int)(-1)) ? -1.0 : index2timestamp(requestedIndex);

						if (movie_ && timestamp >= 0.0)
						{
							bool result = movie_->setPosition(max(0.0, timestamp - 0.25));
							ocean_assert_and_suppress_unused(result, result);

							result = movie_->start();
							ocean_assert(result);

							frameRequestIndex_ = requestedIndex;
							lastFrameEventTimestamp_.toNow();

							maySleep = false;
						}
					}
				}
			}
		}

		if (maySleep)
		{
			sleep(1);
		}
	}
}

void MovieFrameProvider::onPreviewFrame(const Frame& frame, const SharedAnyCamera& /*camera*/)
{
	ocean_assert(frame.isValid());
	ocean_assert(enabled_);

	// we do not forward invalid frames
	if (!frame.isValid())
	{
		return;
	}

#ifdef OCEAN_DEBUG
	const Timestamp debugStartTimestamp(true);
#endif

	while (!released_)
	{
		TemporaryScopedLock scopedLock(frameLock_);

		if (previewFrameQueue_.size() < maximalQueueSize_)
		{
			break;
		}

		scopedLock.release();

		sleep(1u);

		ocean_assert(debugStartTimestamp + 10 > Timestamp(true));
	}

	const ScopedLock scopedLock(frameLock_);

	FrameRef frameRef(new Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));
	frameRef->setRelativeTimestamp(frame.relativeTimestamp());

	previewFrameQueue_.push(frameRef);
}

void MovieFrameProvider::onFrame(const Frame& frame, const SharedAnyCamera& /*camera*/)
{
	ocean_assert(frame.isValid());
	ocean_assert(enabled_);

	// we do not forward invalid frames
	if (!frame.isValid())
	{
		return;
	}

#ifdef OCEAN_DEBUG
	const Timestamp debugStartTimestamp(true);
#endif

	while (!released_)
	{
		TemporaryScopedLock scopedLock(frameLock_);

		if (frameQueue_.size() < maximalQueueSize_)
		{
			break;
		}

		scopedLock.release();

		sleep(1u);

		ocean_assert(debugStartTimestamp + 10 > Timestamp(true));
	}

	const ScopedLock scopedLock(frameLock_);

	FrameRef frameRef(new Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));
	frameRef->setRelativeTimestamp(frame.relativeTimestamp());

	frameQueue_.push(std::move(frameRef));
}

bool MovieFrameProvider::handlePreviewFrame(const Frame& frame, const unsigned int index, const unsigned int previewWidth, const unsigned int previewHeight)
{
	ocean_assert(frame);
	ocean_assert(previewWidth != 0u && previewHeight != 0u);
	ocean_assert(frame.width() / previewWidth == frame.height() / previewHeight);

	ocean_assert(index != (unsigned int)(-1));

	bool updatePreviewFrame = true;

	// if the preview frame has been determined already (with the correct dimension)
	if (index < previewFrames_.size() && previewFrames_[index].first)
	{
		updatePreviewFrame = false;
	}

	if (!updatePreviewFrame)
	{
		previewFrameCallbacks_(index);

		return true;
	}

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	// as we have to shrink the preview frame we need a a zipped pixel format
	FrameType intermediateFrameType = frame.frameType();
	if (intermediateFrameType.numberPlanes() != 1u)
	{
		intermediateFrameType = FrameType(intermediateFrameType, FrameType::FORMAT_RGB24);
	}

	if (preferredPixelFormat_ != FrameType::FORMAT_UNDEFINED && preferredPixelOrigin_ != FrameType::ORIGIN_INVALID
			&& FrameType::numberPlanes(preferredPixelFormat_) == 1u
			&& (preferredPixelFormat_ != intermediateFrameType.pixelFormat() || preferredPixelOrigin_ != intermediateFrameType.pixelOrigin()))
	{
		intermediateFrameType = FrameType(intermediateFrameType, preferredPixelFormat_, preferredPixelOrigin_);
	}

	Frame intermediateFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

	if (intermediateFrame.frameType() != intermediateFrameType && !CV::FrameConverter::Comfort::convert(frame, intermediateFrameType.pixelFormat(), intermediateFrameType.pixelOrigin(), intermediateFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
	{
		return false;
	}

	if (updatePreviewFrame)
	{
		Frame previewFrame(FrameType(intermediateFrame, previewWidth, previewHeight));

		if (!CV::FrameInterpolator::resize(intermediateFrame, previewFrame, CV::FrameInterpolator::RM_AUTOMATIC, scopedWorker()))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		previewFrame.setTimestamp(frame.timestamp());
		previewFrame.setRelativeTimestamp(frame.relativeTimestamp());

		const ScopedLock scopedLock(frameLock_);

		if (index >= previewFrames_.size())
		{
			previewFrames_.resize(index + 1, PreviewFrame(false, FrameRef()));
		}

		ocean_assert(index < previewFrames_.size());
		previewFrames_[index] = PreviewFrame(true, FrameRef(new Frame(std::move(previewFrame))));

		ocean_assert(index == 0u || previewFrames_[index - 1u].second->relativeTimestamp() < previewFrames_[index].second->relativeTimestamp());

		currentFrameNumber_ = max(currentFrameNumber_, index + 1u);
	}

	previewFrameCallbacks_(index);

	return true;
}

unsigned int MovieFrameProvider::timestamp2index(const double timestamp, const bool lookup)
{
	const ScopedLock scopedLock(frameLock_);

	if (lookup)
	{
		if (currentFrameNumber_ > 0u && previewFrames_[currentFrameNumber_ - 1u].second->relativeTimestamp() >= Timestamp(timestamp))
		{
			// binary search

			int left = 0;
			int right = int(currentFrameNumber_) - 1;

			do
			{
				const int middle = left + (right - left) / 2;

				ocean_assert(middle < int(previewFrames_.size()));
				ocean_assert(previewFrames_[middle].second);

				const double frameTimestamp = double(previewFrames_[middle].second->relativeTimestamp());

				if (frameTimestamp == timestamp)
				{
					return middle;
				}

				if (frameTimestamp < timestamp)
				{
					// right half
					left = middle + 1u;
				}
				else
				{
					// left half
					right = middle - 1u;
				}

				if (left == right)
				{
					if (previewFrames_[left].second->relativeTimestamp() == Timestamp(timestamp))
					{
						return left;
					}

					return (unsigned int)(-1);
				}
			}
			while (left < right);
		}

		return (unsigned int)(-1);
	}

	if (actualFrameNumber_ != 0u)
	{
		// binary search

		int left = 0;
		int right = int(actualFrameNumber_) - 1;

		do
		{
			const int middle = left + (right - left) / 2;

			ocean_assert(middle < int(previewFrames_.size()));
			ocean_assert(previewFrames_[middle].second);

			const double frameTimestamp = double(previewFrames_[middle].second->relativeTimestamp());

			if (frameTimestamp == timestamp)
			{
				return middle;
			}

			if (frameTimestamp < timestamp)
			{
				// right half
				left = middle + 1u;
			}
			else
			{
				// left half
				right = middle - 1u;
			}

			if (left == right)
			{
				ocean_assert(previewFrames_[left].second->relativeTimestamp() == Timestamp(timestamp));
				return left;
			}
		}
		while (left < right);
	}
	else
	{
		if (currentFrameNumber_ == 0u)
		{
			return currentFrameNumber_;
		}

		if (previewFrames_[currentFrameNumber_ - 1u].second && previewFrames_[currentFrameNumber_ - 1u].second->relativeTimestamp() < Timestamp(timestamp))
		{
			return currentFrameNumber_;
		}

		for (unsigned int n = 0u; n < previewFrames_.size(); ++n)
		{
			const PreviewFrame& previewFrame = previewFrames_[n];

			if (previewFrame.second && previewFrame.second->relativeTimestamp() == Timestamp(timestamp))
			{
				return n;
			}
		}
	}

	return (unsigned int)(-1);
}

double MovieFrameProvider::index2timestamp(const unsigned int index)
{
	const ScopedLock scopedLock(frameLock_);

	if (actualFrameNumber_ != 0u)
	{
		if (index < actualFrameNumber_)
		{
			return double(previewFrames_[index].second->relativeTimestamp());
		}
	}
	else
	{
		if (index < previewFrames_.size() && previewFrames_[index].second)
		{
			return double(previewFrames_[index].second->relativeTimestamp());
		}
	}

	return -1.0;
}

}

}
