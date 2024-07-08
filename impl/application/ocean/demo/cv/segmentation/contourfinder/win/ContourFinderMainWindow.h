/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_FINDER_WIN_CONTOUR_FINDER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_FINDER_WIN_CONTOUR_FINDER_MAINWINDOW_H

#include "application/ocean/demo/cv/segmentation/contourfinder/win/ContourFinder.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationcontourfinderwin
 */
class ContourFinderMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param media Optional media file to be used
		 */
		ContourFinderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media);

		/**
		 * Destructs the main window.
		 */
		~ContourFinderMainWindow() override;

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

		/// Segmentation window .
		Platform::Win::BitmapWindow contourWindow_;

		/// Mask window.
		Platform::Win::BitmapWindow maskWindow_;

		/// Pixel positions of the contour.
		CV::PixelPositions pixelPositions_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Worker object.
		Worker worker_;

		/// Random generator.
		RandomGenerator randomGenerator_;

		/// Optional media file to be used.
		std::string mediaFile;

		/// State to gather new points for the rough contour.
		bool gatherRoughContour_ = false;

		/// State to create a fine contour.
		bool createFineContour_ = false;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_FINDER_WIN_CONTOUR_FINDER_MAINWINDOW_H
