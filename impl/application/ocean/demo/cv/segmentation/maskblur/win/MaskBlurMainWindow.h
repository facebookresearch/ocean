/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_MASKBLUR_WIN_MASK_BLUR_MAIN_WINDOW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_MASKBLUR_WIN_MASK_BLUR_MAIN_WINDOW_H

#include "application/ocean/demo/cv/segmentation/maskblur/win/MaskBlur.h"

#include "ocean/base/Frame.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationmaskblurwin
 */
class MaskBlurMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance The application instance
		 * @param name The name of the main window
		 */
		MaskBlurMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~MaskBlurMainWindow() override;

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
		 * Function for keyboard button down events.
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event function to repaint the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Event functions for drag&drop events for files.
		 * @see Window::onDragAndDrop().
		 */
		void onDragAndDrop(const Files& files) override;

		/**
		 * Draws an elliptic mask.
		 * @param x Horizontal center position in window domain, with range (-infinity, infinity)
		 * @param y Vertical center position in window domain, with range (-infinity, infinity)
		 * @param value The value that will be drawn
		 */
		void drawMask(const int x, const int y, const uint8_t value);

		/**
		 * Blurs the image.
		 */
		bool blurImage();

	protected:

		/// The image.
		Frame image_;

		/// The mask.
		Frame mask_;

		/// True, if the mask has changed.
		bool maskHasChanged_ = false;
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_MASKBLUR_WIN_MASK_BLUR_MAIN_WINDOW_H
