/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBBRATION_DETECTOR__WIN_DETECTOR_MAIN_WINDOW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBBRATION_DETECTOR__WIN_DETECTOR_MAIN_WINDOW_H

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemocvliveundistortwin
 */
class DetectorMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Definition of individual application mode.
		 */
		enum ApplicationMode : uint32_t
		{
			/// Detect the points of the calibration board.
			AM_POINT_DETECTION,
			/// Detect the calibration board.
			AM_BOARD_DETECTION,

			// The number of application states.
			AM_END
		};

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param file Optional media file
		 */
		DetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~DetectorMainWindow() override;

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
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 * @param camera The camera profile of the frame, if known
		 */
		void onFrame(const Frame& frame, const SharedAnyCamera& camera);

	protected:

		/// The current application mode.
		ApplicationMode applicationMode_ = AM_POINT_DETECTION;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// True, to save an image.
		bool saveImage_ = false;
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBBRATION_DETECTOR__WIN_DETECTOR_MAIN_WINDOW_H
