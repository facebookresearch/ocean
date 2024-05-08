/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/frameconverteryuv/win/FrameConverterYUVMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

FrameConverterYUVMainWindow::FrameConverterYUVMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	windowY_(instance, L"Y frame"),
	windowU_(instance, L"U frame"),
	windowV_(instance, L"V frame"),
	mediaFile_(file),
	ignoreTimestamp_(false)
{
	// nothing to do here
}

FrameConverterYUVMainWindow::~FrameConverterYUVMainWindow()
{
	// nothing to do here
}

void FrameConverterYUVMainWindow::onInitialized()
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

	const Media::FiniteMediumRef finiteMedium(frameMedium_);
	if (finiteMedium)
	{
		finiteMedium->setLoop(true);
	}

	windowY_.setParent(handle());
	windowY_.initialize();
	windowY_.show();

	windowU_.setParent(handle());
	windowU_.initialize();
	windowU_.show();

	windowV_.setParent(handle());
	windowV_.initialize();
	windowV_.show();
}

void FrameConverterYUVMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && (frame->timestamp() != recentFrameTimestamp_ || ignoreTimestamp_))
		{
			onFrame(*frame);

			recentFrameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void FrameConverterYUVMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void FrameConverterYUVMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "Unsupported pixel format!");
		return;
	}

	const HighPerformanceTimer timer;

	Frame topLeftYUV;
	if (!CV::FrameConverter::Comfort::convert(topLeft, FrameType::FORMAT_YUV24, topLeftYUV, false, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "Unsupported pixel format!");
		return;
	}

	Frames y_u_vFrames;
	const bool result = CV::FrameChannels::Comfort::separateTo1Channel(topLeftYUV, y_u_vFrames, FrameType::FORMAT_Y8);

	const double time = timer.mseconds();

	setFrame(topLeft);

	if (result)
	{
		ocean_assert(y_u_vFrames.size() == 3);
		windowY_.setFrame(y_u_vFrames[0]);
		windowU_.setFrame(y_u_vFrames[1]);
		windowV_.setFrame(y_u_vFrames[2]);
	}

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(time) + std::string("ms"));

	repaint();
	windowY_.repaint();
	windowU_.repaint();
	windowV_.repaint();
}
