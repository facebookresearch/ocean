/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYTRACKER_WIN_HOMOGRAPHY_TRACKER_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYTRACKER_WIN_HOMOGRAPHY_TRACKER_MAIN_WINDOW_H

#include "application/ocean/demo/tracking/homographytracker/win/HomographyTrackerMain.h"

#include "application/ocean/demo/tracking/homographytracker/HomographyTrackerWrapper.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackinghomographytrackerwin
 */
class HomographyTrackerMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param commandArguments The command arguments used to configure the tracker
		 */
		HomographyTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		virtual ~HomographyTrackerMainWindow();

	protected:

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

	protected:

		/// The platform independent wrapper for the homography tracker.
		HomographyTrackerWrapper homographyTrackerWrapper_;

		/// The recent touch position.
		Vector2 recentTouchPosition_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYTRACKER_WIN_HOMOGRAPHY_TRACKER_MAIN_WINDOW_H
