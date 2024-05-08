/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_CAMERA_UNDISTORTION_WIN_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_CAMERA_UNDISTORTION_WIN_MAIN_WINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemomathcameraundistortionwin
 */
class CameraUndistortionMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param mediaFilename The filename of the media file
		 */
		CameraUndistortionMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFilename);

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

	protected:

		/// The frame which will be undistorted.
		Frame distortedFrame_;

		/// The perfect pinhole camera.
		PinholeCamera perfectPinholeCamera_;

		/// The camera profile which is applied.
		PinholeCamera distortedPinholeCamera_;

		/// The update step.
		Scalar updateStep_ = Scalar(0.01);

		/// Windows that shows the un-distorted frame.
		Platform::Win::BitmapWindow undistortedWindow_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_CAMERA_UNDISTORTION_WIN_MAIN_WINDOW_H
