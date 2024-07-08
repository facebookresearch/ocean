/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGE_CONVERTER_WIN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGE_CONVERTER_WIN_MAINWINDOW_H

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemomediaimageconverterwin
 */
class ImageConverterMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param fileInput Optional input media file
		 */
		ImageConverterMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& fileInput = std::string());

		/**
		 * Destructs the main window.
		 */
		~ImageConverterMainWindow() override;

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

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional input media file to be used.
		std::string mediaFileInput_;

		/// Optional output media file to be used.
		std::string mediaFileOutputType_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGE_CONVERTER_WIN_MAINWINDOW_H
