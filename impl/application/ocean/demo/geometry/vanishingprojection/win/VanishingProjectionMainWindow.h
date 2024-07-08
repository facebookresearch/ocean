/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_GEOMETRY_VANISHING_PROJECTION_WIN_VANISHING_PROJECTION_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_GEOMETRY_VANISHING_PROJECTION_WIN_VANISHING_PROJECTION_MAINWINDOW_H

#include "application/ocean/demo/geometry/vanishingprojection/win/VanishingProjection.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemogeometryvanishingprojection
 */
class VanishingProjectionMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param mediaFile Name of the media file
		 */
		VanishingProjectionMainWindow(HINSTANCE instance, const std::wstring& name, const std::wstring& mediaFile);

		/**
		 * Destructs the main window.
		 */
		~VanishingProjectionMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Event function to repaint the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

	protected:

		/// Pixel positions of the contour.
		CV::PixelPositions pixelPositions_;

		/// Media frame.
		Frame frame_;

		/// Window holding the rectangle rectification result.
		Platform::Win::BitmapWindow rectificationWindowRectangle_;

		/// Window holding the parallelogram rectification result.
		Platform::Win::BitmapWindow rectificationWindowParallelogram_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_GEOMETRY_VANISHING_PROJECTION_WIN_VANISHING_PROJECTION_MAINWINDOW_H
