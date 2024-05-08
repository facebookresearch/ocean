/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/shrinker/win/ShrinkerMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

ShrinkerMainWindow::ShrinkerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	frameWindow2_(instance, L"Frame size 1/2"),
	frameWindow4_(instance, L"Frame size 1/4"),
	frameWindow_8(instance, L"Frame size 1/8"),
	mediaFile_(file)
{
	// nothing to do here
}

ShrinkerMainWindow::~ShrinkerMainWindow()
{
	// nothing to do here
}

void ShrinkerMainWindow::onInitialized()
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

	frameWindow2_.setParent(handle());
	frameWindow2_.initialize();
	frameWindow2_.show();

	frameWindow4_.setParent(handle());
	frameWindow4_.initialize();
	frameWindow4_.show();

	frameWindow_8.setParent(handle());
	frameWindow_8.initialize();
	frameWindow_8.show();
}

void ShrinkerMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && frame->timestamp() != frameTimestamp_)
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void ShrinkerMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	static HighPerformanceStatistic performance;

	performance.start();

	Frame frame_2;
	if (CV::FrameShrinker::downsampleByTwo11(frame, frame_2, WorkerPool::get().scopedWorker()()))
	{
		frameWindow2_.setFrame(frame_2);
	}

	Frame frame_4;
	if (CV::FrameShrinker::downsampleByTwo11(frame_2, frame_4, WorkerPool::get().scopedWorker()()))
	{
		frameWindow4_.setFrame(frame_4);
	}

	Frame frame_8;
	if (CV::FrameShrinker::downsampleByTwo11(frame_4, frame_8, WorkerPool::get().scopedWorker()()))
	{
		frameWindow_8.setFrame(frame_8);
	}

	performance.stop();

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Entire time: ") + String::toAString(performance.averageMseconds()) + std::string("ms"));

	if (performance.measurements() > 500)
	{
		performance.reset();
	}

	repaint();

	frameWindow2_.repaint();
	frameWindow4_.repaint();
	frameWindow_8.repaint();
}
