/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/BitmapWindow.h"
#include "ocean/platform/wxwidgets/Utilities.h"

#include "ocean/base/String.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(BitmapWindow, wxScrolledWindow)
	EVT_PAINT(BitmapWindow::onPaint)
	EVT_SIZE(BitmapWindow::onSize)
	EVT_LEFT_DOWN(BitmapWindow::onMouseLeftDown)
	EVT_LEFT_UP(BitmapWindow::onMouseLeftUp)
	EVT_LEFT_DCLICK(BitmapWindow::onMouseLeftDblClick)
	EVT_MIDDLE_DOWN(BitmapWindow::onMouseMiddleDown)
	EVT_MIDDLE_UP(BitmapWindow::onMouseMiddleUp)
	EVT_RIGHT_DOWN(BitmapWindow::onMouseRightDown)
	EVT_RIGHT_UP(BitmapWindow::onMouseRightUp)
	EVT_RIGHT_DCLICK(BitmapWindow::onMouseRightDblClick)
	EVT_MOUSEWHEEL(BitmapWindow::onMouseWheel)
	EVT_MOTION(BitmapWindow::onMouseMove)
	EVT_ERASE_BACKGROUND(BitmapWindow::onEraseBackground)
END_EVENT_TABLE()

BitmapWindow::BitmapWindow(const wxString& title, wxWindow* parent, const wxPoint& pos, const wxSize& size) :
	wxScrolledWindow(parent, -1, pos, size, wxRETAINED, title)
{
	// nothing to do here
}

void BitmapWindow::setBitmap(const wxBitmap& bitmap, const bool resetZoom)
{
	TemporaryScopedLock scopedLock(bitmapLock_);
		bitmap_ = bitmap;
	scopedLock.release();

	displayBitmapLeft_ = NumericT<int>::minValue();
	displayBitmapTop_ = NumericT<int>::minValue();
	displayBitmapWidth_ = NumericT<int>::minValue();
	displayBitmapHeight_ = NumericT<int>::minValue();

	previousMouseMiddleBitmapX_ = Numeric::minValue();
	previousMouseMiddleBitmapY = Numeric::minValue();

	if (resetZoom)
	{
		zoom_ = 1.0;
	}

	Refresh(true);
}

bool BitmapWindow::setFrame(const Frame& frame, const bool resetZoom)
{
	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	rgbFrame.makeContinuous();

	const wxImage image = wxImage(int(rgbFrame.width()), int(rgbFrame.height()), const_cast<uint8_t*>(rgbFrame.constdata<uint8_t>()), true);
	setBitmap(wxBitmap(image), resetZoom);

	return true;
}

void BitmapWindow::setDisplayMode(const DisplayMode mode)
{
	displayMode_ = mode;
	Refresh(false);
}

void BitmapWindow::setZoom(const Scalar zoom)
{
	if (zoom <= Numeric::eps())
	{
		return;
	}

	displayBitmapLeft_ = NumericT<int>::minValue();
	displayBitmapTop_ = NumericT<int>::minValue();
	displayBitmapWidth_ = NumericT<int>::minValue();
	displayBitmapHeight_ = NumericT<int>::minValue();

	previousMouseMiddleBitmapX_ = Numeric::minValue();
	previousMouseMiddleBitmapY = Numeric::minValue();

	zoom_ = zoom;

	SetVirtualSize(zoomedWidth(), zoomedHeight());

	Refresh(false);
	Update();
}

void BitmapWindow::setEnableScrollbars(const bool /*enable*/)
{
	ocean_assert(false && "Not implemented!");
}

void BitmapWindow::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	DoPrepareDC(dc);

	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	if (bitmap_.IsNull() || bitmapWidth <= 0 || bitmapHeight <= 0)
	{
		dc.Clear();
		return;
	}

	int clientWidth = 0;
	int clientHeight = 0;
	GetClientSize(&clientWidth, &clientHeight);

	if (clientWidth <= 0 || clientHeight <= 0)
	{
		return;
	}

	switch (displayMode_)
	{
		case DM_USER:
		{
			const Scalar zoomWidth = Scalar(bitmapWidth) * zoom_;
			const Scalar zoomHeight = Scalar(bitmapHeight) * zoom_;

			const int zoomWidthR = int(floor(zoomWidth + 0.5));
			const int zoomHeightR = int(floor(zoomHeight + 0.5));

			int virtualWidth = 0;
			int virtualHeight = 0;
			GetVirtualSize(&virtualWidth, &virtualHeight);

			const int left = (virtualWidth - zoomWidthR) / 2;
			const int top = (virtualHeight - zoomHeightR) / 2;

			displayBitmapLeft_ = left;
			displayBitmapTop_ = top;
			displayBitmapWidth_ = zoomWidthR;
			displayBitmapHeight_ = zoomHeightR;
			Utilities::bitmapOutput(dc, left, top, zoomWidthR, zoomHeightR, 0, 0, bitmapWidth, bitmapHeight, bitmap_);

			break;
		}

		case DM_STRETCH:
		{
			displayBitmapLeft_ = 0;
			displayBitmapTop_ = 0;
			displayBitmapWidth_ = clientWidth;
			displayBitmapHeight_ = clientHeight;
			Utilities::bitmapOutput(dc, 0, 0, clientWidth, clientHeight, 0, 0, bitmapWidth, bitmapHeight, bitmap_);
			break;
		}

		case DM_ENLARGE_TO_SCREEN:
		{
			if (clientWidth >= bitmapWidth && clientHeight >= bitmapHeight)
			{
				// image needs to be enlarged to fit to the client area and a background border may be visible
				paintFit2Client(dc, clientWidth, clientHeight);
			}
			else
			{
				// the image needs to be centered without zooming
				paintSubset(dc, clientWidth, clientHeight);
			}

			break;
		}

		case DM_ZOOM_TO_FULLSCREEN:
		{
			paintFit2Fullscreen(dc, clientWidth, clientHeight);
			break;
		}

		case DM_SHRINK_TO_SCREEN:
		{
			if (bitmapWidth <= clientWidth && bitmapHeight <= clientHeight)
				paintCenter(dc, clientWidth, clientHeight, bitmapWidth, bitmapHeight);
			else
				paintFit2Client(dc, clientWidth, clientHeight);

			break;
		}
	}

	onPaintOverlay(event, dc);
}

void BitmapWindow::onPaintOverlay(wxPaintEvent& /*event*/, wxPaintDC& /*dc*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseLeftDown(wxMouseEvent& /*event*/)
{
	SetFocus();
}

void BitmapWindow::onMouseLeftUp(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseLeftDblClick(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseMiddleDown(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseMiddleUp(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseRightDown(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseRightUp(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseRightDblClick(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void BitmapWindow::onMouseWheel(wxMouseEvent& event)
{
	bool eraseBackground = false;

	if (event.GetWheelRotation() < 0)
	{
		zoom_ *= 0.9;
		eraseBackground = true;
	}
	else
	{
		zoom_ *= 1 / 0.9;
	}

	TemporaryScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
		return;

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	scopedLock.release();

	const int zoomWidth = int(floor(zoom_ * Scalar(bitmapWidth) + 0.5));
	const int zoomHeight = int(floor(zoom_ * Scalar(bitmapHeight) + 0.5));

	int clientWidth = 0;
	int clientHeight = 0;
	GetClientSize(&clientWidth, &clientHeight);

	const int virtualWidth = max(zoomWidth, clientWidth);
	const int virtualHeight = max(zoomHeight, clientHeight);

	if (scrollbarsEnabled_)
	{
		const bool needHorizontalScrollbar = virtualWidth > clientWidth;
		const bool needVerticalScrollbar = virtualHeight > clientHeight;

		const int scrollRateX = needHorizontalScrollbar ? 1 : 0;
		const int scrollRateY = needVerticalScrollbar ? 1 : 0;

		SetScrollRate(scrollRateX, scrollRateY);
	}

	// we want to ensure that the area of interest does not change, so that we need to adjust the scroll bar locations

	int oldX = 0;
	int oldY = 0;
	GetViewStart(&oldX, &oldY);

	const wxSize oldVirtualSize = GetVirtualSize();

	SetVirtualSize(virtualWidth, virtualHeight);

	if (oldVirtualSize.x != 0 && oldVirtualSize.y != 0)
	{
		const int newX = double(oldX + event.GetPosition().x) / double(oldVirtualSize.x) * virtualWidth - event.GetPosition().x;
		const int newY = double(oldY + event.GetPosition().y) / double(oldVirtualSize.y) * virtualHeight - event.GetPosition().y;

		SetScrollbars(1, 1, virtualWidth, virtualHeight, minmax(0, newX, virtualWidth - 10), minmax(0, newY, virtualHeight - 10));
	}

	displayMode_ = DM_USER;

	// adds a repaint request to the event queue
	Refresh(eraseBackground);
}

void BitmapWindow::paintCenter(wxDC& dc, const int clientWidth, const int clientHeight, const int bitmapWidth, const int bitmapHeight)
{
	const int left = (clientWidth - bitmapWidth) / 2;
	const int top = (clientHeight - bitmapHeight) / 2;

	zoom_ = 1.0;

	ocean_assert(left >= 0 && top >= 0);

	displayBitmapLeft_ = left;
	displayBitmapTop_ = top;
	displayBitmapWidth_ = bitmapWidth;
	displayBitmapHeight_ = bitmapHeight;

	const ScopedLock scopedLock(bitmapLock_);

	Utilities::bitmapOutput(dc, left, top, bitmap_);
}

void BitmapWindow::paintSubset(wxDC& dc, const int clientWidth, const int clientHeight)
{
	zoom_ = 1.0;

	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	ocean_assert(clientWidth > 0 && clientHeight > 0);
	ocean_assert(bitmapWidth > 0 && bitmapHeight > 0);

	if (clientWidth >= bitmapWidth)
	{
		ocean_assert(clientHeight < bitmapHeight);

		const int outputLeft = (clientWidth - bitmapWidth) / 2;
		const int inputHeight = clientHeight;
		const int inputTop = (bitmapHeight - clientHeight) / 2;

		displayBitmapLeft_ = outputLeft;
		displayBitmapTop_ = 0;
		displayBitmapWidth_ = bitmapWidth;
		displayBitmapHeight_ = bitmapHeight;
		Utilities::bitmapOutput(dc, outputLeft, 0, bitmapWidth, clientHeight, 0, inputTop, bitmapWidth, inputHeight, bitmap_);
	}
	else
	{
		ocean_assert(clientWidth < bitmapWidth);

		const int outputTop = (clientHeight - bitmapHeight) / 2;
		const int inputWidth = clientWidth;
		const int inputLeft = (bitmapWidth - clientWidth) / 2;

		displayBitmapLeft_ = 0;
		displayBitmapTop_ = outputTop;
		displayBitmapWidth_ = bitmapWidth;
		displayBitmapHeight_ = bitmapHeight;
		Utilities::bitmapOutput(dc, 0, outputTop, clientWidth, bitmapHeight, inputLeft, 0, inputWidth, bitmapHeight, bitmap_);
	}
}

void BitmapWindow::paintFit2Client(wxDC& dc, const int clientWidth, const int clientHeight)
{
	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	ocean_assert(clientWidth > 0 && clientHeight > 0);
	ocean_assert(bitmapWidth > 0 && bitmapHeight > 0);

	const Scalar clientRatio = Scalar(clientWidth) / Scalar(clientHeight);
	const Scalar bitmapRatio = Scalar(bitmapWidth) / Scalar(bitmapHeight);

	if (clientRatio >= bitmapRatio)
	{
		const int displayHeight = clientHeight;
		zoom_ = Scalar(displayHeight) / Scalar(bitmapHeight);

		const Scalar displayWidth = Scalar(displayHeight) * bitmapRatio;

		const int displayWidthR = floor(displayWidth + 0.5);
		const int left = (clientWidth - displayWidthR) / 2;
		ocean_assert(left >= 0);

		displayBitmapLeft_ = left;
		displayBitmapTop_ = 0;
		displayBitmapWidth_ = displayWidthR;
		displayBitmapHeight_ = displayHeight;
		Utilities::bitmapOutput(dc, left, 0, displayWidthR, displayHeight, 0, 0, bitmapWidth, bitmapHeight, bitmap_);
	}
	else
	{
		const int displayWidth = clientWidth;
		zoom_ = Scalar(displayWidth) / Scalar(bitmapWidth);

		const Scalar displayHeight = Scalar(displayWidth) / bitmapRatio;

		const int displayHeightR = floor(displayHeight + 0.5);
		const int top = (clientHeight - displayHeightR) / 2;
		ocean_assert(top >= 0);

		displayBitmapLeft_ = 0;
		displayBitmapTop_ = top;
		displayBitmapWidth_ = displayWidth;
		displayBitmapHeight_ = displayHeightR;
		Utilities::bitmapOutput(dc, 0, top, displayWidth, displayHeightR, 0, 0, bitmapWidth, bitmapHeight, bitmap_);
	}
}

void BitmapWindow::paintFit2Fullscreen(wxDC& dc, const int clientWidth, const int clientHeight)
{
	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	ocean_assert(clientWidth > 0 && clientHeight > 0);
	ocean_assert(bitmapWidth > 0 && bitmapHeight > 0);

	const Scalar clientRatio = Scalar(clientWidth) / Scalar(clientHeight);
	const Scalar bitmapRatio = Scalar(bitmapWidth) / Scalar(bitmapHeight);

	if (clientRatio < bitmapRatio)
	{
		const int outputHeight = clientHeight;
		zoom_ = Scalar(outputHeight) / Scalar(bitmapHeight);

		const Scalar inputWidth = Scalar(clientWidth) / zoom_;

		const int inputWidthR = floor(inputWidth + 0.5);
		const int inputLeft = (bitmapWidth - inputWidthR) / 2;
		ocean_assert(inputLeft >= 0);

		displayBitmapWidth_ = int(floor(Scalar(bitmapWidth) * zoom_ + 0.5));
		displayBitmapHeight_ = int(floor(Scalar(bitmapHeight) * zoom_ + 0.5));

		ocean_assert(displayBitmapWidth_ >= clientWidth);
		displayBitmapLeft_ = (displayBitmapWidth_ - clientWidth) / 2;
		displayBitmapTop_ = 0;
		Utilities::bitmapOutput(dc, 0, 0, clientWidth, clientHeight, inputLeft, 0, inputWidthR, bitmapHeight, bitmap_);
	}
	else
	{
		const int outputWidth = clientWidth;
		zoom_ = Scalar(outputWidth) / Scalar(bitmapWidth);

		const Scalar inputHeight = Scalar(clientHeight) / zoom_;

		const int inputHeightR = floor(inputHeight + 0.5);
		const int inputTop = (bitmapHeight - inputHeightR) / 2;
		ocean_assert(inputTop >= 0);

		displayBitmapWidth_ = int(floor(Scalar(bitmapWidth) * zoom_ + 0.5));
		displayBitmapHeight_ = int(floor(Scalar(bitmapHeight) * zoom_ + 0.5));

		ocean_assert(displayBitmapHeight_ >= clientHeight);
		displayBitmapLeft_ = 0;
		displayBitmapTop_ = (displayBitmapHeight_ - clientHeight) / 2;
		Utilities::bitmapOutput(dc, 0, 0, clientWidth, clientHeight, 0, inputTop, bitmapWidth, inputHeightR, bitmap_);
	}
}

void BitmapWindow::onSize(wxSizeEvent& /*event*/)
{
	if (displayMode_ == DM_USER)
	{
		TemporaryScopedLock scopedLock(bitmapLock_);

		if (!bitmap_.IsOk())
		{
			return;
		}

		const int bitmapWidth = bitmap_.GetWidth();
		const int bitmapHeight = bitmap_.GetHeight();

		scopedLock.release();

		const int zoomWidth = int(floor(zoom_ * Scalar(bitmapWidth) + 0.5));
		const int zoomHeight = int(floor(zoom_ * Scalar(bitmapHeight) + 0.5));

		int clientWidth = 0;
		int clientHeight = 0;
		GetClientSize(&clientWidth, &clientHeight);

		const int virtualWidth = max(zoomWidth, clientWidth);
		const int virtualHeight = max(zoomHeight, clientHeight);

		if (scrollbarsEnabled_)
		{
			const bool needHorizontalScrollbar = virtualWidth > clientWidth;
			const bool needVerticalScrollbar = virtualHeight > clientHeight;

			const int scrollRateX = needHorizontalScrollbar ? 1 : 0;
			const int scrollRateY = needVerticalScrollbar ? 1 : 0;

			SetScrollRate(scrollRateX, scrollRateY);
		}

		SetVirtualSize(virtualWidth, virtualHeight);
	}

	// adds a repaint request to the event queue (and erases the background)
	Refresh(true);
}

void BitmapWindow::onMouseMove(wxMouseEvent& event)
{
	if (event.MiddleIsDown())
	{
		if (GetVirtualSize() != GetClientSize())
		{
			const wxPoint point = event.GetPosition();

			const int currentX = point.x;
			const int currentY = point.y;

			ocean_assert(previousMouseX_ != -1 && previousMouseY_ != -1);

			const int deltaX = currentX - previousMouseX_;
			const int deltaY = currentY - previousMouseY_;

			int x, y;
			GetViewStart(&x, &y);

			const int virtualWidth = GetVirtualSize().x;
			const int virtualHeight = GetVirtualSize().y;

			SetScrollbars(1, 1, virtualWidth, virtualHeight, minmax(0, x - deltaX, virtualWidth - 10), minmax(0, y - deltaY, virtualHeight - 10));
		}
	}

	previousMouseX_ = event.GetPosition().x;
	previousMouseY_ = event.GetPosition().y;
}

void BitmapWindow::onEraseBackground(wxEraseEvent& /*event*/)
{
	wxClientDC dc(this);

	const int width = zoomedWidth();
	const int height = zoomedHeight();

	const wxSize virtualSize = GetVirtualSize();

	const int left = (virtualSize.x - width) / 2;
	const int top = (virtualSize.y - height) / 2;

	dc.SetBrush(wxBrush(GetBackgroundColour()));
	dc.SetPen(wxPen(GetBackgroundColour()));

	// top area
	dc.DrawRectangle(0, 0, virtualSize.x, top);
	dc.DrawRectangle(0, top, left, height);
	dc.DrawRectangle(left + width, top, virtualSize.x - width - left, height);
	dc.DrawRectangle(0, top + height, virtualSize.x, virtualSize.y - height - top);
}

int BitmapWindow::zoomedWidth() const
{
	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return 0;
	}

	return int(floor(zoom_ * Scalar(bitmap_.GetWidth()) + Scalar(0.5)));
}

int BitmapWindow::zoomedHeight() const
{
	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return 0;
	}

	return int(floor(zoom_ * Scalar(bitmap_.GetHeight()) + Scalar(0.5)));
}

bool BitmapWindow::window2bitmap(const int x, const int y, Scalar& xBitmap, Scalar& yBitmap, const bool allowPositionOutsideBitmap)
{
	if (displayBitmapLeft_ == NumericT<int>::minValue() || displayBitmapTop_ == NumericT<int>::minValue() || displayBitmapWidth_ <= 0 || displayBitmapHeight_ <= 0)
	{
		return false;
	}

	int xVirtual = -1;
	int yVirtual = -1;
	CalcUnscrolledPosition(x, y, &xVirtual, &yVirtual);

	if (allowPositionOutsideBitmap == false && (xVirtual <= displayBitmapLeft_ || yVirtual <= displayBitmapTop_ || xVirtual >= displayBitmapLeft_ + displayBitmapWidth_ || yVirtual >= displayBitmapTop_ + displayBitmapHeight_))
	{
		return false;
	}

	const int xPos = (xVirtual - displayBitmapLeft_);
	const int yPos = (yVirtual - displayBitmapTop_);

	ocean_assert(allowPositionOutsideBitmap || (xPos >= 0 && xPos < displayBitmapWidth_));
	ocean_assert(allowPositionOutsideBitmap || (yPos >= 0 && yPos < displayBitmapHeight_));

	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return false;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	xBitmap = Scalar(xPos * bitmapWidth) / Scalar(displayBitmapWidth_);
	yBitmap = Scalar(yPos * bitmapHeight) / Scalar(displayBitmapHeight_);

	return true;
}

bool BitmapWindow::virtualWindow2bitmap(const int xVirtual, const int yVirtual, Scalar& xBitmap, Scalar& yBitmap)
{
	if (displayBitmapLeft_ == NumericT<int>::minValue() || displayBitmapTop_ == NumericT<int>::minValue() || displayBitmapWidth_ <= 0 || displayBitmapHeight_ <= 0)
	{
		return false;
	}

	if (xVirtual <= displayBitmapLeft_ || yVirtual <= displayBitmapTop_ || xVirtual >= displayBitmapLeft_ + displayBitmapWidth_ || yVirtual >= displayBitmapTop_ + displayBitmapHeight_)
	{
		return false;
	}

	const int xPos = (xVirtual - displayBitmapLeft_);
	const int yPos = (yVirtual - displayBitmapTop_);

	ocean_assert(xPos >= 0 && xPos < displayBitmapWidth_);
	ocean_assert(yPos >= 0 && yPos < displayBitmapHeight_);

	const ScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return false;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	xBitmap = Scalar(xPos * bitmapWidth) / Scalar(displayBitmapWidth_);
	yBitmap = Scalar(yPos * bitmapHeight) / Scalar(displayBitmapHeight_);

	return true;
}

bool BitmapWindow::bitmap2virtualWindow(const Scalar xBitmap, const Scalar yBitmap, Scalar& xVirtual, Scalar& yVirtual, const bool allowPositionOutsideBitmap)
{
	TemporaryScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return false;
	}

	const Scalar bitmapWidth = Scalar(bitmap_.GetWidth());
	const Scalar bitmapHeight = Scalar(bitmap_.GetHeight());

	scopedLock.release();

	if (bitmapWidth <= Numeric::eps() || bitmapHeight <= Numeric::eps())
	{
		return false;
	}

	if (allowPositionOutsideBitmap == false && (xBitmap <= 0 || yBitmap <= 0 || xBitmap >= bitmapWidth || yBitmap >= bitmapHeight))
	{
		return false;
	}

	xVirtual = Scalar(displayBitmapLeft_) + (xBitmap * displayBitmapWidth_) / Scalar(bitmapWidth);
	yVirtual = Scalar(displayBitmapTop_) + (yBitmap * displayBitmapHeight_) / Scalar(bitmapHeight);

	ocean_assert(allowPositionOutsideBitmap || (xVirtual >= 0 && yVirtual >= 0));
	ocean_assert(allowPositionOutsideBitmap || (xVirtual < GetVirtualSize().x && yVirtual <= GetVirtualSize().y));

	return true;
}

bool BitmapWindow::bitmap2window(const int xBitmap, const int yBitmap, Scalar& x, Scalar& y)
{
	TemporaryScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return false;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	scopedLock.release();

	if (bitmapWidth <= 0 || bitmapHeight <= 0)
	{
		return false;
	}

	if (xBitmap <= 0 || yBitmap <= 0 || xBitmap >= bitmapWidth || yBitmap >= bitmapHeight)
	{
		return false;
	}

	const Scalar xVirtual = Scalar(displayBitmapLeft_) + Scalar(xBitmap * displayBitmapWidth_) / Scalar(bitmapWidth);
	const Scalar yVirtual = Scalar(displayBitmapTop_) + Scalar(yBitmap * displayBitmapHeight_) / Scalar(bitmapHeight);

	ocean_assert(xVirtual >= 0 && yVirtual >= 0);
	ocean_assert(xVirtual < GetVirtualSize().x && yVirtual <= GetVirtualSize().y);

	int xScrollPosition = 0;
	int yScrollPosition = 0;
	GetViewStart(&xScrollPosition, &yScrollPosition);

	int xScrollUnit = 0;
	int yScrollUnit = 0;
	GetScrollPixelsPerUnit(&xScrollUnit, &yScrollUnit);

	x = xVirtual - Scalar(xScrollPosition * xScrollUnit);
	y = yVirtual - Scalar(yScrollPosition * yScrollUnit);

	return true;
}

bool BitmapWindow::bitmap2window(const Scalar xBitmap, const Scalar yBitmap, Scalar& x, Scalar& y)
{
	TemporaryScopedLock scopedLock(bitmapLock_);

	if (!bitmap_.IsOk())
	{
		return false;
	}

	const int bitmapWidth = bitmap_.GetWidth();
	const int bitmapHeight = bitmap_.GetHeight();

	scopedLock.release();

	if (bitmapWidth <= 0 || bitmapHeight <= 0)
	{
		return false;
	}

	if (xBitmap <= 0 || yBitmap <= 0 || xBitmap >= Scalar(bitmapWidth) || yBitmap >= Scalar(bitmapHeight))
	{
		return false;
	}

	const Scalar xVirtual = Scalar(displayBitmapLeft_) + Scalar(xBitmap * displayBitmapWidth_) / Scalar(bitmapWidth);
	const Scalar yVirtual = Scalar(displayBitmapTop_) + Scalar(yBitmap * displayBitmapHeight_) / Scalar(bitmapHeight);

	ocean_assert(xVirtual >= 0 && yVirtual >= 0);
	ocean_assert(xVirtual < GetVirtualSize().x && yVirtual <= GetVirtualSize().y);

	int xScrollPosition = 0;
	int yScrollPosition = 0;
	GetViewStart(&xScrollPosition, &yScrollPosition);

	int xScrollUnit = 0;
	int yScrollUnit = 0;
	GetScrollPixelsPerUnit(&xScrollUnit, &yScrollUnit);

	x = xVirtual - Scalar(xScrollPosition * xScrollUnit);
	y = yVirtual - Scalar(yScrollPosition * yScrollUnit);

	return true;
}

}

}

}
