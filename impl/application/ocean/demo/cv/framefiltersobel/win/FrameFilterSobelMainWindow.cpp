/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/framefiltersobel/win/FrameFilterSobelMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterMean.h"
#include "ocean/cv/FrameFilterSobel.h"
#include "ocean/cv/FrameNormalizer.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

FrameFilterSobelMainWindow::FrameFilterSobelMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	frameWindow0_(instance, L"0 degree filter"),
	frameWindow90_(instance, L"90 degree filter"),
	frameWindow45_(instance, L"45 degree filter"),
	frameWindow135_(instance, L"135 degree filter"),
	frameWindowMagnitude_(instance, L"Magnitude degree filter (standard | normalized)"),
	frameWindowMagnitudeAverage_(instance, L"Average magnitude degree filter (standard | normalized)"),
	mediaFile_(file)
{
	// nothing to do here
}

FrameFilterSobelMainWindow::~FrameFilterSobelMainWindow()
{
	// nothing to do here
}

void FrameFilterSobelMainWindow::onInitialized()
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

	frameWindow0_.setParent(handle());
	frameWindow0_.initialize();
	frameWindow0_.show();

	frameWindow90_.setParent(handle());
	frameWindow90_.initialize();
	frameWindow90_.show();

	frameWindow45_.setParent(handle());
	frameWindow45_.initialize();
	frameWindow45_.show();

	frameWindow135_.setParent(handle());
	frameWindow135_.initialize();
	frameWindow135_.show();

	frameWindowMagnitude_.setParent(handle());
	frameWindowMagnitude_.initialize();
	frameWindowMagnitude_.show();

	frameWindowMagnitudeAverage_.setParent(handle());
	frameWindowMagnitudeAverage_.initialize();
	frameWindowMagnitudeAverage_.show();
}

void FrameFilterSobelMainWindow::onIdle()
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

void FrameFilterSobelMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void FrameFilterSobelMainWindow::onFrame(const Frame& frame)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame targetFrame(FrameType(yFrame, FrameType::genericPixelFormat<uint8_t, 4u>()));

	HighPerformanceTimer timer;
	CV::FrameFilterSobel::filter8BitPerChannel<int8_t, 1u>(yFrame.constdata<uint8_t>(), targetFrame.data<int8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), targetFrame.paddingElements(), &worker_);
	const double time = timer.mseconds();

	Frame absoluteResponseFrame(yFrame.frameType());

	for (unsigned int y = 0u; y < targetFrame.height(); ++y)
	{
		const int8_t* targetRow = targetFrame.constrow<int8_t>(y);
		uint8_t* targetRowUnsigned = targetFrame.row<uint8_t>(y);

		uint8_t* const absoluteResponseRow = absoluteResponseFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < targetFrame.width(); ++x)
		{
			absoluteResponseRow[x] = uint8_t(std::max(abs(targetRow[x * 4u + 0u]), std::max(abs(targetRow[x * 4u + 1u]), std::max(abs(targetRow[x * 4u + 2u]), abs(targetRow[x * 4u + 3u])))));

			targetRowUnsigned[x * 4u + 0u] += uint8_t(127);
			targetRowUnsigned[x * 4u + 1u] += uint8_t(127);
			targetRowUnsigned[x * 4u + 2u] += uint8_t(127);
			targetRowUnsigned[x * 4u + 3u] += uint8_t(127);
		}
	}

	Frame absoluteResponseFrameAverage(absoluteResponseFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	CV::FrameFilterMean::filter(absoluteResponseFrameAverage, 11u);

	Frame absoluteResponseFrameNormalized(absoluteResponseFrame.frameType());
	Frame absoluteResponseFrameAverageNormalized(absoluteResponseFrameAverage.frameType());

	CV::FrameNormalizer::normalizeToUint8(absoluteResponseFrame, absoluteResponseFrameNormalized);
	CV::FrameNormalizer::normalizeToUint8(absoluteResponseFrameAverage, absoluteResponseFrameAverageNormalized);

	absoluteResponseFrame.copy(absoluteResponseFrame.width() / 2u, 0u, absoluteResponseFrameNormalized.subFrame(absoluteResponseFrame.width() / 2u, 0u, absoluteResponseFrame.width() / 2u, absoluteResponseFrame.height()));
	absoluteResponseFrameAverage.copy(absoluteResponseFrameAverage.width() / 2u, 0u, absoluteResponseFrameAverageNormalized.subFrame(absoluteResponseFrameAverage.width() / 2u, 0u, absoluteResponseFrameAverage.width() / 2u, absoluteResponseFrameAverage.height()));

	Frames yFrameAngles;
	CV::FrameChannels::Comfort::separateTo1Channel(targetFrame, yFrameAngles, FrameType::FORMAT_Y8);

	setFrame(yFrame);

	if (yFrameAngles.size() == 4)
	{
		frameWindow0_.setFrame(yFrameAngles[0]);
		frameWindow90_.setFrame(yFrameAngles[1]);
		frameWindow45_.setFrame(yFrameAngles[2]);
		frameWindow135_.setFrame(yFrameAngles[3]);
	}

	frameWindowMagnitude_.setFrame(absoluteResponseFrame);
	frameWindowMagnitudeAverage_.setFrame(absoluteResponseFrameAverage);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(time) + std::string("ms"));

	repaint();

	frameWindow0_.repaint();
	frameWindow90_.repaint();
	frameWindow45_.repaint();
	frameWindow135_.repaint();

	frameWindowMagnitude_.repaint();
	frameWindowMagnitudeAverage_.repaint();
}
