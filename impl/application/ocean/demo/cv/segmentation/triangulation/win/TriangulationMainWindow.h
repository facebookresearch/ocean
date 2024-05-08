/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_TRIANGULATION_WIN_TRIANGULATION_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_TRIANGULATION_WIN_TRIANGULATION_MAINWINDOW_H

#include "application/ocean/demo/cv/segmentation/triangulation/win/Triangulation.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationtriangulation
 */
class TriangulationMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		TriangulationMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~TriangulationMainWindow() override;

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
		 * Function for keyboard button down events.
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

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

		/// Mask frame.
		Frame maskFrame_;

		/// Width of the triangulation area, in pixel.
		unsigned int areaWidth_ = 800u;

		/// Height of the triangulation area, in pixel.
		unsigned int areaHeight_ = 800u;

		/// Toggle visualization statement.
		bool toggleVisualization_ = true;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_TRIANGULATION_WIN_TRIANGULATION_MAINWINDOW_H
