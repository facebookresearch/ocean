/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_WIN_POINT_TRACKER_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_WIN_POINT_TRACKER_MAIN_WINDOW_H

#include "application/ocean/demo/tracking/pointtracker/win/PointTrackerMain.h"

#include "application/ocean/demo/tracking/pointtracker/PointTrackerWrapper.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackingpointtrackerwin
 */
class PointTrackerMainWindow :
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
		PointTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		~PointTrackerMainWindow() override;

	protected:

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

	protected:

		// The platform independent wrapper for the point tracker.
		PointTrackerWrapper pointTrackerWrapper_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_WIN_POINT_TRACKER_MAIN_WINDOW_H
