/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CANVASFONTCREATOR_WIN_CANVAS_FONT_CREATOR_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CANVASFONTCREATOR_WIN_CANVAS_FONT_CREATOR_MAIN_WINDOW_H

#include "application/ocean/demo/cv/canvasfontcreator/win/CanvasFontCreator.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvcanvasfontcreatorwin
 */
class CanvasFontCreatorMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow,
	protected CV::Canvas // to get access to the protected sub-classes for Canvas
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 */
		CanvasFontCreatorMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		virtual ~CanvasFontCreatorMainWindow();

	protected:

		/**
		 * Event function to repaint the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Determines the bounding box of a character.
		 * @param frame The frame in which the character is painted, must be valid
		 * @param invalidColor The color of the area outside of the painted character
		 * @return The bounding box of the character
		 */
		static CV::PixelBoundingBox determineCharacterBoundingBox(const Frame& frame, const unsigned char* invalidColor);
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CANVASFONTCREATOR_WIN_CANVAS_FONT_CREATOR_MAIN_WINDOW_H
