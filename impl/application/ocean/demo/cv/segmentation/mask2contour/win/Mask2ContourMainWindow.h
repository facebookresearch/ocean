/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_MASK2CONTOUR_WIN_MASK2CONTOUR_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_MASK2CONTOUR_WIN_MASK2CONTOUR_MAINWINDOW_H

#include "application/ocean/demo/cv/segmentation/mask2contour/win/Mask2Contour.h"

#include "ocean/base/Frame.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationmask2contourwin
 */
class Mask2ContourMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance The application instance
		 * @param name The name of the main window
		 */
		Mask2ContourMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~Mask2ContourMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for mouse move event.
		 * @see Window::onMouseMove().
		 */
		void onMouseMove(const MouseButton buttons, const int x, const int y) override;

		/**
		 * Event function to repaint the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Draws an elliptic mask.
		 * @param x Horizontal center position in window domain, with range (-infinity, infinity)
		 * @param y Vertical center position in window domain, with range (-infinity, infinity)
		 * @param value The value that will be drawn
		 */
		void drawMask(const int x, const int y, const uint8_t value);

	protected:

		/// Mask frame.
		Frame maskFrame_;

		/// Width of the triangulation area, in pixel.
		const unsigned int areaWidth_ = 800u;

		/// Height of the triangulation area, in pixel.
		const unsigned int areaHeight_ = 800u;

		/// Border window.
		Platform::Win::BitmapWindow borderWindow_;

		/// Contour window.
		Platform::Win::BitmapWindow contourWindow_;

		/// Mask window.
		Platform::Win::BitmapWindow maskWindow_;

		/// Convex mask window.
		Platform::Win::BitmapWindow maskWindowConvex_;

		/// XOR mask window.
		Platform::Win::BitmapWindow maskWindowXOR_;

		/// Triangulated mask window.
		Platform::Win::BitmapWindow maskWindowTriangulated_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_MASK2CONTOUR_WIN_MASK2CONTOUR_MAINWINDOW_H
