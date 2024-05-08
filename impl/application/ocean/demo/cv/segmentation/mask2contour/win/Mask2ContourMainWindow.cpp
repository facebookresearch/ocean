/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/mask2contour/win/Mask2ContourMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/MaskCreator.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/platform/win/Utilities.h"

Mask2ContourMainWindow::Mask2ContourMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	borderWindow_(instance, L"Detected border"),
	contourWindow_(instance, L"Reconstructed contour"),
	maskWindow_(instance, L"Reconstructed mask"),
	maskWindowConvex_(instance, L"Reconstructed convex mask"),
	maskWindowXOR_(instance, L"Reconstructed XOR mask"),
	maskWindowTriangulated_(instance, L"Reconstructed triangulated mask")
{
	maskFrame_.set(FrameType(areaWidth_, areaHeight_, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true, true);
	maskFrame_.setValue(0xFF);
}

Mask2ContourMainWindow::~Mask2ContourMainWindow()
{
	// nothing to do here
}

void Mask2ContourMainWindow::onInitialized()
{
	bitmap_ = Platform::Win::Bitmap(maskFrame_.width(), maskFrame_.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
	adjustToBitmapSize();

	borderWindow_.setParent(handle());
	borderWindow_.initialize();
	borderWindow_.show();

	contourWindow_.setParent(handle());
	contourWindow_.initialize();
	contourWindow_.show();

	maskWindow_.setParent(handle());
	maskWindow_.initialize();
	maskWindow_.show();

	maskWindowConvex_.setParent(handle());
	maskWindowConvex_.initialize();
	maskWindowConvex_.show();

	maskWindowXOR_.setParent(handle());
	maskWindowXOR_.initialize();
	maskWindowXOR_.show();

	maskWindowTriangulated_.setParent(handle());
	maskWindowTriangulated_.initialize();
	maskWindowTriangulated_.show();
}

void Mask2ContourMainWindow::onIdle()
{
	onPaint();
	Sleep(1);
}

void Mask2ContourMainWindow::onMouseDown(const MouseButton button, const int x, const int y)
{
	if (button == BUTTON_LEFT)
	{
		drawMask(x, y, 0x00);
	}
	else if (button == BUTTON_RIGHT)
	{
		drawMask(x, y, 0xFF);
	}
}

void Mask2ContourMainWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	if (buttons & BUTTON_LEFT)
	{
		drawMask(x, y, 0x00);
	}
	else if (buttons & BUTTON_RIGHT)
	{
		drawMask(x, y, 0xFF);
	}
}

void Mask2ContourMainWindow::onPaint()
{
	HighPerformanceTimer timer;

	// detect the border pixels with 4-neighborhood
	timer.start();

	CV::PixelPositions borderPixels4;
	ocean_assert(maskFrame_.isContinuous());
	CV::Segmentation::MaskAnalyzer::findBorderPixels4(maskFrame_.constdata<uint8_t>(), maskFrame_.width(), maskFrame_.height(), maskFrame_.paddingElements(), borderPixels4);

	const double timeBorder4 = timer.mseconds();


	// draw border frame
	Frame borderFrame(FrameType(maskFrame_.frameType()));
	borderFrame.setValue(0xFF);

	for (CV::PixelPositions::const_iterator i = borderPixels4.begin(); i != borderPixels4.end(); ++i)
	{
		borderFrame.pixel<uint8_t>(i->x(), i->y())[0] = 0x00;
	}

	timer.start();

	// determine outer and inner contours
	CV::Segmentation::PixelContours outerContours, innerContours;
	CV::Segmentation::MaskAnalyzer::pixels2contours(maskFrame_.constdata<uint8_t>(), maskFrame_.width(), maskFrame_.height(), borderPixels4, outerContours, innerContours);

	const double timeContours = timer.mseconds();


	// draw contour frame
	Frame contourFrame(FrameType(maskFrame_.frameType()));
	contourFrame.setValue(0xFF);

	for (size_t n = 0; n < outerContours.size(); ++n)
	{
		const CV::PixelPositions& contourPositions = outerContours[n].pixels();

		for (CV::PixelPositions::const_iterator i = contourPositions.begin(); i != contourPositions.end(); ++i)
		{
			contourFrame.pixel<uint8_t>(i->x(), i->y())[0] = 0x00;
		}
	}

	for (size_t n = 0; n < innerContours.size(); ++n)
	{
		const CV::PixelPositions& contourPositions = innerContours[n].pixels();

		for (CV::PixelPositions::const_iterator i = contourPositions.begin(); i != contourPositions.end(); ++i)
		{
			contourFrame.pixel<uint8_t>(i->x(), i->y())[0] = 0x80;
		}
	}


	// standard mask, able to handle masks with at most one hole in it
	Frame resultFrame(maskFrame_.frameType());
	resultFrame.setValue(0xFF);

	timer.start();

	for (size_t n = 0; n < outerContours.size(); ++n)
	{
		ocean_assert(resultFrame.isContinuous());
		CV::Segmentation::MaskCreator::denseContour2inclusiveMask(resultFrame.data<uint8_t>(), resultFrame.width(), resultFrame.height(), resultFrame.paddingElements(), outerContours[n], 0x00);
	}
	for (size_t n = 0; n < innerContours.size(); ++n)
	{
		ocean_assert(resultFrame.isContinuous());
		CV::Segmentation::MaskCreator::denseContour2exclusiveMask(resultFrame.data<uint8_t>(), resultFrame.width(), resultFrame.height(), resultFrame.paddingElements(), innerContours[n], 0xFF);
	}

	const double timeMask = timer.mseconds();


	// convex mask, very fast but not able to create a concave mask
	Frame resultFrameConvex(maskFrame_.frameType());
	resultFrameConvex.setValue(0xFF);

	timer.start();

	for (size_t n = 0; n < outerContours.size(); ++n)
	{
		ocean_assert(resultFrameConvex.isContinuous());
		CV::Segmentation::MaskCreator::denseContour2inclusiveMaskHotizontallyConvex(resultFrameConvex.data<uint8_t>(), resultFrameConvex.width(), resultFrameConvex.height(), outerContours[n].pixels(), 0x00);
	}

	const double timeMaskConvex = timer.mseconds();


	// XOR mask, able to handle arbitrary masks with e.g., recursive holes in it
	Frame resultFrameXOR(maskFrame_.frameType());
	resultFrameXOR.setValue(0xFF);

	timer.start();

	for (size_t n = 0; n < outerContours.size(); ++n)
	{
		ocean_assert(resultFrameXOR.isContinuous());
		CV::Segmentation::MaskCreator::denseContour2inclusiveMaskXOR(resultFrameXOR.data<uint8_t>(), resultFrameXOR.width(), resultFrameXOR.height(), resultFrameXOR.paddingElements(), outerContours[n]);
	}
	for (size_t n = 0; n < innerContours.size(); ++n)
	{
		ocean_assert(resultFrameXOR.isContinuous());
		CV::Segmentation::MaskCreator::denseContour2exclusiveMaskXOR(resultFrameXOR.data<uint8_t>(), resultFrameXOR.width(), resultFrameXOR.height(), resultFrameXOR.paddingElements(), innerContours[n]);
	}

	const double timeMaskXOR = timer.mseconds();


	// triangulated mask, triangulating the contour, quite slow not able to handle masks with holes
	Frame resultFrameTriangulated(maskFrame_.frameType());
	resultFrameTriangulated.setValue(0xFF);

	timer.start();

	for (size_t n = 0; n < outerContours.size(); ++n)
	{
		ocean_assert(resultFrameXOR.isContinuous());
		CV::Segmentation::MaskCreator::contour2inclusiveMaskByTriangulation(resultFrameTriangulated.data<uint8_t>(), resultFrameTriangulated.width(), resultFrameTriangulated.height(), resultFrameTriangulated.paddingElements(), outerContours[n].simplified(), 0x00, WorkerPool::get().scopedWorker()());
	}

	const double timeMaskTriangulated = timer.mseconds();


	// update the windows

	borderWindow_.setFrame(borderFrame);
	borderWindow_.repaint();

	contourWindow_.setFrame(contourFrame);
	contourWindow_.repaint();

	maskWindow_.setFrame(resultFrame);
	maskWindow_.repaint();

	maskWindowConvex_.setFrame(resultFrameConvex);
	maskWindowConvex_.repaint();

	maskWindowXOR_.setFrame(resultFrameXOR);
	maskWindowXOR_.repaint();

	maskWindowTriangulated_.setFrame(resultFrameTriangulated);
	maskWindowTriangulated_.repaint();

	setFrame(maskFrame_);

	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, std::string("Border: ") + String::toAString(timeBorder4) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 25, std::string("Contour: ") + String::toAString(timeContours) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 55, std::string("Mask: ") + String::toAString(timeMask) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 75, std::string("Convex Mask: ") + String::toAString(timeMaskConvex) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 95, std::string("XOR Mask: ") + String::toAString(timeMaskXOR) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 115, std::string("Triangulated Mask: ") + String::toAString(timeMaskTriangulated) + std::string("ms"));

	BitmapWindow::onPaint();
}

void Mask2ContourMainWindow::drawMask(const int x, const int y, const uint8_t value)
{
	if (x >= 0 && x < int(maskFrame_.width()) && y >= 0 && y < int(maskFrame_.height()))
	{
		const unsigned int size = (GetKeyState(VK_CONTROL) & 0x8000) ? 91u : 31u;

		CV::Canvas::ellipse(maskFrame_, CV::PixelPosition(x, y), size, size, &value);
	}
}
