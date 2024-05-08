/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_SEED_SEGMENTATION_WIN_SS_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_SEED_SEGMENTATION_WIN_SS_MAIN_WINDOW_H

#include "application/ocean/demo/cv/segmentation/seedsegmentation/win/SeedSegmentation.h"

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationseedsegmentationrwin
 */
class SSMainWindow :
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
		SSMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~SSMainWindow() override;

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
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for keyboard button up events.
		 * @see Window::onKeyUp().
		 */
		void onKeyUp(const int key) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// Segmentation window .
		Platform::Win::BitmapWindow segmentationWindow_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// Horizontal seed position.
		unsigned int seedPositionX_ = (unsigned int)(-1);

		/// Vertical seed position.
		unsigned int seedPositionY_ = (unsigned int)(-1);

		/// True, if the segmentation has to be invoked.
		bool invokeSeedSegmentation_ = false;

		/// Area size half, in pixel.
		unsigned int areaSizeHalf_ = 0u;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_SEED_SEGMENTATION_WIN_SS_MAIN_WINDOW_H
