/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/binarizationotsu/win/BinarizationOtsuMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverterThreshold.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

BinarizationOtsuMainWindow::BinarizationOtsuMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	binaryWindow_(instance, L"Otsu binarization"),
	mediaFile_(file)
{
	// nothing to do here
}

BinarizationOtsuMainWindow::~BinarizationOtsuMainWindow()
{
	// nothing to do here
}

void BinarizationOtsuMainWindow::onInitialized()
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

	binaryWindow_.setParent(handle());
	binaryWindow_.initialize();
	binaryWindow_.show();
}

void BinarizationOtsuMainWindow::onIdle()
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

void BinarizationOtsuMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	HighPerformanceTimer timer;
	{
		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, &worker_))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const uint8_t threshold = CV::FrameConverterThreshold::calculateOtsuThreshold(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), &worker_);
		CV::FrameConverterThreshold::convertY8ToB8(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), threshold, &worker_);

		binaryWindow_.setFrame(yFrame);

		Platform::Win::Utilities::textOutput(binaryWindow_.bitmap().dc(), 5, 5, String::toAString(timer.mseconds()) + std::string("ms"));
		Platform::Win::Utilities::textOutput(binaryWindow_.bitmap().dc(), 5, 25, "Threshold: " + std::to_string((int) threshold));
	}

	repaint();
	binaryWindow_.repaint();
}
