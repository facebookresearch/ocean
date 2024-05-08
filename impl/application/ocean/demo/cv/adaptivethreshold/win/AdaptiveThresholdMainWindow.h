// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADAPTIVE_THRESHOLD_WIN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADAPTIVE_THRESHOLD_WIN_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvadaptivethresholdwin
 */
class AdaptiveThresholdMainWindow :
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
		AdaptiveThresholdMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~AdaptiveThresholdMainWindow() override;

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

		/// Threshold window.
		Platform::Win::BitmapWindow thresholdWindow_;

		/// Threshold window.
		Platform::Win::BitmapWindow thresholdWindowMirrored_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Worker object.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADAPTIVE_THRESHOLD_WIN_MAINWINDOW_H
