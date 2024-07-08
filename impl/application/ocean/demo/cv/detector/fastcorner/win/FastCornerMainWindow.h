/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FAST_CORNER_WIN_FAST_CORNER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FAST_CORNER_WIN_FAST_CORNER_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorfastcornerwin
 */
class FASTCornerMainWindow :
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
		FASTCornerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~FASTCornerMainWindow() override;

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
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Worker object.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// State determining whether the frame timestamp will be ignored.
		bool ignoreTimestamp_ = false;

		/// Fast corner strength threshold.
		unsigned int fastCornerThreshold_ = 50u;

		/// Number of visible corners.
		unsigned int fastNumberVisible_ = 200u;

		/// Left sub frame corner.
		unsigned int fastSubFrameLeft_ = 0u;

		/// Top sub frame corner.
		unsigned int fastSubFrameTop_ = 0u;

		/// Right sub frame corner.
		unsigned int fastSubFrameRight_ = 0u;

		/// Bottom sub frame corner.
		unsigned int fastSubFrameBottom_ = 0u;

		/// Selection index for sub frames.
		unsigned int fastSubFrameSelectionIndex_ = 0u;

		/// Frame smoothing.
		bool fastCornerFrameSmoothing_ = false;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FAST_CORNER_WIN_FAST_CORNER_MAINWINDOW_H
