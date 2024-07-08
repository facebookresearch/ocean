/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/OILImageSequenceRecorder.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/String.h"

#include "ocean/io/File.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILImageSequenceRecorder::OILImageSequenceRecorder()
{
	imageRecorder_.setFilenameSuffixed(false);
}

OILImageSequenceRecorder::~OILImageSequenceRecorder()
{
	stopThreadExplicitly();
}

OILImageSequenceRecorder::RecorderMode OILImageSequenceRecorder::mode() const
{
	return recorderMode;
}

unsigned int OILImageSequenceRecorder::pendingImages() const
{
	const ScopedLock scopedLock(frameQueueLock_);

	return (unsigned int)(frameQueue_.size());
}

OILImageSequenceRecorder::Encoders OILImageSequenceRecorder::frameEncoders() const
{
	static Encoders encoders;

	if (encoders.empty())
	{
		encoders.push_back("bmp");
		encoders.push_back("dds");
		encoders.push_back("jpg");
		encoders.push_back("jpe");
		encoders.push_back("jpeg");
		encoders.push_back("pcx");
		encoders.push_back("png");
		encoders.push_back("raw");
		encoders.push_back("tga");
		encoders.push_back("tif");
		encoders.push_back("tiff");
	}

	return encoders;
}

bool OILImageSequenceRecorder::setMode(const RecorderMode mode)
{
	const ScopedLock scopedLock(recorderLock);

	if (isRecording_)
		return false;

	return ImageSequenceRecorder::setMode(mode);
}

bool OILImageSequenceRecorder::setStartIndex(const unsigned int index)
{
	const ScopedLock scopedLock(recorderLock);

	if (isRecording_)
	{
		return false;
	}

	return ImageSequenceRecorder::setStartIndex(index);
}

bool OILImageSequenceRecorder::addImage(const Frame& frame)
{
	if (!frame.isValid())
	{
		return false;
	}

	if (!isRecording())
	{
		return false;
	}

	const FrameRef frameRef(new Frame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA));

	const ScopedLock scopedLock(recorderLock);

	if (recorderMode == RM_IMMEDIATE)
	{
		imageRecorder_.saveImage(*frameRef, addOptionalSuffixToFilename(recorderFilename, frameCounter_ + recorderStartIndex, recorderFilenameSuffixed));
	}
	else
	{
		const ScopedLock scopedLockQueue(frameQueueLock_);
		frameQueue_.push(std::make_pair(frameRef, frameCounter_));
	}

	frameCounter_++;

	return true;
}

bool OILImageSequenceRecorder::start()
{
	const ScopedLock scopedLock(recorderLock);

	if (isRecording_)
		return true;

	if (recorderFilename.empty())
		return false;

	frameCounter_ = 0u;
	while (!frameQueue_.empty())
	{
		frameQueue_.pop();
	}

	startTimestamp_.toNow();

	if (recorderMode == RM_PARALLEL && !isThreadInvokedToStart())
	{
		startThread();
	}

	isRecording_ = true;
	return true;
}

bool OILImageSequenceRecorder::stop()
{
	const ScopedLock scopedLock(recorderLock);

	if (!isRecording_)
		return true;

	isRecording_ = false;
	return true;
}

bool OILImageSequenceRecorder::isRecording() const
{
	return isRecording_;
}

bool OILImageSequenceRecorder::forceSaving()
{
	const ScopedLock scopedLock(recorderLock);

	if (isRecording_ || recorderMode != RM_EXPLICIT)
	{
		return false;
	}

	ocean_assert(!isThreadActive() || !isThreadInvokedToStart());

	const ScopedLock scopedLockQueue(frameQueueLock_);

	bool result = true;

	while (!frameQueue_.empty())
	{
		const FrameRef frame = frameQueue_.front().first;
		const unsigned int frameIndex = frameQueue_.front().second;
		frameQueue_.pop();

		result = imageRecorder_.saveImage(*frame, addOptionalSuffixToFilename(recorderFilename, frameIndex + recorderStartIndex, recorderFilenameSuffixed)) && result;
	}

	return result;
}

bool OILImageSequenceRecorder::lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency)
{
	const ScopedLock scopedLock(recorderLock);

	if (!isRecording_)
	{
		return false;
	}

	if (!recorderFrameType.isValid() || recorderFrameFrequency <= 0.0f)
	{
		return false;
	}

	if (respectFrameFrequency)
	{
		ocean_assert(startTimestamp_.isValid());

		const double seconds = float(frameCounter_) / recorderFrameFrequency;

		const Timestamp next(startTimestamp_ + seconds);
		if (Timestamp(true) < next)
		{
			return false;
		}
	}

	ocean_assert(!frame_.isValid());
	if (frame_)
	{
		return false;
	}

	frame_ = Frame(recorderFrameType);
	ocean_assert(frame_);

	recorderFrame = Frame(frame_, Frame::ACM_USE_KEEP_LAYOUT);

	return true;
}

void OILImageSequenceRecorder::unlockBufferToFill()
{
	const ScopedLock scopedLock(recorderLock);

	ocean_assert(frame_);

	if (recorderMode == RM_IMMEDIATE)
	{
		imageRecorder_.saveImage(frame_, addOptionalSuffixToFilename(recorderFilename, frameCounter_ + recorderStartIndex, recorderFilenameSuffixed));
	}
	else
	{
		const ScopedLock scopedLockQueue(frameQueueLock_);
		frameQueue_.push(std::make_pair(FrameRef(new Frame(std::move(frame_))), frameCounter_));
	}

	frameCounter_++;

	frame_.release();
}

void OILImageSequenceRecorder::threadRun()
{
	while (!shouldThreadStop())
	{
		FrameRef frame;
		unsigned int frameIndex = (unsigned int)(-1);

		TemporaryScopedLock scopedLock(frameQueueLock_);

		if (!frameQueue_.empty())
		{
			frame = frameQueue_.front().first;
			frameIndex = frameQueue_.front().second;
			frameQueue_.pop();
		}

		scopedLock.release();

		if (frame)
		{
			ocean_assert(frameIndex != (unsigned int)(-1));
			imageRecorder_.saveImage(*frame, addOptionalSuffixToFilename(recorderFilename, frameIndex + recorderStartIndex, recorderFilenameSuffixed));
		}
		else
		{
			sleep(1);
		}
	}
}

}

}

}
