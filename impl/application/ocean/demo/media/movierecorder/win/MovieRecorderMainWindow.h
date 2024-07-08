/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_RECORDER_WIN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_RECORDER_WIN_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/MovieRecorder.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemomediamovierecorderwin
 */
class MovieRecorderMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param fileInput Optional input media file
		 * @param fileOutput Optional output media file
		 */
		MovieRecorderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& fileInput = std::string(), const std::string& fileOutput = std::string());

		/**
		 * Destructs the main window.
		 */
		~MovieRecorderMainWindow() override;

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
		 * @see Window::onKeyDonw().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Toggles the recording state.
		 */
		void toggleRecording();

	protected:

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Target frame type.
		FrameType frameTargetType_;

		/// Movie recorder object.
		Media::MovieRecorderRef movieRecorder_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_ = Timestamp(false);

		/// Worker object.
		Worker worker_;

		/// Optional input media file to be used.
		std::string mediaFileInput_;

		/// Optional output media file to be used.
		std::string mediaFileOutput_;

		/// State determining whether the next frame will be recorded.
		bool recordFrame_ = false;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_RECORDER_WIN_MAINWINDOW_H
