/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_CV_DETECTOR_FAST_COMPARISON_WIN_FAST_COMPARISON_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_CV_DETECTOR_FAST_COMPARISON_WIN_FAST_COMPARISON_MAINWINDOW_H

#include "application/ocean/test/cv/detector/ApplicationTestCVDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationtestcvdetectorfastcomparisonwin
 */
class MainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param file Optional media file
		 */
		MainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~MainWindow() override;

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
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// Median window.
		Platform::Win::BitmapWindow originalWindow_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// State determining whether the frame timestamp will be ignored.
		bool ignoreTimestamp_ = false;

		/// Worker object.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_CV_DETECTOR_FAST_COMPARISON_WIN_FAST_COMPARISON_MAINWINDOW_H
