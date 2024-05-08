/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_POINT_MOTION_WIN_POINT_MOTION_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_POINT_MOTION_WIN_POINT_MOTION_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Vector2.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * @ingroup applicationdemocvsynthesis
 * This class implements a main window.
 * @ingroup applicationdemocvsynthesispointmotionwin
 */
class PointMotionMainWindow :
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
		PointMotionMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~PointMotionMainWindow() override;

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
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Current frame pyramid.
		CV::FramePyramid currentFramePyramid_;

		/// Initial frame pyramid.
		CV::FramePyramid initialFramePyramid_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Worker object.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// Initial positions.
		CV::PixelPositions initialPositions_;

		/// Accurate initial positions.
		Vectors2 accurateInitialPositions_;

		/// Previous positions.
		CV::PixelPositions previousPositions_;

		/// Accurate previous positions.
		Vectors2 accuratePreviousPositions_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_POINT_MOTION_WIN_POINT_MOTION_MAINWINDOW_H
