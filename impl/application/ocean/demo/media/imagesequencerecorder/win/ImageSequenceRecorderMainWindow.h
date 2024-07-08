/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGESEQUENCERECORDER_WIN_IMAGE_SEQUENCE_RECORDER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGESEQUENCERECORDER_WIN_IMAGE_SEQUENCE_RECORDER_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/ImageSequenceRecorder.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemomediaimagerecorderwin
 */
class ImageSequenceRecorderMainWindow :
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
		ImageSequenceRecorderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~ImageSequenceRecorderMainWindow() override;

		/**
		 * Sets the prefix of the image filenames.
		 * @param filename Image filename prefix
		 * @return True, if succeeded
		 */
		bool setFilename(const std::string& filename);

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

		/// Frame medium.
		Media::FrameMediumRef frameMedium_;

		/// Frame recorder.
		Media::ImageSequenceRecorderRef imageSequenceRecorder_;

		/// Frame timestamp.
		Timestamp frameTimestamp_;

		/// Frame type.
		FrameType frameType_;

		/// Image filename prefix.
		std::string windowFilename_;

		/// Window lock.
		Lock lock_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGESEQUENCERECORDER_WIN_IMAGE_SEQUENCE_RECORDER_MAINWINDOW_H
