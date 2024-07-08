/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_FILTER_MEDIAN_WIN_FRAME_FILTER_MEDIAN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_FILTER_MEDIAN_WIN_FRAME_FILTER_MEDIAN_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvframefiltermedianwin
 */
class FrameFilterMedianMainWindow :
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
		FrameFilterMedianMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~FrameFilterMedianMainWindow() override;

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

		/// Median window for RGB24 images and filter size 3.
		Platform::Win::BitmapWindow medianWindow_RGB24_Size3_;

		/// Median window for RGB24 images and filter size 21.
		Platform::Win::BitmapWindow medianWindow_RGB24_Size21_;

		/// Median window for Y8 images and filter size 3.
		Platform::Win::BitmapWindow medianWindow_Y8_Size3_;

		/// Median window for Y8 images and filter size 21.
		Platform::Win::BitmapWindow medianWindow_Y8_Size21_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// The worker object used to distribute the computation.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_FILTER_MEDIAN_WIN_FRAME_FILTER_MEDIAN_MAINWINDOW_H
