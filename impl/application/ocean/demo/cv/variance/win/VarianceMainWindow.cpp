/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/variance/win/VarianceMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterLaplace.h"
#include "ocean/cv/FrameNormalizer.h"
#include "ocean/cv/FrameVariance.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

VarianceMainWindow::VarianceMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	laplaceWindow_(instance, L"Laplace filter"),
	laplaceDeviationWindow_(instance, L"Laplace deviation"),
	deviationWindowY_(instance, L"Deviation Y"),
	deviationWindowU_(instance, L"Deviation U"),
	deviationWindowV_(instance, L"Deviation V"),
	mediaFile_(file)
{
	// nothing to do here
}

VarianceMainWindow::~VarianceMainWindow()
{
	// nothing to do here
}

void VarianceMainWindow::onInitialized()
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

	laplaceWindow_.setParent(handle());
	laplaceWindow_.initialize();
	laplaceWindow_.show();

	laplaceDeviationWindow_.setParent(handle());
	laplaceDeviationWindow_.initialize();
	laplaceDeviationWindow_.show();

	deviationWindowY_.setParent(handle());
	deviationWindowY_.initialize();
	deviationWindowY_.show();

	deviationWindowU_.setParent(handle());
	deviationWindowU_.initialize();
	deviationWindowU_.show();

	deviationWindowV_.setParent(handle());
	deviationWindowV_.initialize();
	deviationWindowV_.show();
}

void VarianceMainWindow::onIdle()
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

void VarianceMainWindow::onFrame(const Frame& frame)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame yuvFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_YUV24, yuvFrame,  CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frames separatedFrames;
	if (!CV::FrameChannels::Comfort::separateTo1Channel(frame, separatedFrames, FrameType::FORMAT_Y8) || separatedFrames.size() != 3)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	const Frame& frameY = separatedFrames[0];
	const Frame& frameU = separatedFrames[1];
	const Frame& frameV = separatedFrames[2];

	Frame deviationY(FrameType(yuvFrame, FrameType::FORMAT_Y8));
	Frame deviationU(FrameType(yuvFrame, FrameType::FORMAT_Y8));
	Frame deviationV(FrameType(yuvFrame, FrameType::FORMAT_Y8));
	CV::FrameVariance::deviation1Channel8Bit(frameY.constdata<uint8_t>(), deviationY.data<uint8_t>(), frameY.width(), frameY.height(), frameY.paddingElements(), deviationY.paddingElements(), 15u);
	CV::FrameVariance::deviation1Channel8Bit(frameU.constdata<uint8_t>(), deviationU.data<uint8_t>(), frameU.width(), frameU.height(), frameU.paddingElements(), deviationU.paddingElements(), 15u);
	CV::FrameVariance::deviation1Channel8Bit(frameV.constdata<uint8_t>(), deviationV.data<uint8_t>(), frameV.width(), frameV.height(), frameV.paddingElements(), deviationV.paddingElements(), 15u);

	CV::FrameNormalizer::normalizeToUint8(deviationY);
	CV::FrameNormalizer::normalizeToUint8(deviationU);
	CV::FrameNormalizer::normalizeToUint8(deviationV);

	Frame laplaceFrame(yFrame.frameType());

	HighPerformanceTimer timer;
	CV::FrameFilterLaplace::filterNormalizedMagnitude1Channel8Bit(yFrame.constdata<uint8_t>(), laplaceFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), laplaceFrame.paddingElements(), WorkerPool::get().scopedWorker()());
	const double timeLaplace = timer.mseconds();

	Frame deviationFrame(yFrame.frameType());
	timer.start();
	CV::FrameVariance::deviation1Channel8Bit(laplaceFrame.constdata<uint8_t>(), deviationFrame.data<uint8_t>(), laplaceFrame.width(), laplaceFrame.height(), laplaceFrame.paddingElements(), deviationFrame.paddingElements(), 7u);
	const double timeDeviation = timer.mseconds();

	for (unsigned int y = 0u; y < deviationFrame.height(); ++y)
	{
		uint8_t* const deviationRow = deviationFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < deviationFrame.width(); ++x)
		{
			deviationRow[x] = uint8_t(std::min(deviationRow[x] * 2u, 0xFFu));
		}
	}

	laplaceWindow_.setFrame(laplaceFrame);
	laplaceDeviationWindow_.setFrame(deviationFrame);

	deviationWindowY_.setFrame(deviationY);
	deviationWindowU_.setFrame(deviationU);
	deviationWindowV_.setFrame(deviationV);

	Platform::Win::Utilities::textOutput(laplaceWindow_.bitmap().dc(), 5, 5, String::toAString(timeLaplace) + std::string("ms"));
	Platform::Win::Utilities::textOutput(laplaceDeviationWindow_.bitmap().dc(), 5, 5, String::toAString(timeDeviation) + std::string("ms"));

	setFrame(frame);

	repaint();
	laplaceWindow_.repaint();
	laplaceDeviationWindow_.repaint();

	deviationWindowY_.repaint();
	deviationWindowU_.repaint();
	deviationWindowV_.repaint();
}
