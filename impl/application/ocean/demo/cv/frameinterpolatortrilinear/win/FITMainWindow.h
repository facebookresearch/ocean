/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_TRILINEAR_WIN_INTERPOLATOR_TRILINEAR_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_TRILINEAR_WIN_INTERPOLATOR_TRILINEAR_MAINWINDOW_H

#include "application/ocean/demo/cv/frameinterpolatortrilinear/win/FrameInterpolatorTrilinear.h"
#include "application/ocean/demo/cv/frameinterpolatortrilinear/win/FITBitmapWindow.h"

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemocvframeinterpolatortrilinearwin
 */
class FITMainWindow :
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
		FITMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~FITMainWindow() override;

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

		/**
		 * Interpolates a source frame with the full border bilinear interpolation function.
		 * @param source The source frame that will be interpolated
		 * @param target Resulting interpolated target frame (this frame must have a valid frame type)
		 */
		void onInterpolateBilinearNativeFrame(const Frame& source, Frame& target);

		/**
		 * Interpolates a source frame with the full border tri-linear interpolation function.
		 * @param source The source frame that will be interpolated
		 * @param target Resulting interpolated target frame (this frame must have a valid frame type)
		 */
		void onInterpolateTrilinearNativeFrame(const Frame& source, Frame& target);

	protected:

		/// Interpolation window for the full border bilinear interpolation function.
		FITBitmapWindow bilinearInterpolationWindow_;

		/// Interpolation window for the full border tri-linear interpolation function.
		FITBitmapWindow trilinearInterpolationWindow_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_INTERPOLATOR_TRILINEAR_WIN_INTERPOLATOR_TRILINEAR_MAINWINDOW_H
