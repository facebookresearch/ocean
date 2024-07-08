/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/harriscornervotes/win/HarrisCornerVotesMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/FrameConverterColorMap.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

HarrisCornerVotesMainWindow::HarrisCornerVotesMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

HarrisCornerVotesMainWindow::~HarrisCornerVotesMainWindow()
{
	// nothing to do here
}

void HarrisCornerVotesMainWindow::onInitialized()
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
		frameMedium_->start();
	}
}

void HarrisCornerVotesMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void HarrisCornerVotesMainWindow::onKeyDown(const int /*key*/)
{
	++colorMapIndex_;

	skipNegativeResponses_ = (colorMapIndex_ % (CV::FrameConverterColorMap::definedColorMaps().size() * 2)) >= CV::FrameConverterColorMap::definedColorMaps().size();
}

void HarrisCornerVotesMainWindow::onFrame(const Frame& frame)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		return;
	}

	HighPerformanceTimer timer;

	Frame voteFrame(FrameType(yFrame, FrameType::genericPixelFormat<int32_t, 1u>()));
	CV::Detector::HarrisCornerDetector::harrisVotesFrame(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), voteFrame.data<int32_t>(), voteFrame.paddingElements(), WorkerPool::get().scopedWorker()(), true /*setBorderPixels*/);

	const double msTime = timer.mseconds();

	if (skipNegativeResponses_)
	{
		for (unsigned int y = 0u; y < voteFrame.height(); ++y)
		{
			int32_t* const row = voteFrame.row<int32_t>(y);

			for (unsigned int x = 0u; x < voteFrame.width(); ++x)
			{
				if (row[x] < 0)
				{
					row[x] = 0;
				}
			}
		}
	}

	const CV::FrameConverterColorMap::ColorMaps& definedColorMaps = CV::FrameConverterColorMap::definedColorMaps();

	const CV::FrameConverterColorMap::ColorMap colorMap = definedColorMaps[colorMapIndex_ % definedColorMaps.size()];

	Frame colorMapFrame;
	if (CV::FrameConverterColorMap::Comfort::convertInteger1ChannelToRGB24(voteFrame, colorMapFrame, colorMap, WorkerPool::get().scopedWorker()()))
	{
		CV::Canvas::drawText(colorMapFrame, String::toAString(msTime, 2u) + "ms, " + CV::FrameConverterColorMap::translateColorMap(colorMap), 5, 5, CV::Canvas::black(), CV::Canvas::white());

		setFrame(colorMapFrame);
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	repaint(false);
}
