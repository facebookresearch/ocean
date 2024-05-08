/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/framefilterpatch/win/FrameFilterPatchMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterPatch.h"
#include "ocean/cv/NonMaximumSuppression.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

FrameFilterPatchMainWindow::FrameFilterPatchMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	filterWindow_(instance, L"Filter result"),
	mediaFile_(file)
{
	// nothing to do here
}

FrameFilterPatchMainWindow::~FrameFilterPatchMainWindow()
{
	// nothing to do here
}

void FrameFilterPatchMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}

	filterWindow_.setParent(handle());
	filterWindow_.initialize();
	filterWindow_.show();
}

void FrameFilterPatchMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_ || ignoreTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void FrameFilterPatchMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void FrameFilterPatchMainWindow::onFrame(const Frame& frame)
{
	Frame yuvFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_YUV24, yuvFrame, CV::FrameChannels::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	Frames frames;
	if (!CV::FrameChannels::Comfort::separateTo1Channel(yuvFrame, frames, FrameType::FORMAT_Y8) || frames.size() != 3)
	{
		return;
	}

	Frame patchFrame0(frames[0].frameType());
	Frame patchFrame1(frames[1].frameType());
	Frame patchFrame2(frames[2].frameType());

	const HighPerformanceTimer timer;

	CV::FrameFilterPatch::filter1Channel8Bit(frames[0].constdata<uint8_t>(), patchFrame0.data<uint8_t>(), frames[0].width(), frames[0].height(), 25u, frames[0].paddingElements(), patchFrame0.paddingElements(), WorkerPool::get().scopedWorker()());
	CV::FrameFilterPatch::filter1Channel8Bit(frames[2].constdata<uint8_t>(), patchFrame1.data<uint8_t>(), frames[1].width(), frames[1].height(), 25u, frames[1].paddingElements(), patchFrame1.paddingElements(), WorkerPool::get().scopedWorker()());
	CV::FrameFilterPatch::filter1Channel8Bit(frames[1].constdata<uint8_t>(), patchFrame2.data<uint8_t>(), frames[2].width(), frames[2].height(), 25u, frames[2].paddingElements(), patchFrame2.paddingElements(), WorkerPool::get().scopedWorker()());

	const double time = timer.mseconds();

	Frame patchFrame(frames[0].frameType());

	for (unsigned int y = 0u; y < patchFrame.height(); ++y)
	{
		const uint8_t* const row0 = patchFrame0.constrow<uint8_t>(y);
		const uint8_t* const row1 = patchFrame1.constrow<uint8_t>(y);
		const uint8_t* const row2 = patchFrame2.constrow<uint8_t>(y);

		uint8_t* const row = patchFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < patchFrame.width(); ++x)
		{
			const uint8_t maxValue = max(row0[x], max(row1[x], row2[x]));

			if (maxValue > 20u)
			{
				row[x] = maxValue;
			}
			else
			{
				row[x] = 0;
			}
		}
	}

	setFrame(frame);
	filterWindow_.setFrame(patchFrame);

	Platform::Win::Utilities::textOutput(filterWindow_.bitmap().dc(), 5, 5, std::string("Performance: ") + String::toAString(time));

	repaint();
	filterWindow_.repaint();
}
