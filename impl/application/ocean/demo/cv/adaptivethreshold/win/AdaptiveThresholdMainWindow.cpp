/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/adaptivethreshold/win/AdaptiveThresholdMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterThreshold.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

AdaptiveThresholdMainWindow::AdaptiveThresholdMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	thresholdWindow_(instance, L"Adaptive threshold (non mirrored)"),
	thresholdWindowMirrored_(instance, L"Adaptive threshold (mirrored border)"),
	mediaFile_(file)
{
	// nothing to do here
}

AdaptiveThresholdMainWindow::~AdaptiveThresholdMainWindow()
{
	// nothing to do here
}

void AdaptiveThresholdMainWindow::onInitialized()
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

	thresholdWindow_.setParent(handle());
	thresholdWindow_.initialize();
	thresholdWindow_.show();

	thresholdWindowMirrored_.setParent(handle());
	thresholdWindowMirrored_.initialize();
	thresholdWindowMirrored_.show();
}

void AdaptiveThresholdMainWindow::onIdle()
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

void AdaptiveThresholdMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	HighPerformanceTimer timer;

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		ocean_assert(false && "Unsupported pixel format!");
		return;
	}

	constexpr unsigned int border = 80u;
	constexpr unsigned int kernel = 2u * border + 1u;

	{
		const Frame integralFrame = CV::IntegralImage::Comfort::createBorderedImage(yFrame, border);

		Frame target(yFrame.frameType());
		ocean_assert(yFrame.isContinuous() && integralFrame.isContinuous() && target.isContinuous());
		CV::FrameConverterThreshold::convertBorderedY8ToB8(yFrame.constdata<uint8_t>(), integralFrame.constdata<uint32_t>(), target.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements(), target.paddingElements(), kernel, 0.85f, &worker_);

		thresholdWindow_.setFrame(target);
		Platform::Win::Utilities::textOutput(thresholdWindow_.bitmap().dc(), 5, 5, String::toAString(timer.mseconds()) + std::string("ms"));

		repaint();
		thresholdWindow_.repaint();
	}

	{
		Frame integralFrame(FrameType(yFrame.width() + kernel, yFrame.height() + kernel, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));

		CV::IntegralImage::createBorderedImageMirror<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), border, yFrame.paddingElements(), integralFrame.paddingElements());

		Frame target(yFrame.frameType());
		CV::FrameConverterThreshold::convertBorderedY8ToB8(yFrame.constdata<uint8_t>(), integralFrame.constdata<uint32_t>(), target.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements(), target.paddingElements(), kernel, 0.85f, &worker_);

		thresholdWindowMirrored_.setFrame(target);
		Platform::Win::Utilities::textOutput(thresholdWindowMirrored_.bitmap().dc(), 5, 5, String::toAString(timer.mseconds()) + std::string("ms"));
		thresholdWindowMirrored_.repaint();
	}
}
