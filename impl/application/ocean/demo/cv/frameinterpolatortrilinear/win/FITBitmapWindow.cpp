/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/frameinterpolatortrilinear/win/FITBitmapWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/platform/win/Utilities.h"

FITBitmapWindow::FITBitmapWindow(HINSTANCE instance, const std::wstring& name, const Callback& callback) :
	Window(instance, name),
	callback_(callback)
{
	// nothing to do here
}

FITBitmapWindow::~FITBitmapWindow()
{
	// nothing to do here
}

void FITBitmapWindow::setFrame(const Frame& frame)
{
	nativeFrame_.copy(frame);
	interpolateNativeFrame();
}

void FITBitmapWindow::modifyWindowClass(WNDCLASSW& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
}

void FITBitmapWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	Window::onResize(clientWidth, clientHeight);
	interpolateNativeFrame();
}

void FITBitmapWindow::onPaint()
{
	if (windowBitmap_.isValid() == false || clientHeight() == 0 || windowBitmap_.height() == 0)
	{
		return;
	}

	BitBlt(dc(), 0, 0, clientWidth(), clientHeight(), windowBitmap_.dc(), 0, 0, SRCCOPY);
}

void FITBitmapWindow::interpolateNativeFrame()
{
	if (!nativeFrame_.isValid() || nativeFrame_.pixelFormat() != FrameType::FORMAT_RGB24)
	{
		return;
	}

	HighPerformanceTimer timer;

	const unsigned int targetWidth = clientWidth();
	const unsigned int targetHeight = clientHeight();

	if (targetWidth != 0u && targetHeight != 0u)
	{
		Frame targetFrame(FrameType(nativeFrame_, targetWidth, targetHeight));

		if (callback_)
		{
			callback_(nativeFrame_, targetFrame);

			const double time = timer.mseconds();

			windowBitmap_.set(targetFrame);
			Platform::Win::Utilities::textOutput(windowBitmap_.dc(), 5, 5, String::toAString(time) + std::string("ms"));
		}
	}
}
