/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Screen.h"

#include <winsock2.h>
#include <windows.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

int Screen::primaryDisplayWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

int Screen::primaryDisplayHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

int Screen::virtualDisplayWidth()
{
	return GetSystemMetrics(SM_CXVIRTUALSCREEN);
}

int Screen::virtualDisplayHeight()
{
	return GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

CV::PixelBoundingBoxI Screen::virtualDisplayBoundingBox()
{
	return CV::PixelBoundingBoxI(CV::PixelPositionI(GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN)), GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
}

CV::PixelPositionI Screen::suitableWindowPosition(const unsigned int windowWidth, const unsigned int windowHeight, const int left, const int top, const HWND parent)
{
	ocean_assert(windowWidth >= 1u && windowHeight >= 1u);
	ocean_assert(parent == nullptr || IsWindow(parent));

	const CV::PixelBoundingBoxI screenBoundingBox(virtualDisplayBoundingBox());

	CV::PixelBoundingBoxI resultBoundingBox(CV::PixelPositionI(left, top), windowWidth, windowHeight);

	if ((resultBoundingBox.left() == NumericT<int>::minValue() || resultBoundingBox.top() == NumericT<int>::minValue()) && parent && IsWindow(parent))
	{
		RECT rect;
		if (GetWindowRect(parent, &rect) == TRUE)
			resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.left + (rect.right - rect.left) / 2 - int(windowWidth / 2u), rect.top + (rect.bottom - rect.top) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
	}

	ocean_assert(screenBoundingBox.isValid() && resultBoundingBox.isValid());

	// ensure that at least 50% of the window is visible inside the bounding box of the virtual screen
	if ((screenBoundingBox && resultBoundingBox).size() >= resultBoundingBox.size() / 2u)
		return resultBoundingBox.topLeft();

	// we try again to adjust the position due to the parent window
	if (parent && IsWindow(parent))
	{
		RECT rect;
		if (GetWindowRect(parent, &rect) == TRUE)
			resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.left + (rect.right - rect.left) / 2 - int(windowWidth / 2u), rect.top + (rect.bottom - rect.top) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
	}

	// ensure that at least 50% of the window is visible inside the bounding box of the virtual screen
	if ((screenBoundingBox && resultBoundingBox).size() >= resultBoundingBox.size() / 2u)
		return resultBoundingBox.topLeft();

	return CV::PixelPositionI(screenBoundingBox.left() + int(screenBoundingBox.width() / 2u) - int(windowWidth / 2u), screenBoundingBox.top() + int(screenBoundingBox.height() / 2u) - int(windowHeight / 2u));
}

unsigned int Screen::screens()
{
	return GetSystemMetrics(SM_CMONITORS);
}

bool Screen::screen(const unsigned int positionX, const unsigned int positionY, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height)
{
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	POINT point = {LONG(positionX), LONG(positionY)};

	if (GetMonitorInfo(MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST), &info) == FALSE)
		return false;

	left = info.rcMonitor.left;
	top = info.rcMonitor.top;
	width = info.rcMonitor.right - info.rcMonitor.left;
	height = info.rcMonitor.bottom - info.rcMonitor.top;
	return true;
}

bool Screen::screen(const HWND window, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height)
{
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &info) == FALSE)
		return false;

	left = info.rcMonitor.left;
	top = info.rcMonitor.top;
	width = info.rcMonitor.right - info.rcMonitor.left;
	height = info.rcMonitor.bottom - info.rcMonitor.top;
	return true;
}

int Screen::dpi(const HDC dc)
{
	int logicalPixelSize = 0;

	if (dc)
	{
		logicalPixelSize = GetDeviceCaps(dc, LOGPIXELSX);
		ocean_assert(logicalPixelSize == GetDeviceCaps(dc, LOGPIXELSY));
	}

	// if either no device context was provided or if the dpi value could not be determined from the window (which can happen at least on Windows 7)
	if (logicalPixelSize == 0)
	{
		const HDC localDC = GetDC(nullptr);

		logicalPixelSize = GetDeviceCaps(localDC, LOGPIXELSX);
		ocean_assert(logicalPixelSize == GetDeviceCaps(localDC, LOGPIXELSY));

		ReleaseDC(nullptr, localDC);
	}

	ocean_assert(logicalPixelSize > 0);
	return logicalPixelSize;
}

double Screen::scaleFactor(const HDC dc)
{
	return double(dpi(dc)) * 0.01041666666666666666667; // 1/96 (96 is the dpi of a 100 percent scaling on Windows platforms)
}

}

}

}
