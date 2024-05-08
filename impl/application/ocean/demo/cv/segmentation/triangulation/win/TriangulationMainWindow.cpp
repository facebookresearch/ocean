/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/triangulation/win/TriangulationMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/MaskCreator.h"
#include "ocean/cv/segmentation/PixelContour.h"
#include "ocean/cv/segmentation/Triangulation.h"

#include "ocean/platform/win/Utilities.h"

TriangulationMainWindow::TriangulationMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name)
{
	// nothing to do here
}

TriangulationMainWindow::~TriangulationMainWindow()
{
	// nothing to do here
}

void TriangulationMainWindow::onInitialized()
{
	bitmap_ = Platform::Win::Bitmap(areaWidth_, areaHeight_, FrameType::FORMAT_BGR24, FrameType::ORIGIN_UPPER_LEFT);
	adjustToBitmapSize();
}

void TriangulationMainWindow::onIdle()
{
	onPaint();
	Sleep(1);
}

void TriangulationMainWindow::onKeyDown(const int /*key*/)
{
	toggleVisualization_ = !toggleVisualization_;
}

void TriangulationMainWindow::onMouseUp(const MouseButton button, const int x, const int y)
{
	if (button == BUTTON_RIGHT)
	{
		pixelPositions_.clear();
	}
	else
	{
		int bitmapX, bitmapY;
		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
		{
			pixelPositions_.push_back(CV::PixelPosition(bitmapX, bitmapY));
		}

		repaint();
	}
}

void TriangulationMainWindow::onPaint()
{
	bitmap_ = Platform::Win::Bitmap(areaWidth_, areaHeight_, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);

	Rectangle(bitmap().dc(), -1, -1, bitmap().width() + 1, bitmap().height() + 1);

	if (pixelPositions_.size() >= 3)
	{
		maskFrame_.set(FrameType(areaWidth_, areaHeight_, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);
		maskFrame_.setValue(0xFF);

		HighPerformanceTimer timer;

		CV::Segmentation::PixelContour contour(pixelPositions_);
		contour.makeDense();

		const double contourTime = timer.mseconds();

		timer.start();

		CV::Segmentation::MaskCreator::denseContour2inclusiveMask(maskFrame_.data<uint8_t>(), maskFrame_.width(), maskFrame_.height(), maskFrame_.paddingElements(), contour, 0x80);

		const double maskTime = timer.mseconds();

		timer.start();

		// here in this demo application we use outline-4 pixels (that results in a (triangulated) mask slightly larger (by one pixel) than the input mask)
		// we could use MaskAnalyzer::findBorderPixels4() instead so that we receive the same (triangulated) mask as the input mask

		CV::PixelPositions outlinePixels4;
		CV::Segmentation::MaskAnalyzer::findOutline4(maskFrame_.constdata<uint8_t>(), maskFrame_.width(), maskFrame_.height(), maskFrame_.paddingElements(), outlinePixels4);

		CV::PixelPositions contour4;
		CV::Segmentation::MaskAnalyzer::pixels2contour(outlinePixels4, areaWidth_, areaHeight_, contour4);

		CV::Segmentation::PixelContour pixelContour(contour4);
		pixelContour.simplify();

		const CV::Segmentation::Triangulation::IndexTriangles indexTriangles(CV::Segmentation::Triangulation::triangulate(pixelContour));
		const CV::Advanced::PixelTriangles triangles(CV::Segmentation::Triangulation::triangulation2triangles(pixelContour.pixels(), indexTriangles));

		if (!toggleVisualization_)
		{
			CV::Segmentation::MaskCreator::triangles2inclusiveMask(maskFrame_.data<uint8_t>(), areaWidth_, areaHeight_, maskFrame_.paddingElements(), triangles.data(), triangles.size(), 0x60, WorkerPool::get().scopedWorker()());
		}

		const double triangulationTime = timer.mseconds();

		bitmap_ = Platform::Win::Bitmap(maskFrame_);
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Contour: ") + String::toAString(contourTime) + std::string("ms"));
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, std::string("Mask: ") + String::toAString(maskTime) + std::string("ms"));
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 45, std::string("Triangulation: ") + String::toAString(triangulationTime) + std::string("ms"));

		if (contour)
		{
			const size_t index = contour.indexLeftPosition();
			const CV::PixelPosition& position = contour.pixels()[index];

			Ellipse(bitmap().dc(), int(position.x()) - 6, int(position.y()) - 6, int(position.x()) + 6, int(position.y()) + 6);

			const bool counterClockwise = contour.isCounterClockwise();

			if (counterClockwise)
			{
				Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 105, "Counter clockwise");
			}
			else
			{
				Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 105, "Clockwise");
			}
		}
	}

	if (!pixelPositions_.empty())
	{
		const HPEN pen = CreatePen(0, 3, 0);
		const HPEN old = HPEN(SelectObject(bitmap().dc(), pen));

		MoveToEx(bitmap().dc(), int(pixelPositions_.back().x()), int(pixelPositions_.back().y()), nullptr);
		for (unsigned int n = 0; n < pixelPositions_.size(); ++n)
		{
			LineTo(bitmap().dc(), int(pixelPositions_[n].x()), int(pixelPositions_[n].y()));
		}

		SelectObject(bitmap().dc(), old);
		DeleteObject(pen);
	}

	for (unsigned int n = 0; n < pixelPositions_.size(); ++n)
	{
		const int x = int(pixelPositions_[n].x());
		const int y = int(pixelPositions_[n].y());

		Ellipse(bitmap().dc(), x - 2, y - 2, x + 2, y + 2);
		Platform::Win::Utilities::textOutput(bitmap().dc(), x + 5, y + 5, String::toAString(n));
	}

	BitmapWindow::onPaint();
}
