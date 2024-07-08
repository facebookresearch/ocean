/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/frameinterpolatorbilinear/win/FIBMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

FIBMainWindow::FIBMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	interpolationWindow_(instance, L"Bilinear interpolation with pixel center at (0.0, 0.0)", CV::PC_TOP_LEFT),
	interpolationFullBorderWindow_(instance, L"Bilinear interpolation with pixel center at (0.5, 0.5)", CV::PC_CENTER),
	mediaFile_(file)
{
	// nothing to do here
}

FIBMainWindow::~FIBMainWindow()
{
	// nothing to do here
}

void FIBMainWindow::onInitialized()
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

	interpolationWindow_.setParent(handle());
	interpolationWindow_.initialize();
	interpolationWindow_.show();

	interpolationFullBorderWindow_.setParent(handle());
	interpolationFullBorderWindow_.initialize();
	interpolationFullBorderWindow_.show();
}

void FIBMainWindow::onIdle()
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

void FIBMainWindow::onFrame(const Frame& frame)
{
	HighPerformanceTimer timer;

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, false, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	setFrame(rgbFrame);

	interpolationWindow_.setFrame(rgbFrame);
	interpolationFullBorderWindow_.setFrame(rgbFrame);

	repaint();
	interpolationWindow_.repaint();
	interpolationFullBorderWindow_.repaint();
}
