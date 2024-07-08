/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_HARRIS_CORNER_WIN_HARRIS_CORNER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_HARRIS_CORNER_WIN_HARRIS_CORNER_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/HarrisCorner.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorharriscornerwin
 */
class HarrisCornerMainWindow :
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
		HarrisCornerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~HarrisCornerMainWindow() override;

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

		/**
		 * Paints a set of feature points into a given frame.
		 * @param corners The corners to be painted
		 * @param frame The frame in which is painted
		 * @param worker Optional worker object to distribute the function
		 */
		static void paintCorners(const CV::Detector::HarrisCorners& corners, Frame& frame, Worker* worker);

		/**
		 * Paints a set of feature points into a given frame.
		 * @param corners The corners to be painted
		 * @param frame The frame in which is painted
		 * @param firstCorner The first corner to be handled
		 * @param numberCorners The number of corners to be handled
		 */
		static void paintCornersSubset(const CV::Detector::HarrisCorner* corners, Frame* frame, const unsigned int firstCorner, const unsigned int numberCorners);

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

		/// Harris corner strength threshold
		int harrisCornerThreshold_ = 50;

		/// Left sub frame corner.
		unsigned int harrisSubFrameLeft_ = 0u;

		/// Top sub frame corner.
		unsigned int harrisSubFrameTop_ = 0u;

		/// Right sub frame corner.
		unsigned int harrisSubFrameRight_ = 0u;

		/// Bottom sub frame corner.
		unsigned int harrisSubFrameBottom_ = 0u;

		/// Selection index for sub frames.
		unsigned int harrisSubFrameSelectionIndex_ = 0u;

		/// Exact position state.
		bool harrisExactPosition_ = false;

		/// Smoothing state.
		bool harrisSmoothing_ = false;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_HARRIS_CORNER_WIN_HARRIS_CORNER_MAINWINDOW_H
