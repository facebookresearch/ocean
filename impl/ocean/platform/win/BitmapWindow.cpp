/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

BitmapWindow::BitmapWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name)
{
	// nothing to do here
}

BitmapWindow::~BitmapWindow()
{
	// nothing to do here
}

void BitmapWindow::setBitmap(const Bitmap& bitmap)
{
	const ScopedLock scopedLock(lock_);

	bitmap_ = bitmap;

	repaint(true /*eraseBackground*/);
}

void BitmapWindow::setFrame(const Frame& frame)
{
	const ScopedLock scopedLock(lock_);

	if (!frame.isValid())
	{
		bitmap_.release();
		repaint(true /*eraseBackground*/);

		return;
	}

	if (bitmap_.width() == 0u || bitmap_.height() == 0u)
	{
		ocean_assert(width() >= clientWidth());
		ocean_assert(height() >= clientHeight());

		resize(width() - clientWidth() + frame.width(), height() - clientHeight() + frame.height());
	}

	Frame rgbFrame;
	if (CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, false, WorkerPool::get().scopedWorker()()))
	{
		const bool setFrameResult = bitmap_.set(rgbFrame);
		ocean_assert_and_suppress_unused(setFrameResult, setFrameResult);

		onResize(clientWidth(), clientHeight());
	}
}

void BitmapWindow::setDisplayMode(const DisplayMode mode)
{
	if (displayMode_ == mode)
	{
		return;
	}

	displayMode_ = mode;

	repaint(true /*eraseBackground*/);
}

void BitmapWindow::adjustToBitmapSize()
{
	if (bitmap_.width() == 0u || bitmap_.height() == 0u)
	{
		return;
	}

	ocean_assert(width() >= clientWidth());
	ocean_assert(height() >= clientHeight());

	stretchLeft_ = 0u;
	stretchTop_ = 0u;

	stretchWidth_ = bitmap_.width();
	stretchHeight_ = bitmap_.height();

	resize(width() - clientWidth() + bitmap_.width(), height() - clientHeight() + bitmap_.height());
}

void BitmapWindow::modifyWindowClass(WNDCLASSW& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
}

void BitmapWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	if (bitmap_.width() == 0 || bitmap_.height() == 0 || clientWidth == 0 || clientHeight == 0)
	{
		stretchWidth_ = 0;
		stretchHeight_ = 0;
		stretchLeft_ = 0;
		stretchTop_ = 0;
		return;
	}

	switch (displayMode_)
	{
		case DM_STRETCH_TO_SCREEN:
		{
			stretchLeft_ = 0u;
			stretchTop_ = 0u;
			stretchWidth_ = clientWidth;
			stretchHeight_ = clientHeight;
			break;
		}

		default:
		{
			ocean_assert(displayMode_ == DM_FIT_TO_SCREEN);

			const float newClientAspect = float(clientWidth) / float(clientHeight);
			const float bitmapAspect = float(bitmap_.width()) / float(bitmap_.height());

			if (newClientAspect > bitmapAspect)
			{
				stretchHeight_ = clientHeight;
				stretchWidth_ = (unsigned int)(stretchHeight_ * bitmapAspect);

				ocean_assert(clientWidth >= stretchWidth_);
				stretchLeft_ = (clientWidth - stretchWidth_) / 2u;

				stretchTop_ = 0u;
			}
			else
			{
				stretchWidth_ = clientWidth;
				stretchHeight_ = (unsigned int)(stretchWidth_ / bitmapAspect);

				stretchLeft_ = 0u;

				ocean_assert(clientHeight >= stretchHeight_);
				stretchTop_ = (clientHeight - stretchHeight_) / 2u;
			}

			break;
		}
	}
}

void BitmapWindow::onPaint()
{
	const ScopedLock scopedLock(lock_);

	if (bitmap_.isValid() == false || clientHeight() == 0u || bitmap_.height() == 0u)
	{
		return;
	}

	if (bitmap_.width() == clientWidth() && bitmap_.height() == clientHeight())
	{
		BitBlt(dc(), 0, 0, clientWidth(), clientHeight(), bitmap_.dc(), 0, 0, SRCCOPY);
	}
	else
	{
		if (stretchLeft_ != 0)
		{
			BitBlt(dc(), 0, 0, stretchLeft_, stretchHeight_, nullptr, 0, 0, BLACKNESS);
			BitBlt(dc(), stretchLeft_ + stretchWidth_, 0, stretchLeft_, stretchHeight_, nullptr, 0, 0, BLACKNESS);
		}
		else
		{
			BitBlt(dc(), 0, 0, stretchWidth_, stretchTop_, nullptr, 0, 0, BLACKNESS);
			BitBlt(dc(), 0, stretchTop_ + stretchHeight_, stretchWidth_, stretchTop_, nullptr, 0, 0, BLACKNESS);
		}

		SetStretchBltMode(dc(), HALFTONE);
		StretchBlt(dc(), stretchLeft_, stretchTop_, stretchWidth_, stretchHeight_, bitmap_.dc(), 0, 0, bitmap_.width(), bitmap_.height(), SRCCOPY);
	}
}

void BitmapWindow::onMouseDoubleClick(const MouseButton button, const int x, const int y)
{
	Window::onMouseDoubleClick(button, x, y);

	adjustToBitmapSize();
}

bool BitmapWindow::window2bitmap(const int windowX, const int windowY, int& bitmapX, int& bitmapY)
{
	if (stretchWidth_ == 0u || stretchHeight_ == 0u)
	{
		return false;
	}

	const int relativeBitmapX = windowX - stretchLeft_;
	const int relativeBitmapY = windowY - stretchTop_;

	const int absoluteBitmapX = relativeBitmapX * int(bitmap_.width()) / int(stretchWidth_);
	const int absoluteBitmapY = relativeBitmapY * int(bitmap_.height()) / int(stretchHeight_);

	bitmapX = absoluteBitmapX;
	bitmapY = absoluteBitmapY;

	return true;
}

}

}

}
