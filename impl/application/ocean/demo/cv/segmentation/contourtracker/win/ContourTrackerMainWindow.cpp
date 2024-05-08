/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/contourtracker/win/ContourTrackerMainWindow.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/media/Manager.h"

ContourTrackerMainWindow::ContourTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(media)
{
	// nothing to do here
}

ContourTrackerMainWindow::~ContourTrackerMainWindow()
{
	// nothing to do here
}

void ContourTrackerMainWindow::onInitialized()
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

void ContourTrackerMainWindow::onIdle()
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

void ContourTrackerMainWindow::onMouseDown(const MouseButton button, const int /*x*/, const int /*y*/)
{
	if (applicationState_ == AS_IDLE)
	{
		applicationState_ = AS_ROUGH_CONTOUR;
		pixelPositions_.clear();
	}

	if (applicationState_ == AS_TRACK_CONTOUR && button == BUTTON_RIGHT)
	{
		applicationState_ = AS_IDLE;
	}
}

void ContourTrackerMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	if (applicationState_ == AS_ROUGH_CONTOUR)
	{
		applicationState_ = AS_FINE_CONTOUR;
	}
}

void ContourTrackerMainWindow::onMouseMove(const MouseButton /*buttons*/, const int x, const int y)
{
	if (applicationState_ == AS_ROUGH_CONTOUR)
	{
		int bitmapX, bitmapY;

		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
		{
			pixelPositions_.emplace_back(bitmapX, bitmapY);
		}
	}
}

void ContourTrackerMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	setFrame(topLeft);

	const unsigned int extraContourOffset = 10u;

	if (applicationState_ == AS_ROUGH_CONTOUR)
	{
		if (!pixelPositions_.empty())
		{
			const HPEN pen = CreatePen(0, 5, 0xFF);
			const HPEN oldPen = HPEN(SelectObject(bitmap().dc(), pen));

			MoveToEx(bitmap().dc(), int(pixelPositions_.front().x()), int(pixelPositions_.front().y()), nullptr);
			for (const CV::PixelPosition& pixelPosition : pixelPositions_)
			{
				const int x = int(pixelPosition.x());
				const int y = int(pixelPosition.y());

				LineTo(bitmap().dc(), x, y);
			}

			SelectObject(bitmap().dc(), oldPen);
			DeleteObject(pen);
		}
	}
	else if (applicationState_ == AS_FINE_CONTOUR)
	{
		applicationState_ = AS_IDLE;

		if (pixelPositions_.size() >= 3)
		{
			if (contourTracker_.detectObject(topLeft, CV::Segmentation::PixelContour(pixelPositions_), randomGenerator_, extraContourOffset, &worker_))
			{
				ocean_assert(!contourTracker_.denseContourSubPixel().empty());
				applicationState_ = AS_TRACK_CONTOUR;
			}
		}
	}
	else if (applicationState_ == AS_TRACK_CONTOUR)
	{
		if (!contourTracker_.trackObject(topLeft, randomGenerator_, extraContourOffset, &worker_))
		{
			contourTracker_.clear();
			applicationState_ = AS_IDLE;
		}

		const CV::Segmentation::PixelContour& denseContour(contourTracker_.denseContour());
		const CV::PixelPositions& contourPoints = denseContour.pixels();

		const HPEN pen = CreatePen(0, 3, 0x00);
		const HPEN oldPen = HPEN(SelectObject(bitmap().dc(), pen));

		if (!contourPoints.empty())
		{
			MoveToEx(bitmap().dc(), int(contourPoints.back().x()), int(contourPoints.back().y()), nullptr);
		}

		for (const CV::PixelPosition& contourPoint : contourPoints)
		{
			LineTo(bitmap().dc(), int(contourPoint.x()), int(contourPoint.y()));
		}

		SelectObject(bitmap().dc(), oldPen);
		DeleteObject(pen);
	}

	repaint();
}
