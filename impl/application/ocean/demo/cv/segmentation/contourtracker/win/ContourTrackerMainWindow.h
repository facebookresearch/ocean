/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_TRACKER_WIN_CONTOUR_TRACKER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_TRACKER_WIN_CONTOUR_TRACKER_MAINWINDOW_H

#include "application/ocean/demo/cv/segmentation/contourtracker/win/ContourTracker.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/segmentation/ContourTracker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationcontourtrackerwin
 */
class ContourTrackerMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	protected:

		/**
		 * Definition of individual application states.
		 */
		enum ApplicationState
		{
			/// Idle state.
			AS_IDLE,
			/// Gathering rough contour.
			AS_ROUGH_CONTOUR,
			/// Create fine contour from rough contour.
			AS_FINE_CONTOUR,
			/// Contour tracking.
			AS_TRACK_CONTOUR
		};

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param media Optional media file to be used
		 */
		ContourTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media);

		/**
		 * Destructs the main window.
		 */
		~ContourTrackerMainWindow() override;

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
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for mouse move event.
		 * @see Window::onMouseMove().
		 */
		void onMouseMove(const MouseButton buttons, const int x, const int y) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// Application state.
		ApplicationState applicationState_ = AS_IDLE;

		/// Pixel positions of the contour.
		CV::PixelPositions pixelPositions_;

		/// Contour tracker.
		CV::Segmentation::ContourTracker contourTracker_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Random number generator object.
		RandomGenerator randomGenerator_;

		/// Worker object.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_TRACKER_WIN_CONTOUR_TRACKER_MAINWINDOW_H
