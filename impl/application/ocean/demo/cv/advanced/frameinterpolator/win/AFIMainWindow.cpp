/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/advanced/frameinterpolator/win/AFIMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInverter.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/PixelTriangle.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

AFIMainWindow::AFIMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	interpolationWindow_(instance, L"Interpolation")
{
	if (!file.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(file, Media::Medium::FRAME_MEDIUM);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);
	if (finiteMedium)
	{
		finiteMedium->setLoop(true);
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}
}

AFIMainWindow::~AFIMainWindow()
{
	// nothing to do here
}

void AFIMainWindow::onInitialized()
{
	interpolationWindow_.setParent(handle());
	interpolationWindow_.initialize();
	interpolationWindow_.show();
}

void AFIMainWindow::onIdle()
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

void AFIMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void AFIMainWindow::onFrame(const Frame& frame)
{
	HighPerformanceTimer timer;

	Frame frameTopLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, frameTopLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	CV::Advanced::PixelTriangle sourceTriangle(CV::PixelPosition(30u, 30u), CV::PixelPosition(500u, 200u), CV::PixelPosition(70u, 400u));
	CV::Advanced::PixelTriangle targetTriangle(CV::PixelPosition(500u, 20u), CV::PixelPosition(300u, 400u), CV::PixelPosition(50u, 20u));

	Frame targetFrame(frameTopLeft.frameType());
	targetFrame.setValue(0x00);

	timer.start();
	CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolateTriangle(frameTopLeft, targetFrame, sourceTriangle, targetTriangle, WorkerPool::get().scopedWorker()());
	const double time = timer.mseconds();

	MoveToEx(bitmap().dc(), int(sourceTriangle.point2().x()), int(sourceTriangle.point2().y()), nullptr);
	LineTo(bitmap().dc(), int(sourceTriangle.point0().x()), int(sourceTriangle.point0().y()));
	LineTo(bitmap().dc(), int(sourceTriangle.point1().x()), int(sourceTriangle.point1().y()));
	LineTo(bitmap().dc(), int(sourceTriangle.point2().x()), int(sourceTriangle.point2().y()));

	setFrame(frameTopLeft);
	Platform::Win::Utilities::textOutput(bitmap().dc(), int(sourceTriangle.point0().x()), int(sourceTriangle.point0().y()), "0");
	Platform::Win::Utilities::textOutput(bitmap().dc(), int(sourceTriangle.point1().x()), int(sourceTriangle.point1().y()), "1");
	Platform::Win::Utilities::textOutput(bitmap().dc(), int(sourceTriangle.point2().x()), int(sourceTriangle.point2().y()), "2");
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(time) + std::string("ms"));
	repaint();

	interpolationWindow_.setFrame(targetFrame);
	Platform::Win::Utilities::textOutput(interpolationWindow_.bitmap().dc(), int(targetTriangle.point0().x()), int(targetTriangle.point0().y()), "0");
	Platform::Win::Utilities::textOutput(interpolationWindow_.bitmap().dc(), int(targetTriangle.point1().x()), int(targetTriangle.point1().y()), "1");
	Platform::Win::Utilities::textOutput(interpolationWindow_.bitmap().dc(), int(targetTriangle.point2().x()), int(targetTriangle.point2().y()), "2");
	interpolationWindow_.repaint();
}
