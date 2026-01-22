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
#include "ocean/media/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

LiveUndistortMainWindow::LiveUndistortMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file, const std::string& resolution) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file),
	resolution_(resolution)
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
		if (!resolution_.empty())
		{
			unsigned int preferredWidth = 0u;
			unsigned int preferredHeight = 0u;

			if (Media::Utilities::parseResolution(resolution_, preferredWidth, preferredHeight))
			{
				frameMedium_->setPreferredFrameDimension(preferredWidth, preferredHeight);
			}
		}

		frameMedium_->start();

		const Timestamp startTimestamp(true);

		while (frameMedium_->frame().isNull() && startTimestamp + 5 > Timestamp(true))
		{
			Thread::sleep(1);
		}
	}
}

void LiveUndistortMainWindow::onIdle()
{
	if (frameMedium_)
	{
		SharedAnyCamera camera;
		const FrameRef frame(frameMedium_->frame(&camera));

		if (frame && *frame && (frame->timestamp() != frameTimestamp_ || ignoreTimestamp_))
		{
			if (camera)
			{
				onFrame(*frame, *camera);
			}
			else
			{
				Log::warning() << "Missing camera profile";
			}

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Thread::sleep(1u);
}

void LiveUndistortMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void LiveUndistortMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	undistortFrame_ = !undistortFrame_;
}

void LiveUndistortMainWindow::onFrame(const Frame& frame, const AnyCamera& camera)
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

	if (undistortFrame_)
	{
		const AnyCameraPinhole perfectCamera(PinholeCamera(camera.width(), camera.height(), camera.fovX()));

		Frame undistortedFrame;
		if (CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(rgbFrame, camera, SquareMatrix3(true), perfectCamera, undistortedFrame, nullptr, WorkerPool::get().scopedWorker()()))
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
