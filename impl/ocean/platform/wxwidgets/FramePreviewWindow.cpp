/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/FramePreviewWindow.h"
#include "ocean/platform/wxwidgets/Utilities.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(FramePreviewWindow, wxPanel)
	EVT_PAINT(FramePreviewWindow::onPaint)
	EVT_SIZE(FramePreviewWindow::onSize)
	EVT_LEFT_DOWN(FramePreviewWindow::onMouseLeftDown)
	EVT_LEFT_UP(FramePreviewWindow::onMouseLeftUp)
	EVT_LEFT_DCLICK(FramePreviewWindow::onMouseLeftDblClick)
	EVT_MIDDLE_DOWN(FramePreviewWindow::onMouseMiddleDown)
	EVT_MIDDLE_UP(FramePreviewWindow::onMouseMiddleUp)
	EVT_RIGHT_DOWN(FramePreviewWindow::onMouseRightDown)
	EVT_RIGHT_UP(FramePreviewWindow::onMouseRightUp)
	EVT_RIGHT_DCLICK(FramePreviewWindow::onMouseRightDblClick)
	EVT_MOUSEWHEEL(FramePreviewWindow::onMouseWheel)
	EVT_MOTION(FramePreviewWindow::onMouseMove)
	EVT_ERASE_BACKGROUND(FramePreviewWindow::onEraseBackground)
END_EVENT_TABLE()

FramePreviewWindow::FramePreviewWindow(const wxString& title, wxWindow* parent, const wxPoint& pos, const wxSize& size) :
	wxPanel(parent, -1, pos, size, wxRETAINED, title)
{
	// nothing to do here
}

FramePreviewWindow::~FramePreviewWindow()
{
	if (frameProvider_)
	{
		frameProvider_->removePreviewFrameCallback(Media::MovieFrameProvider::PreviewFrameCallback(*this, &FramePreviewWindow::onPreviewFrame));
		frameProvider_->removeEventCallback(Media::MovieFrameProvider::EventCallback(*this, &FramePreviewWindow::onEvent));
	}
}

void FramePreviewWindow::setFrameProvider(const Media::MovieFrameProviderRef& frameProvider)
{
	if (frameProvider_)
	{
		frameProvider_->removePreviewFrameCallback(Media::MovieFrameProvider::PreviewFrameCallback(*this, &FramePreviewWindow::onPreviewFrame));
		frameProvider_->removeEventCallback(Media::MovieFrameProvider::EventCallback(*this, &FramePreviewWindow::onEvent));
	}

	frameProvider_ = frameProvider;
	frameNumber_ = 0u;
	virtualWidth_ = 0u;

	if (frameProvider_)
	{
		frameNumber_ = frameProvider_->frameNumber();

		frameProvider_->addPreviewFrameCallback(Media::MovieFrameProvider::PreviewFrameCallback(*this, &FramePreviewWindow::onPreviewFrame));
		frameProvider_->addEventCallback(Media::MovieFrameProvider::EventCallback(*this, &FramePreviewWindow::onEvent));

		const int height = GetClientSize().GetHeight();

		if (height > 0)
		{
			frameProvider_->setPreferredPreviewDimension(0u, (unsigned int)height);
		}
	}

	updateDispalyParameter();
}

void FramePreviewWindow::onPaint(wxPaintEvent& /*event*/)
{
	wxPaintDC dc(this);

	if (newPreviewFrame_)
	{
		frameNumber_ = frameProvider_->frameNumber();
		updateDispalyParameter();

		newPreviewFrame_ = false;
	}

	const wxPen oldPen(dc.GetPen());
	const wxBrush oldBrush(dc.GetBrush());

	dc.SetPen(wxPen(dc.GetBackground().GetColour()));
	dc.SetBrush(dc.GetBackground());

	const int clientWidth = GetClientSize().GetWidth();
	const int clientHeight = GetClientSize().GetHeight();

	if (frameProvider_.isNull() || clientWidth <= 0 || clientHeight <= 0 || previewWidth_ == 0u || previewHeight_ == 0u || frameNumber_ == 0u || previewZoom_ == 0u)
	{
		dc.DrawRectangle(0, 0, clientWidth, clientHeight);
		return;
	}

	int indexFirst;
	if (!calculateFirstVisibleElement(virtualPositionStart_, previewWidth_, separatorWidth_, previewZoom_, true, indexFirst))
	{
		dc.DrawRectangle(0, 0, clientWidth, clientHeight);
		return;
	}

	if (indexFirst >= int(frameNumber_))
	{
		dc.DrawRectangle(0, 0, clientWidth, clientHeight);
		return;
	}

	int indexLast;
	if (!calculateLastVisibleElement(virtualPositionStart_, (unsigned int)clientWidth, previewWidth_, separatorWidth_, previewZoom_, true, indexLast))
	{
		dc.DrawRectangle(0, 0, clientWidth, clientHeight);
		return;
	}

	if (indexLast < 0)
	{
		dc.DrawRectangle(0, 0, clientWidth, clientHeight);
		return;
	}

	// crop first and last index
	indexFirst = max(0, indexFirst);
	ocean_assert(indexLast >= indexFirst);

	ocean_assert(frameNumber_ > 0u);
	indexLast = min(indexLast, int(frameNumber_) - 1);
	ocean_assert(indexLast >= 0);

	indexLast -= indexLast % int(previewZoom_);
	ocean_assert(indexLast >= 0 && indexLast % int(previewZoom_) == 0);
	ocean_assert(indexLast >= indexFirst);

	const unsigned int indices = (unsigned int)((indexLast - indexFirst) / int(previewZoom_)) + 1u;

	elements_.clear();
	elements_.reserve(indices);

	for (unsigned int n = 0u; n < indices; ++n)
	{
		const unsigned int index = indexFirst + n * previewZoom_;
		int virtualPositionLeft;

		if (calculateVirtualPositionStart(index, previewWidth_, separatorWidth_, previewZoom_, virtualPositionLeft))
		{
			elements_.push_back(Element(index, virtualPositionLeft, n));
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}

	ocean_assert(elements_.size() == indices);
	ocean_assert(!elements_.empty());

	// paint left border
	const int firstDisplayPosition = elements_.front().displayPositionStart(virtualPositionStart_, previewZoom_);
	if (firstDisplayPosition > 0)
	{
		dc.DrawRectangle(0, 0, firstDisplayPosition, clientHeight);
	}

	const FrameRefs previewFrames(frameProvider_->previewFrames(indexFirst, indices, previewZoom_));
	ocean_assert(previewFrames.size() == elements_.size());

	for (Elements::const_iterator i = elements_.begin(); i != elements_.end(); /* noop */)
	{
		const int displayPositionLeft = i->displayPositionStart(virtualPositionStart_, previewZoom_);

		ocean_assert(i->displayId() < previewFrames.size());

		if (!previewFrames[i->displayId()].isNull())
		{
			drawFrame(dc, *previewFrames[i->displayId()], i->index(), displayPositionLeft, i->index() == selectionIndex_);
		}

		if (++i != elements_.end())
		{
			dc.DrawRectangle(displayPositionLeft + int(previewWidth_), 0, separatorWidth_, clientHeight);
		}
	}

	// paint right border
	const int lastDisplayPositionEnd = elements_.back().displayPositionEnd(virtualPositionStart_, previewWidth_, previewZoom_);
	if (lastDisplayPositionEnd < clientWidth)
	{
		dc.DrawRectangle(lastDisplayPositionEnd, 0, clientWidth - lastDisplayPositionEnd, clientHeight);
	}

	dc.SetPen(oldPen);
	dc.SetBrush(oldBrush);
}

void FramePreviewWindow::drawFrame(wxDC& dc, const Frame& frame, const unsigned int frameIndex, const int displayPositionLeft, const bool selection)
{
	const int clientHeight = GetClientSize().GetHeight();

	if (frame.isValid())
	{
		ocean_assert(frame.pixelFormat() == FrameType::FORMAT_RGB24);
		ocean_assert(frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

		Frame drawFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

		if (selection)
		{
			drawFrame = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

			for (unsigned int y = 0u; y < drawFrame.height(); ++y)
			{
				uint8_t* row = drawFrame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < drawFrame.width(); ++x)
				{
					row[x * 3u] = uint8_t(0xFFu);
				}
			}
		}

		const wxImage windowImage = wxImage(int(frame.width()), int(frame.height()), const_cast<uint8_t*>(drawFrame.constdata<uint8_t>()), true);
		wxBitmap bitmap(windowImage);

		if (displayFrameIndex_)
		{
			wxMemoryDC localDc;
			localDc.SelectObject(bitmap);
			localDc.SetFont(*wxNORMAL_FONT);

			const std::wstring text(L"Frame + " + String::toWString(frameIndex));
			localDc.DrawText(text.c_str(), 2, 2);
		}

		Utilities::bitmapOutput(dc, displayPositionLeft, 0, int(previewWidth_), clientHeight, 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), bitmap);
	}
	else
	{
		const wxPen oldPen(dc.GetPen());
		const wxBrush oldBrush(dc.GetBrush());

		dc.SetPen(wxPen(wxColour(0x000000ul)));
		dc.SetBrush(wxBrush(wxColour(0xCDB400ul)));

		dc.DrawRectangle(displayPositionLeft, 0, int(previewWidth_), clientHeight);

		if (displayFrameIndex_)
		{
			const wxFont oldFont(dc.GetFont());
			dc.SetFont(*wxNORMAL_FONT);

			const std::wstring text(L"Frame + " + String::toWString(frameIndex));
			dc.DrawText(text.c_str(), displayPositionLeft + 2, 2);

			dc.SetFont(oldFont);
		}

		dc.SetPen(oldPen);
		dc.SetBrush(oldBrush);
	}
}

void FramePreviewWindow::onMouseLeftDown(wxMouseEvent& event)
{
	SetFocus();

	bool needUpdate = false;

	const int position = event.GetPosition().x;

	for (Elements::const_iterator i = elements_.begin(); i != elements_.end(); ++i)
	{
		const int displayPositionLeft = i->displayPositionStart(virtualPositionStart_, previewZoom_);

		if (position >= displayPositionLeft && position < displayPositionLeft + int(previewWidth_))
		{
			selectionIndex_ = i->index();
			needUpdate = true;
			break;
		}
	}

	if (!needUpdate)
	{
		needUpdate = selectionIndex_ != (unsigned int)-1;
		selectionIndex_ = (unsigned int)-1;
	}

	if (needUpdate)
	{
		Refresh(false);

		if (selectionCallback_)
		{
			selectionCallback_(selectionIndex_);
		}
	}
}

void FramePreviewWindow::onMouseLeftUp(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseLeftDblClick(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseMiddleDown(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseMiddleUp(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseRightDown(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseRightUp(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseRightDblClick(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onMouseWheel(wxMouseEvent& event)
{
	bool needUpdate = false;

	if (event.GetWheelRotation() < 0)
	{
		if (wxGetKeyState(WXK_CONTROL))
		{
			int virtualMouse = virtualPositionStart_ + event.GetX() * int(previewZoom_);
			int localOffset = virtualMouse % int((previewWidth_ + separatorWidth_) * previewZoom_);

			virtualMouse -= localOffset;
			localOffset /= int(previewZoom_);

			previewZoom_ = max(1u, previewZoom_ / 2u);
			virtualPositionStart_ = virtualMouse + (localOffset - event.GetX()) * int(previewZoom_);
		}
		else
		{
			virtualPositionStart_ += 40 * int(previewZoom_);
		}

		needUpdate = true;
	}

	if (event.GetWheelRotation() > 0)
	{
		if (wxGetKeyState(WXK_CONTROL))
		{
			int virtualMouse = virtualPositionStart_ + event.GetX() * int(previewZoom_);
			int localOffset = virtualMouse % int((previewWidth_ + separatorWidth_) * previewZoom_);

			virtualMouse -= localOffset;
			localOffset /= int(previewZoom_);

			previewZoom_ = min(previewZoom_ * 2u, 0x10000000u);
			virtualPositionStart_ = virtualMouse + (localOffset - event.GetX()) * int(previewZoom_);
		}
		else
		{
			virtualPositionStart_ -= 40 * int(previewZoom_);
		}

		needUpdate = true;
	}

	if (needUpdate)
	{
		Refresh(false);
	}
}

void FramePreviewWindow::onSize(wxSizeEvent& /*event*/)
{
	const int clientHeight = GetClientSize().GetHeight();

	if (clientHeight > 0 && frameProvider_)
	{
		const Media::MovieFrameProvider::Dimension providerPreview(frameProvider_->preferredPreviewDimension());

		if (providerPreview.first != 0u || providerPreview.second < (unsigned int)clientHeight)
		{
			frameProvider_->setPreferredPreviewDimension(0u, (unsigned int)clientHeight);
		}
	}

	updateDispalyParameter();

	Refresh(false);
}

void FramePreviewWindow::onMouseMove(wxMouseEvent& /*event*/)
{
	// nothing to do here
}

void FramePreviewWindow::onEraseBackground(wxEraseEvent& /*event*/)
{
	wxClientDC dc(this);
}

void FramePreviewWindow::onPreviewFrame(const unsigned int /*index*/)
{
	Refresh(false);
}

void FramePreviewWindow::onEvent(const Media::MovieFrameProvider::EventType /*eventType*/, const unsigned long long /*value*/)
{
	newPreviewFrame_ = true;

	Refresh(false);
}

void FramePreviewWindow::updateDispalyParameter()
{
	if (frameProvider_.isNull())
	{
		virtualWidth_ = 0u;
		previewWidth_ = 0u;
		previewHeight_ = 0u;
		return;
	}

	ocean_assert(frameProvider_);

	Media::MovieFrameProvider::Dimension previewDimension(frameProvider_->determinePreviewDimensions());

	const int clientHeight = GetClientSize().GetHeight();
	if (clientHeight <= 0 || previewDimension.first == 0u || previewDimension.second == 0u)
	{
		virtualWidth_ = 0u;
		previewWidth_ = 0u;
		previewHeight_ = 0u;
		return;
	}

	// simple case: client height is identical to preview height
	if ((unsigned int)clientHeight == previewDimension.second)
	{
		previewWidth_ = previewDimension.first;
		previewHeight_ = previewDimension.second;
	}
	else
	{
		const unsigned int scaledPreviewWidth = (previewDimension.first * (unsigned int)(clientHeight) + previewDimension.second / 2u) / previewDimension.second;

		previewWidth_ = scaledPreviewWidth;
		previewHeight_ = (unsigned int)(clientHeight);
	}

	if (frameNumber_ == 0u || previewWidth_ == 0u || previewHeight_ == 0u)
	{
		virtualWidth_ = 0u;
		previewWidth_ = 0u;
		previewHeight_ = 0u;
		return;
	}

	ocean_assert(frameNumber_ > 0u);
	virtualWidth_ = frameNumber_ * (previewWidth_ + separatorWidth_) + separatorWidth_;
}

bool FramePreviewWindow::calculateFirstVisibleElement(const int virtualPositionStart, const unsigned int previewWidth, const unsigned int separatorWidth, const unsigned int zoom, const bool adjust2zoom, int& index)
{
	ocean_assert(previewWidth != 0u);
	ocean_assert(zoom != 0u);

	if (zoom == 0u || previewWidth + separatorWidth == 0u)
	{
		ocean_assert(false && "Invalid parameter");
		return false;
	}

	index = virtualPositionStart / int(previewWidth + separatorWidth);

	if (adjust2zoom)
	{
		if (index >= 0)
		{
			index -= index % int(zoom);
		}
		else
		{
			index -= int(zoom) - (-index % int(zoom));
		}

		ocean_assert(index >= 0 && index % int(zoom) == 0 || index < 0 && -index % int(zoom) == 0);
	}

	return true;
}

bool FramePreviewWindow::calculateLastVisibleElement(const int virtualPositionStart, const unsigned int displayWidth, const unsigned int previewWidth, const unsigned int separatorWidth, const unsigned int zoom, const bool adjust2zoom, int& index)
{
	ocean_assert(previewWidth != 0u);
	ocean_assert(zoom != 0u);

	const int virtualPositionStop = virtualPositionStart + int(displayWidth * zoom);

	if (zoom == 0u || previewWidth + separatorWidth == 0u)
	{
		ocean_assert(false && "Invalid parameter");
		return false;
	}

	// check if no preview element is visible
	if (virtualPositionStop <= int(separatorWidth))
	{
		return false;
	}

	index = (virtualPositionStop - int(separatorWidth)) / int(previewWidth + separatorWidth);

	if (adjust2zoom)
	{
		if (index >= 0)
		{
			index -= index % int(zoom);
		}
		else
		{
			index -= int(zoom) - (-index % int(zoom));
		}

		ocean_assert(index >= 0 && index % int(zoom) == 0 || index < 0 && -index % int(zoom) == 0);
	}

	return true;
}

bool FramePreviewWindow::calculateVirtualPositionStart(const unsigned int index, const unsigned int previewWidth, const unsigned int separatorWidth, const unsigned int zoom, int& position)
{
	// layout with zoom 1: separator | preview0 | separator | preview1 | separator | preview2 | separator | preview3  | separator | preview4  | separator
	// layout with zoom 2: separator | preview0 | separator | preview2 | separator | preview4 | separator | preview6  | separator | preview8  | separator
	// layout with zoom 4: separator | preview0 | separator | preview4 | separator | preview8 | separator | preview12 | separator | preview16 | separator

	ocean_assert(previewWidth != 0u);
	ocean_assert(zoom != 0u);

	if (zoom == 0u || index % zoom != 0u)
	{
		ocean_assert(false && "Invalid parameter!");
		return false;
	}

	position = int(separatorWidth + index * (previewWidth + separatorWidth));
	return true;
}

}

}

}
