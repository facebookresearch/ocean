/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/Screen.h"

#include <wx/display.h>

#ifdef __APPLE__
	#include <AppKit/AppKit.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

CV::PixelPositionI Screen::suitableWindowPositionApple(const unsigned int windowWidth, const unsigned int windowHeight, const int left, const int top, const OCEAN_WXWIDGETS_HANDLE parent)
{
	ocean_assert(windowWidth >= 1u && windowHeight >= 1u);

	const CV::PixelBoundingBoxI screenBoundingBox(virtualDisplayBoundingBox());

	CV::PixelBoundingBoxI resultBoundingBox(CV::PixelPositionI(left, top), windowWidth, windowHeight);

	if ((resultBoundingBox.left() == NumericT<int>::minValue() || resultBoundingBox.top() == NumericT<int>::minValue()) && parent)
	{
		NSView* nsView = (__bridge NSView*)parent;
		const NSRect rect = [nsView frame];

		resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.origin.x + (rect.size.width) / 2 - int(windowWidth / 2u), rect.origin.y + (rect.size.height) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
	}

	ocean_assert(screenBoundingBox.isValid() && resultBoundingBox.isValid());

	// ensure that at least 50% of the window is visible inside the bounding box of the virtual screen
	if ((screenBoundingBox && resultBoundingBox).size() >= resultBoundingBox.size() / 2u)
		return resultBoundingBox.topLeft();

	// we try again to adjust the position due to the parent window
	if (parent)
	{
		NSView* nsView = (__bridge NSView*)parent;
		const NSRect rect = [nsView frame];

		resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.origin.x + (rect.size.width) / 2 - int(windowWidth / 2u), rect.origin.y + (rect.size.height) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
	}
	else
	{
		int displayNr = wxNOT_FOUND;

		// the After Effects Application
		const NSApplication* app = [NSApplication sharedApplication];

		if (app != nil)
		{
			// the main window is alway the index 0
			const NSWindow* window = [[app windows] objectAtIndex:0];
			if (window != nil)
			{
				// we get the display nr from the center point of the main window

				const NSRect frame = [window frame];
				const wxPoint centerPosition(frame.origin.x + frame.size.width * 0.5, frame.origin.y + frame.size.height * 0.5);

				displayNr = wxDisplay::GetFromPoint(centerPosition);
			}
		}

		// we determine the result bounding box based on the display where the After Effects application lies
		// if the center point lies not on any display we use the primary display which has in wxWidgets always the index 0
		const wxRect rect = wxDisplay(displayNr != wxNOT_FOUND ? displayNr : 0).GetGeometry();

		resultBoundingBox = CV::PixelBoundingBoxI(CV::PixelPositionI(rect.GetX() + (rect.GetWidth()) / 2 - int(windowWidth / 2u), rect.GetY() + (rect.GetHeight()) / 2 - int(windowHeight / 2u)), windowWidth, windowHeight);
	}

	// ensure that at least 50% of the window is visible inside the bounding box of the virtual screen
	if ((screenBoundingBox && resultBoundingBox).size() >= resultBoundingBox.size() / 2u)
		return resultBoundingBox.topLeft();

	return CV::PixelPositionI(screenBoundingBox.left() + int(screenBoundingBox.width() / 2u) - int(windowWidth / 2u), screenBoundingBox.top() + int(screenBoundingBox.height() / 2u) - int(windowHeight / 2u));
}

double Screen::scaleFactorApple(const wxWindow* window)
{
	double factor = 1.0;

	if (window)
		factor = window->GetContentScaleFactor();
	else
	{
		NSScreen* mainScreen = [NSScreen mainScreen];
		ocean_assert(mainScreen);

		factor = [mainScreen backingScaleFactor];
	}

	ocean_assert(factor > 0.0);
	return factor;
}

} // namespace WxWidgets

} // namespace Platform

} // namespace Ocean
