/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_LIVE_UNDISTORT_WIN_MAINWINDOW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_LIVE_UNDISTORT_WIN_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemocvliveundistortwin
 */
class LiveUndistortMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param file Optional media file
		 * @param resolution Optional preferred resolution, e.g., "1280x720"
		 */
		LiveUndistortMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string(), const std::string& resolution = std::string());

		/**
		 * Destructs the main window.
		 */
		~LiveUndistortMainWindow() override;

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
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame The new frame
		 * @param camera The camera profile of the frame
		 */
		void onFrame(const Frame& frame, const AnyCamera& camera);

	protected:

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// Optional preferred resolution.
		std::string resolution_;

		/// State determining whether the frame timestamp will be ignored.
		bool ignoreTimestamp_ = false;

		/// True, to undistort the frame; False, to keep the origin frame.
		bool undistortFrame_ = true;
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_LIVE_UNDISTORT_WIN_MAINWINDOW_H
