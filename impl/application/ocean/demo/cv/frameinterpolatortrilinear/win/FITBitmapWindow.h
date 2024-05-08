/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_TRILINEAR_WIN_FIT_BITMAP_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_TRILINEAR_WIN_FIT_BITMAP_WINDOW_H

#include "application/ocean/demo/cv/frameinterpolatortrilinear/win/FrameInterpolatorTrilinear.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"

#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/Window.h"

/**
 * This class implements an advanced bitmap window displaying the interpolation.
 * @ingroup applicationdemocvframeinterpolatortrilinearwin
 */
class FITBitmapWindow :
	virtual public Platform::Win::Window
{
	public:

		/**
		 * Definition of callback function for interpolation events.
		 */
		typedef Callback<void, const Frame&, Frame&> Callback;

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param callback Interpolation event callback function
		 */
		FITBitmapWindow(HINSTANCE instance, const std::wstring& name, const Callback& callback);

		/**
		 * Destructs the main window.
		 */
		~FITBitmapWindow() override;

		/**
		 * Sets the frame for this window.
		 * @see BitmapWindow::setFrame().
		 */
		void setFrame(const Frame& frame);

	protected:

		/**
		 * Allows the modification of the window class before registration.
		 * @see Window::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASSW& windowClass) override;

		/**
		 * Function for window resize event.
		 * @see Window::onResize().
		 */
		void onResize(const unsigned int clientWidth, const unsigned int clientHeight) override;

		/**
		 * Repaints the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Interpolates the native frame so that it matches with the client size of this frame.
		 */
		void interpolateNativeFrame();

	protected:

		/// Native frame that has to be interpolated.
		Frame nativeFrame_;

		/// Window bitmap
		Platform::Win::Bitmap windowBitmap_;

		/// Interpolation event callback function.
		Callback callback_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_TRILINEAR_WIN_FIT_BITMAP_WINDOW_H
