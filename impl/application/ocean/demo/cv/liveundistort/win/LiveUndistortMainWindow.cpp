/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/liveundistort/win/LiveUndistortMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

LiveUndistortMainWindow::LiveUndistortMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

LiveUndistortMainWindow::~LiveUndistortMainWindow()
{
	// nothing to do here
}

void LiveUndistortMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_)
	{
		frameMedium_->start();

		const Timestamp startTimestamp(true);

		while (frameMedium_->frame().isNull() && startTimestamp + 5 > Timestamp(true))
		{
			Thread::sleep(1);
		}

		const FrameRef frame = frameMedium_->frame();

		if (frame)
		{
			camera_ = IO::CameraCalibrationManager::get().camera(frameMedium_->url(), frame->width(), frame->height());
		}
	}
}

void LiveUndistortMainWindow::onIdle()
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

void LiveUndistortMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void LiveUndistortMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	distortFrame_ = !distortFrame_;
}

void LiveUndistortMainWindow::onFrame(const Frame& frame)
{
	HighPerformanceTimer timer;
	Frame rgbFrame;

	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "Invalid frame pixel format.");

		setFrame(frame);
		repaint();
		return;
	}

	if (distortFrame_ && camera_)
	{
		const PinholeCamera perfectCamera(camera_.intrinsic(), camera_.width(), camera_.height());

		Frame undistortedFrame;
		if (CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(rgbFrame, AnyCameraPinhole(camera_), SquareMatrix3(true), AnyCameraPinhole(perfectCamera), undistortedFrame, nullptr, WorkerPool::get().scopedWorker()()))
		{
			setFrame(undistortedFrame);
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}
	else
	{
		setFrame(rgbFrame);
	}

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Time: ") + String::toAString(timer.mseconds()));

	repaint();
}
