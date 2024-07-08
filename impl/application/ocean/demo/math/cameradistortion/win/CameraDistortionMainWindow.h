/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_CAMERA_DISTORTION_WIN_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_CAMERA_DISTORTION_WIN_MAIN_WINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemomathcameradistortionwin
 */
class CameraDistortionMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		CameraDistortionMainWindow(HINSTANCE instance, const std::wstring& name);

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

	protected:

		/// Windows that shows the distorted frame.
		Platform::Win::BitmapWindow distortedWindow_;

		/// Windows that shows the distorted frame.
		Platform::Win::BitmapWindow largeDistortedWindowSmallCamera_;

		/// Window that shows the distorted frame.
		Platform::Win::BitmapWindow largeDistortedWindowLargeCamera_;

		/// Window that shows the distorted frame by application of the clamped distort function.
		Platform::Win::BitmapWindow largeDistortedWindowClampedAtBorderLargeCamera_;

		/// Window that shows the distorted frame by application of the damped distort function.
		Platform::Win::BitmapWindow largeDistortedWindowDampedLargeCamera_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_CAMERA_DISTORTION_WIN_MAIN_WINDOW_H
