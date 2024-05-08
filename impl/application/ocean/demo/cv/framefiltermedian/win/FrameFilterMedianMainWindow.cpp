/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/framefiltermedian/win/FrameFilterMedianMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterMedian.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

FrameFilterMedianMainWindow::FrameFilterMedianMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	medianWindow_RGB24_Size3_(instance, L"RGB24, Size 3"),
	medianWindow_RGB24_Size21_(instance, L"RGB24, Size 21"),
	medianWindow_Y8_Size3_(instance, L"Y8, Size 3"),
	medianWindow_Y8_Size21_(instance, L"Y8, Size 21"),
	mediaFile_(file)
{
	// nothing to do here
}

FrameFilterMedianMainWindow::~FrameFilterMedianMainWindow()
{
	// nothing to do here
}

void FrameFilterMedianMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
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

	medianWindow_RGB24_Size3_.setParent(handle());
	medianWindow_RGB24_Size3_.initialize();
	medianWindow_RGB24_Size3_.show();

	medianWindow_RGB24_Size21_.setParent(handle());
	medianWindow_RGB24_Size21_.initialize();
	medianWindow_RGB24_Size21_.show();

	medianWindow_Y8_Size3_.setParent(handle());
	medianWindow_Y8_Size3_.initialize();
	medianWindow_Y8_Size3_.show();

	medianWindow_Y8_Size21_.setParent(handle());
	medianWindow_Y8_Size21_.initialize();
	medianWindow_Y8_Size21_.show();
}

void FrameFilterMedianMainWindow::onIdle()
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

void FrameFilterMedianMainWindow::onFrame(const Frame& frame)
{
	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	setFrame(rgbFrame);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	{
		static HighPerformanceStatistic performance;

		Frame medianFrame;

		performance.start();
			CV::FrameFilterMedian::Comfort::filter(rgbFrame, medianFrame, 3u, &worker_);
		performance.stop();

		medianWindow_RGB24_Size3_.setFrame(medianFrame);
		Platform::Win::Utilities::textOutput(medianWindow_RGB24_Size3_.bitmap().dc(), 5, 5, String::toAString(performance.averageMseconds()) + std::string("ms"));
	}

	{
		static HighPerformanceStatistic performance;

		Frame medianFrame;

		performance.start();
			CV::FrameFilterMedian::Comfort::filter(rgbFrame, medianFrame, 21u, &worker_);
		performance.stop();

		medianWindow_RGB24_Size21_.setFrame(medianFrame);
		Platform::Win::Utilities::textOutput(medianWindow_RGB24_Size21_.bitmap().dc(), 5, 5, String::toAString(performance.averageMseconds()) + std::string("ms"));
	}

	{
		static HighPerformanceStatistic performance;

		Frame medianFrame;

		performance.start();
			CV::FrameFilterMedian::Comfort::filter(yFrame, medianFrame, 3u, &worker_);
		performance.stop();

		medianWindow_Y8_Size3_.setFrame(medianFrame);
		Platform::Win::Utilities::textOutput(medianWindow_Y8_Size3_.bitmap().dc(), 5, 5, String::toAString(performance.averageMseconds()) + std::string("ms"));
	}

	{
		static HighPerformanceStatistic performance;

		Frame medianFrame;

		performance.start();
			CV::FrameFilterMedian::Comfort::filter(yFrame, medianFrame, 21u, &worker_);
		performance.stop();

		medianWindow_Y8_Size21_.setFrame(medianFrame);
		Platform::Win::Utilities::textOutput(medianWindow_Y8_Size21_.bitmap().dc(), 5, 5, String::toAString(performance.averageMseconds()) + std::string("ms"));
	}

	repaint();

	medianWindow_RGB24_Size3_.repaint();
	medianWindow_RGB24_Size21_.repaint();

	medianWindow_Y8_Size3_.repaint();
	medianWindow_Y8_Size21_.repaint();
}
