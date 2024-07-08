/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/framefilterscharr/win/FrameFilterScharrMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterMean.h"
#include "ocean/cv/FrameFilterScharr.h"
#include "ocean/cv/FrameNormalizer.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

FrameFilterScharrMainWindow::FrameFilterScharrMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	frameWindow0_(instance, L"0 degree filter"),
	frameWindow90_(instance, L"90 degree filter"),
	frameWindow45_(instance, L"45 degree filter"),
	frameWindow135_(instance, L"135 degree filter"),
	frameWindowMagnitude_(instance, L"Magnitude degree filter (standard | normalized)"),
	frameWindowMagnitudeAverage_(instance, L"Average magnitude degree filter (standard | normalized)")
{
	if (!file.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(file);
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
}

FrameFilterScharrMainWindow::~FrameFilterScharrMainWindow()
{
	// nothing to do here
}

void FrameFilterScharrMainWindow::onInitialized()
{
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

void FrameFilterScharrMainWindow::onIdle()
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

void FrameFilterScharrMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void FrameFilterScharrMainWindow::onFrame(const Frame& frame)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	Frame responseFrame(FrameType(yFrame, FrameType::genericPixelFormat<int8_t, 4u>()));

	HighPerformanceTimer timer;
	CV::FrameFilterScharr::filter8BitPerChannel<int8_t, 1u>(yFrame.data<uint8_t>(), responseFrame.data<int8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), responseFrame.paddingElements(), WorkerPool::get().scopedWorker()());
	const double time = timer.mseconds();

	Frame absoluteResponseFrame(yFrame.frameType());

	for (unsigned int y = 0u; y < yFrame.height(); ++y)
	{
		const int8_t* responseRow_int8 = responseFrame.constrow<int8_t>(y);
		uint8_t* responseRow_uint8 = responseFrame.row<uint8_t>(y);

		uint8_t* absoluteResponse = absoluteResponseFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < yFrame.width(); ++x)
		{
			*absoluteResponse = uint8_t(std::max(std::abs(responseRow_int8[0]), std::max(std::abs(responseRow_int8[1]), std::max(std::abs(responseRow_int8[2]), std::abs(responseRow_int8[3])))));

			responseRow_uint8[0] = responseRow_int8[0] + 127;
			responseRow_uint8[1] = responseRow_int8[1] + 127;
			responseRow_uint8[2] = responseRow_int8[2] + 127;
			responseRow_uint8[3] = responseRow_int8[3] + 127;

			responseRow_int8 += 4;
			responseRow_uint8 += 4;
			++absoluteResponse;
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

	responseFrame.setPixelFormat(FrameType::genericPixelFormat<uint8_t, 4u>());

	Frames yResponseFrames;
	CV::FrameChannels::Comfort::separateTo1Channel(responseFrame, yResponseFrames, FrameType::FORMAT_Y8);

	setFrame(yFrame);
	repaint();

	if (yResponseFrames.size() == 4)
	{
		frameWindow0_.setFrame(yResponseFrames[0]);
		frameWindow90_.setFrame(yResponseFrames[1]);
		frameWindow45_.setFrame(yResponseFrames[2]);
		frameWindow135_.setFrame(yResponseFrames[3]);

		frameWindow0_.repaint();
		frameWindow90_.repaint();
		frameWindow45_.repaint();
		frameWindow135_.repaint();
	}

	frameWindowMagnitude_.setFrame(absoluteResponseFrame);
	frameWindowMagnitudeAverage_.setFrame(absoluteResponseFrameAverage);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(time) + std::string("ms"));

	frameWindowMagnitude_.repaint();
	frameWindowMagnitudeAverage_.repaint();
}
