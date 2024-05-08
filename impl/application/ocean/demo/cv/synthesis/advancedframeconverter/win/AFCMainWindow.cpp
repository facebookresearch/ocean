/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/synthesis/advancedframeconverter/win/AFCMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/advanced/AdvancedFrameConverter.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

AFCMainWindow::AFCMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	frameWindowY_(instance, L"Y frame"),
	frameWindowU_(instance, L"U frame"),
	frameWindowV_(instance, L"V frame"),
	frameWindowT_(instance, L"T frame"),
	mediaFile_(file)
{
	// nothing to do here
}

AFCMainWindow::~AFCMainWindow()
{
	// nothing to do here
}

void AFCMainWindow::onInitialized()
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

	frameWindowY_.setParent(handle());
	frameWindowY_.initialize();
	frameWindowY_.show();

	frameWindowU_.setParent(handle());
	frameWindowU_.initialize();
	frameWindowU_.show();

	frameWindowV_.setParent(handle());
	frameWindowV_.initialize();
	frameWindowV_.show();

	frameWindowT_.setParent(handle());
	frameWindowT_.initialize();
	frameWindowT_.show();
}

void AFCMainWindow::onIdle()
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

void AFCMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void AFCMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	Frame yuvtFrame;

	HighPerformanceTimer timer;
	if (!CV::Advanced::AdvancedFrameConverter::convertToYUVT32ScharrMagnitude(topLeft, yuvtFrame, 21u, &worker_))
	{
		return;
	}

	Frame yFrame(FrameType(topLeft, FrameType::FORMAT_Y8));
	Frame uFrame(yFrame.frameType());
	Frame vFrame(yFrame.frameType());
	Frame tFrame(yFrame.frameType());

	CV::FrameChannels::Comfort::separateTo1Channel(yuvtFrame, {&yFrame, &uFrame, &vFrame, &tFrame});

	setFrame(topLeft);
	frameWindowY_.setFrame(yFrame);
	frameWindowU_.setFrame(uFrame);
	frameWindowV_.setFrame(vFrame);
	frameWindowT_.setFrame(tFrame);

	repaint();
	frameWindowY_.repaint();
	frameWindowU_.repaint();
	frameWindowV_.repaint();
	frameWindowT_.repaint();
}
