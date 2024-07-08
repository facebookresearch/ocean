/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEOPREVIEW_WIN_VIDEO_PREVIEW_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEOPREVIEW_WIN_VIDEO_PREVIEW_MAIN_WINDOW_H

#include "application/ocean/demo/media/videopreview/win/VideoPreviewMain.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemomediavideopreviewwin
 */
class VideoPreviewMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		VideoPreviewMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~VideoPreviewMainWindow() override;

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

		 /// The frame medium providing the visual input.
		Media::FrameMediumRef frameMedium_;

		/// THe timestamp of the most recent input frame.
		Timestamp frameTimestamp_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEOPREVIEW_WIN_VIDEO_PREVIEW_MAIN_WINDOW_H
