/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_BILINEAR_WIN_FIB_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_BILINEAR_WIN_FIB_MAIN_WINDOW_H

#include "application/ocean/demo/cv/frameinterpolatorbilinear/win/FrameInterpolatorBilinear.h"
#include "application/ocean/demo/cv/frameinterpolatorbilinear/win/FIBBitmapWindow.h"

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemocvframeinterpolatorbilinearwin
 */
class FIBMainWindow :
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
		FIBMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~FIBMainWindow() override;

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

		/// Interpolation window for the default interpolation function.
		FIBBitmapWindow interpolationWindow_;

		/// Interpolation window for the full border interpolation function.
		FIBBitmapWindow interpolationFullBorderWindow_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_BILINEAR_WIN_FIB_MAIN_WINDOW_H
