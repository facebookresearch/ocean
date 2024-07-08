/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/masktracker/win/MaskTrackerMainWindow.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/MaskCreator.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/geometry/Homography.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/win/Utilities.h"

MaskTrackerMainWindow::MaskTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(media)
{
	// nothing to do here
}

MaskTrackerMainWindow::~MaskTrackerMainWindow()
{
	// nothing to do here
}

void MaskTrackerMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		if (frameMedium_.isNull())
		{
			frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
		}

		if (frameMedium_.isNull())
		{
			frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::MOVIE);
		}
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

void MaskTrackerMainWindow::onIdle()
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

void MaskTrackerMainWindow::onMouseDown(const MouseButton button, const int x, const int y)
{
	if (button & BUTTON_LEFT)
	{
		addPosition_ = CV::PixelPosition(x, y);
	}
	else if (button & BUTTON_RIGHT)
	{
		removePosition_ = CV::PixelPosition(x, y);
	}
}

void MaskTrackerMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void MaskTrackerMainWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	if (buttons & BUTTON_LEFT)
	{
		addPosition_ = CV::PixelPosition(x, y);
	}
	else if (buttons & BUTTON_RIGHT)
	{
		removePosition_ = CV::PixelPosition(x, y);
	}
}

void MaskTrackerMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_ALWAYS_COPY, &worker_))
	{
		return;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(topLeft, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	SquareMatrix3 homography;
	if (homographyTracker_.trackPoints(topLeft, yFrame, randomGenerator_, previousPositions_, homography, &worker_))
	{
		previousPositions_ = Tracking::HomographyTracker::transformPoints(previousPositions_, homography);
	}

	maskFrame_.set(yFrame.frameType(), true, true);
	maskFrame_.setValue(0xFF);

	CV::Segmentation::PixelContour contour(CV::PixelPosition::vectors2pixelPositions(previousPositions_, topLeft.width(), topLeft.height()));
	contour.makeDistinct();
	contour.makeDense();

	CV::Segmentation::MaskCreator::denseContour2inclusiveMask(maskFrame_.data<uint8_t>(), maskFrame_.width(), maskFrame_.height(), maskFrame_.paddingElements(), contour, 0x00);


	bool determineNewContour = false;

	if (addPosition_)
	{
		constexpr uint8_t value = 0x00;
		CV::Canvas::ellipse8BitPerChannel<1u>(maskFrame_.data<uint8_t>(), maskFrame_.width(), maskFrame_.height(), addPosition_, 51u, 51u, &value);
		addPosition_ = CV::PixelPosition();

		determineNewContour = true;
	}
	else if (removePosition_)
	{
		constexpr uint8_t value = 0xFF;
		CV::Canvas::ellipse8BitPerChannel<1u>(maskFrame_.data<uint8_t>(), maskFrame_.width(), maskFrame_.height(), removePosition_, 51u, 51u, &value);
		removePosition_ = CV::PixelPosition();

		determineNewContour = true;
	}

	if (determineNewContour)
	{
		CV::PixelPositions outlinePixels4;
		CV::Segmentation::MaskAnalyzer::findBorderPixels4(maskFrame_.constdata<uint8_t>(), maskFrame_.width(), maskFrame_.height(), maskFrame_.paddingElements(), outlinePixels4);

		CV::PixelPositions pixelContour;
		CV::Segmentation::MaskAnalyzer::pixels2contour(outlinePixels4, maskFrame_.width(), maskFrame_.height(), pixelContour);

		previousPositions_ = CV::PixelPosition::pixelPositions2vectors(pixelContour);
	}

	for (unsigned int y = 0u; y < topLeft.height(); ++y)
	{
		uint8_t* const topLeftRow = topLeft.row<uint8_t>(y);
		const uint8_t* const maskRow = maskFrame_.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < topLeft.width(); ++x)
		{
			if (maskRow[x] == 0x00)
			{
				topLeftRow[x * 3u + 0u] = 0xFF;
				topLeftRow[x * 3u + 1u] >>= 1u;
				topLeftRow[x * 3u + 2u] >>= 1u;
			}
		}
	}

	for (const CV::PixelPosition& contourPixel : contour.pixels())
	{
		uint8_t* const pixel = topLeft.pixel<uint8_t>(contourPixel.x(), contourPixel.y());

		pixel[0] = 0x00;
		pixel[1] = 0x00;
		pixel[2] = 0xFF;
	}

	setFrame(topLeft);

	repaint();
}
