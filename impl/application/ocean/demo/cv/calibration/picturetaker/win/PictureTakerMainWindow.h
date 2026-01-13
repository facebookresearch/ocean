/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_WIN_PICTURE_TAKER_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_WIN_PICTURE_TAKER_MAIN_WINDOW_H

#include "application/ocean/demo/cv/calibration/picturetaker/win/PictureTakerMain.h"

#include "ocean/base/Frame.h"

#include "ocean/io/Directory.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window for the PictureTaker application.
 * @ingroup applicationdemocvcalibrationpicturetakerwin
 */
class PictureTakerMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param inputSource The input source to be used, empty to use default fallback (LiveVideoId:1, then LiveVideoId:0)
		 * @param preferredWidth The preferred frame width, in pixels
		 * @param preferredHeight The preferred frame height, in pixels
		 * @param preferredPixelFormat The preferred pixel format, use FORMAT_YUYV16 to avoid compression artifacts
		 * @param outputDirectory The output directory for the captured images, empty to create a local directory
		 */
		PictureTakerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& inputSource, unsigned int preferredWidth, unsigned int preferredHeight, FrameType::PixelFormat preferredPixelFormat, const std::string& outputDirectory);

		/**
		 * Destructs the main window.
		 */
		~PictureTakerMainWindow() override;

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
		 * Repaints the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Event function if a key has been pressed.
		 * @param key The key that has been pressed
		 */
		void onKeyDown(const int key) override;

		/**
		 * Takes a picture and saves it to disk.
		 * @param frame The frame to save
		 * @return True, if succeeded
		 */
		bool takePicture(const Frame& frame);

		/**
		 * Writes the camera settings to a file.
		 * @param frame The current frame
		 */
		void writeCameraSettings(const Frame& frame);

	protected:

		/// The input source to be used.
		std::string inputSource_;

		/// The preferred frame width.
		unsigned int preferredWidth_ = 1920u;

		/// The preferred frame height.
		unsigned int preferredHeight_ = 1080u;

		/// The preferred pixel format.
		FrameType::PixelFormat preferredPixelFormat_ = FrameType::FORMAT_YUYV16;

		/// The live video medium providing the visual input.
		Media::LiveVideoRef liveVideo_;

		/// The timestamp of the most recent input frame.
		Timestamp frameTimestamp_;

		/// The directory in which the pictures will be stored.
		IO::Directory directory_;

		/// The counter for the pictures.
		unsigned int pictureCounter_ = 0u;

		/// The current focus value.
		float currentFocus_ = 0.85f;

		/// True, if the user wants to take an image.
		bool takeImage_ = false;

		/// True, if the settings file has been written.
		bool settingsFileWritten_ = false;

		/// True, if a countdown is currently running.
		bool countdownActive_ = false;

		/// The current countdown value (3, 2, 1, 0).
		int countdownValue_ = 3;

		/// The timestamp when the countdown started or last changed.
		Timestamp countdownTimestamp_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_WIN_PICTURE_TAKER_MAIN_WINDOW_H
