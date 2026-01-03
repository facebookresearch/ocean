/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEANAPPLICATION_OCEAN_DEMO_TRACKING_SLAMTRACKER_WIN_SLAM_TRACKER_MAIN_WINDOW_H
#define META_OCEANAPPLICATION_OCEAN_DEMO_TRACKING_SLAMTRACKER_WIN_SLAM_TRACKER_MAIN_WINDOW_H

#include "application/ocean/demo/tracking/slam/slamtracker/win/SLAMTrackerMain.h"

#include "application/ocean/demo/tracking/slam/slamtracker/SLAMTrackerWrapper.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackingslamslamtrackerwin
 */
class SLAMTrackerMainWindow :
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
		SLAMTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		virtual ~SLAMTrackerMainWindow();

	protected:

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKey
		 */
		void onKeyDown(const int key) override;

	protected:

		/// The platform independent wrapper for the SLAM tracker.
		SLAMTrackerWrapper slamTrackerWrapper_;

		/// True, if a recording is active.
		bool isRecording_ = false;
};

#endif // META_OCEANAPPLICATION_OCEAN_DEMO_TRACKING_SLAMTRACKER_WIN_SLAM_TRACKER_MAIN_WINDOW_H
