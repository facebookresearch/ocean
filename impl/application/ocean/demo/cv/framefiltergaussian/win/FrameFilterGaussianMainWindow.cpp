/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/framefiltergaussian/win/FrameFilterGaussianMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

FrameFilterGaussianMainWindow::FrameFilterGaussianMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	filterWindow3_(instance, L"Filter 3x3"),
	filterWindow7_(instance, L"Filter 7x7"),
	filterWindow15_(instance, L"Filter 15x15"),
	filterWindow31_(instance, L"Filter 31x31"),
	mediaFile_(file)
{
	// nothing to do here
}

FrameFilterGaussianMainWindow::~FrameFilterGaussianMainWindow()
{
	// nothing to do here
}

void FrameFilterGaussianMainWindow::onInitialized()
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
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
		frameMedium_->start();
	}

	filterWindow3_.setParent(handle());
	filterWindow3_.initialize();
	filterWindow3_.show();

	filterWindow7_.setParent(handle());
	filterWindow7_.initialize();
	filterWindow7_.show();

	filterWindow15_.setParent(handle());
	filterWindow15_.initialize();
	filterWindow15_.show();

	filterWindow31_.setParent(handle());
	filterWindow31_.initialize();
	filterWindow31_.show();
}

void FrameFilterGaussianMainWindow::onIdle()
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

void FrameFilterGaussianMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void FrameFilterGaussianMainWindow::onFrame(const Frame& frame)
{
	HighPerformanceTimer timer;

	Frame frame3;
	CV::FrameFilterGaussian::filter(frame, frame3, 3u, WorkerPool::get().scopedWorker()());
	const double time3 = timer.mseconds();

	timer.start();

	Frame frame7;
	CV::FrameFilterGaussian::filter(frame, frame7, 7u, WorkerPool::get().scopedWorker()());
	const double time7 = timer.mseconds();

	timer.start();

	Frame frame15;
	CV::FrameFilterGaussian::filter(frame, frame15, 15u, WorkerPool::get().scopedWorker()());
	const double time15 = timer.mseconds();

	timer.start();

	Frame frame31;
	CV::FrameFilterGaussian::filter(frame, frame31, 31u, WorkerPool::get().scopedWorker()());
	const double time31 = timer.mseconds();

	setFrame(frame);

	filterWindow3_.setFrame(frame3);
	filterWindow7_.setFrame(frame7);
	filterWindow15_.setFrame(frame15);
	filterWindow31_.setFrame(frame31);

	Platform::Win::Utilities::textOutput(filterWindow3_.bitmap().dc(), 5, 5, String::toAString(time3, 3u) + "ms");
	Platform::Win::Utilities::textOutput(filterWindow7_.bitmap().dc(), 5, 5, String::toAString(time7, 3u) + "ms");
	Platform::Win::Utilities::textOutput(filterWindow15_.bitmap().dc(), 5, 5, String::toAString(time15, 3u) + "ms");
	Platform::Win::Utilities::textOutput(filterWindow31_.bitmap().dc(), 5, 5, String::toAString(time31, 3u) + "ms");

	repaint();
	filterWindow3_.repaint();
	filterWindow7_.repaint();
	filterWindow15_.repaint();
	filterWindow31_.repaint();
}
