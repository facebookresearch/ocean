/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_RMV_TRACKER_WIN_RMV_TRACKER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_RMV_TRACKER_WIN_RMV_TRACKER_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/tracking/rmv/RMVFeatureTracker6DOF.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackingrmvtrackerwin
 */
class RMVTrackerMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance The application instance
		 * @param name The name of the main window
		 * @param mediaFilename Optional filename of the media to be used as visual input
		 * @param patternFilename Optional filename of the image to be used as tracking pattern
		 */
		RMVTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFilename = std::string(), const std::string& patternFilename = std::string());

		/**
		 * Destructs the main window.
		 */
		~RMVTrackerMainWindow() override;

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
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// The frame medium providing the visual input.
		Media::FrameMediumRef frameMedium_;

		/// THe timestamp of the most recent input frame.
		Timestamp frameTimestamp_;

		/// The actual tracker.
		Tracking::RMV::RMVFeatureTracker6DOF tracker_;

		/// The random number generator object.
		RandomGenerator randomGenerator_;

		/// An optional filename of a media to be used for the visual input, otherwise live video will be used.
		std::string mediaFilename_;

		/// An optional filename of the image used as tracking pattern.
		std::string patternFilename_;

		/// The camera profile to be used.
		PinholeCamera camera_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_RMV_TRACKER_WIN_RMV_TRACKER_MAINWINDOW_H
