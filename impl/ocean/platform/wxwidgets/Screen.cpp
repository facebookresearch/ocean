/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/Screen.h"

#include <wx/display.h>

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

int Screen::primaryDisplayWidth()
{
	if (wxDisplay::GetCount() == 0)
	{
		return 0;
	}

	return wxDisplay(0u).GetGeometry().width;
}

int Screen::primaryDisplayHeight()
{
	if (wxDisplay::GetCount() == 0)
	{
		return 0;
	}

	return wxDisplay(0u).GetGeometry().height;
}

int Screen::virtualDisplayWidth()
{
	const CV::PixelBoundingBoxI boundingBox(virtualDisplayBoundingBox());

	if (boundingBox.isValid())
	{
		return int(boundingBox.width());
	}

	return 0;
}

int Screen::virtualDisplayHeight()
{
	const CV::PixelBoundingBoxI boundingBox(virtualDisplayBoundingBox());

	if (boundingBox.isValid())
	{
		return int(boundingBox.height());
	}

	return 0;
}

CV::PixelBoundingBoxI Screen::virtualDisplayBoundingBox()
{
	CV::PixelBoundingBoxI boundingBox;

	for (unsigned int n = 0; n < wxDisplay::GetCount(); ++n)
	{
		const wxRect rect = wxDisplay(n).GetGeometry();

		boundingBox = boundingBox || CV::PixelBoundingBoxI(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	}

	return boundingBox;
}

CV::PixelPositionI Screen::suitableWindowPosition(const unsigned int windowWidth, const unsigned int windowHeight, const int left, const int top, const OCEAN_WXWIDGETS_HANDLE parent)
{
#if defined(OCEAN_PLATFORM_BUILD_APPLE)

	return suitableWindowPositionApple(windowWidth, windowHeight, left, top, parent);

#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)

	ocean_assert(windowWidth >= 1u && windowHeight >= 1u);

	const CV::PixelBoundingBoxI screenBoundingBox(virtualDisplayBoundingBox());

	CV::PixelBoundingBoxI resultBoundingBox(CV::PixelPositionI(left, top), windowWidth, windowHeight);

	if ((resultBoundingBox.left() == NumericT<int>::minValue() || resultBoundingBox.top() == NumericT<int>::minValue()) && parent)
	{
		const HWND handle = (HWND)parent;
		ocean_assert(IsWindow(handle));
		if (IsWindow(handle))
		{
			RECT rect;
			if (GetWindowRect(handle, &rect) == TRUE)
			{
				resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.left + (rect.right - rect.left) / 2 - int(windowWidth / 2u), rect.top + (rect.bottom - rect.top) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
			}
		}
	}

	ocean_assert(screenBoundingBox.isValid() && resultBoundingBox.isValid());

	// ensure that at least 50% of the window is visible inside the bounding box of the virtual screen
	if ((screenBoundingBox && resultBoundingBox).size() >= resultBoundingBox.size() / 2u)
	{
		return resultBoundingBox.topLeft();
	}

	// we try again to adjust the position due to the parent window
	if (parent)
	{
		const HWND handle = (HWND)parent;

		ocean_assert(IsWindow(handle));
		if (IsWindow(handle))
		{
			RECT rect;
			if (GetWindowRect(handle, &rect) == TRUE)
			{
				resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.left + (rect.right - rect.left) / 2 - int(windowWidth / 2u), rect.top + (rect.bottom - rect.top) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
			}
		}
	}

	// ensure that at least 50% of the window is visible inside the bounding box of the virtual screen
	if ((screenBoundingBox && resultBoundingBox).size() >= resultBoundingBox.size() / 2u)
	{
		return resultBoundingBox.topLeft();
	}

	return CV::PixelPositionI(screenBoundingBox.left() + int(screenBoundingBox.width() / 2u) - int(windowWidth / 2u), screenBoundingBox.top() + int(screenBoundingBox.height() / 2u) - int(windowHeight / 2u));

#else

	#warning Missing implementation
	ocean_assert(false && "Missing implementation!");

	return CV::PixelPositionI(0u, 0u);

#endif
}

double Screen::scaleFactor(const wxWindow* window)
{
#if defined(OCEAN_PLATFORM_BUILD_APPLE)

	return scaleFactorApple(window);

#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)

	double factor = 1.0;
	int logicalPixelSize = 0;

	if (window && window->GetHWND())
	{
		logicalPixelSize = GetDeviceCaps((HDC)window->GetHWND(), LOGPIXELSX);
		ocean_assert(logicalPixelSize == GetDeviceCaps((HDC)window->GetHWND(), LOGPIXELSY));
	}

	// if either no window was provided or if the dpi value could not be determined from the window (which can happen at least on Windows 7)
	if (logicalPixelSize == 0)
	{
		HDC dc = GetDC(nullptr);

		logicalPixelSize = GetDeviceCaps(dc, LOGPIXELSX);
		ocean_assert(logicalPixelSize == GetDeviceCaps(dc, LOGPIXELSY));

		ReleaseDC(nullptr, dc);
	}

	factor = double(logicalPixelSize) * 0.01041666666666666666667; // 1/96 (96 is the dpi of a 100 percent scaling on Windows platforms)
	ocean_assert(factor > 0.0);
	return factor;

#else

	#warning Missing implementation.
	ocean_assert(false && "Missing implementation!");

	return 1.0;

#endif
}

}

}

}
