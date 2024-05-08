/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/frameinterpolatorbilinear/win/FIBBitmapWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/platform/win/Utilities.h"

FIBBitmapWindow::FIBBitmapWindow(HINSTANCE instance, const std::wstring& name, const CV::PixelCenter pixelCenter) :
	Window(instance, name),
	pixelCenter_(pixelCenter)
{
	// nothing to do here
}

FIBBitmapWindow::~FIBBitmapWindow()
{
	// nothing to do here
}

void FIBBitmapWindow::setFrame(const Frame& frame)
{
	nativeFrame_ = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	interpolateNativeFrame();
}

void FIBBitmapWindow::modifyWindowClass(WNDCLASSW& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
}

void FIBBitmapWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	Window::onResize(clientWidth, clientHeight);
	interpolateNativeFrame();
}

void FIBBitmapWindow::onPaint()
{
	if (bitmap_.isValid() == false || clientHeight() == 0 || bitmap_.height() == 0)
	{
		return;
	}

	BitBlt(dc(), 0, 0, clientWidth(), clientHeight(), bitmap_.dc(), 0, 0, SRCCOPY);
}

void FIBBitmapWindow::interpolateNativeFrame()
{
	if (!nativeFrame_.isValid() || nativeFrame_.pixelFormat() != FrameType::FORMAT_RGB24)
	{
		return;
	}

	HighPerformanceTimer timer;

	const unsigned int targetWidth = clientWidth();
	const unsigned int targetHeight = clientHeight();

	if (targetWidth == 0u || targetHeight == 0u)
	{
		return;
	}

	Frame targetFrame(FrameType(targetWidth, targetHeight, nativeFrame_.pixelFormat(), nativeFrame_.pixelOrigin()));

	const Frame& sourceFrame = nativeFrame_;

	const unsigned int sourceWidth = sourceFrame.width();
	const unsigned int sourceHeight = sourceFrame.height();

	for (unsigned int y = 0u; y < targetHeight; ++y)
	{
		uint8_t* targetPixel = targetFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			if (pixelCenter_ == CV::PC_TOP_LEFT)
			{
				const Scalar xSource = Scalar(x * (sourceWidth - 1u)) / Scalar(targetWidth); // [0, sourceWidth - 1]
				const Scalar ySource = Scalar(y * (sourceHeight - 1u)) / Scalar(targetHeight);

				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(sourceFrame.constdata<uint8_t>(), sourceWidth, sourceHeight, sourceFrame.paddingElements(), Vector2(xSource, ySource), targetPixel);
			}
			else
			{
				const Scalar xSource = Scalar(x * sourceWidth) / Scalar(targetWidth); // [0, sourceWidth]
				const Scalar ySource = Scalar(y * sourceHeight) / Scalar(targetHeight);

				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_CENTER>(sourceFrame.constdata<uint8_t>(), sourceWidth, sourceHeight, sourceFrame.paddingElements(), Vector2(xSource, ySource), targetPixel);
			}

			targetPixel += 3;
		}
	}

	const double time = timer.mseconds();

	bitmap_.set(targetFrame);
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, String::toAString(time) + std::string("ms"));
}
