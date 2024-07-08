/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_BILINEAR_WIN_FIB_BITMAP_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_BILINEAR_WIN_FIB_BITMAP_WINDOW_H

#include "application/ocean/demo/cv/frameinterpolatorbilinear/win/FrameInterpolatorBilinear.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"

#include "ocean/cv/CV.h"

#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/Window.h"

/**
 * This class implements an advanced bitmap window displaying the interpolation.
 * @ingroup applicationdemocvframeinterpolatorbilinearwin
 */
class FIBBitmapWindow : virtual public Platform::Win::Window
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
		 * @param pixelCenter The pixel center to be used during interpolation
		 */
		FIBBitmapWindow(HINSTANCE instance, const std::wstring& name, const CV::PixelCenter pixelCenter);

		/**
		 * Destructs the main window.
		 */
		~FIBBitmapWindow() override;

		/**
		 * Sets the frame for this window.
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

		/// The pixel center to be used
		CV::PixelCenter pixelCenter_ = CV::PC_TOP_LEFT;

		/// Native frame that has to be interpolated.
		Frame nativeFrame_;

		/// Window bitmap
		Platform::Win::Bitmap bitmap_;

		/// Interpolation event callback function.
		Callback callback_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_BILINEAR_WIN_FIB_BITMAP_WINDOW_H
